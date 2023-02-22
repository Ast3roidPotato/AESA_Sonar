/* Final ECE230 Project: MSP serial.c
Authors: ECE230 example code, Jacob Consolvi, Evelyn Elmer, and Abigail Kurfman
Description: This code sets up and runs a UART communication rptotocal with the ESP. Some code from Ex602 is used, 
and some was modified for the project applications. 
Last Revised: 2/21/23 
*/
#include "serial.h"
#include "msp.h"
#include <stdarg.h>
#include <stdlib.h>

// UART A0 IO pins
#define UARTA0port P1
#define UARTA0pins BIT2 | BIT3

// UART A0 IO pins on MSP432P4111 chip
#define UARTA0port P1
#define UARTA0pins BIT2 | BIT3

// UART A1 IO pins
#define UARTA1port P2
#define UARTA1pins BIT2 | BIT3

#define SYSTEMCLOCK 12000000 // Hz
#define BAUDRATE 38400       // bits per seconds (Hz)
#define ClockPrescalerValue SYSTEMCLOCK / (16 * BAUDRATE)
#define FirstModulationStage 10    // 48MHz, 38400 Baud
#define SecondModulationStage 0xfd // 48MHz, 38400 Baud

// configure UART EUSCI_A0
void ConfigureUART_A0(void) {
    /* Configure UART pins */
    UARTA0port->SEL0 |= UARTA0pins; // set 2-UART pins as secondary function
    UARTA0port->SEL1 &= ~(UARTA0pins);

    /* Configure UART
     *  Asynchronous UART mode, 8O1 (8-bit data, Even parity, 1 stop bit),
     *  LSB first by default, SMCLK clock source
     */
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset to configure eUSCI
                                            // bit 15 = 1 to enable parity; bit14=0 Old parity; bit13=0 for LSB first;
    // bit12=0 for 8-bit mode; bit11=0 for one stop bit; bits7-6 = 0b10 for SMCLK
    EUSCI_A0->CTLW0 |= 0b1100000010000000;
    // enable even parity, LSB first, 8-bit mode, one stop bit, SMCLK clock source
    //  EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SSEL__SMCLK | 1 << 14;
    /* Baud Rate calculation
     * Refer to Section 24.3.10 of Technical Reference manual
     * BRCLK = 12000000, Baud rate = 38400
     */
    EUSCI_A0->BRW = ClockPrescalerValue * 1.05;

    EUSCI_A0->MCTLW = (SecondModulationStage << 8) + (FirstModulationStage << 4) + 1; // enalble oversampling

    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;     // Clear eUSCI RX interrupt flag

} // end ConfigureUART_A0(void)

static void init() {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    /* Configure MCLK/SMCLK source to DCO, with DCO = 12MHz */
    CS->KEY = CS_KEY_VAL;         // Unlock CS module for register access
    CS->CTL0 = 0;                 // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3; // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 |   // Select ACLK = REFO
               CS_CTL1_SELS_3 |   // SMCLK = DCO
               CS_CTL1_SELM_3;    // MCLK = DCO
    CS->KEY = 0;                  // Lock CS module from unintended accesses

    ConfigureUART_A0();
}

// UART interrupt service routine
void EUSCIA0_IRQHandler(void) {
    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG) {
        // Check if the TX buffer is empty first
        while (!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG))
            ;

        // Echo the received character back
        //  Note that reading RX buffer clears the flag and removes value from buffer
        EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;
    }
}

// print char to UART pin
static void printChar(char c) {
    // Wait for TX buffer to be empty
    while (!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG))
        ;

    // Send character
    EUSCI_A0->TXBUF = c;
}

// split up string to print to UART pin
static void print(char *string, ...) {
    char buffer[100];
    char *ptr = buffer;
    va_list args;
    va_start(args, string);
    vsprintf(buffer, string, args);
    va_end(args);

    while (*ptr) {
        printChar(*(ptr++));
    }
}

// prints the string, then adds a new line after
static void println(char *string, ...) {
    print(string);
    printChar('\r');
    printChar('\n');
}

// reads char data from the UART pin
static void readChar() {
    char receivedChar;
    if ((EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG) == EUSCI_A_IFG_RXIFG) {
        receivedChar = EUSCI_A0->RXBUF;
    } else {
        receivedChar = NULL;
    }
    return receivedChar;
}

// constructors for serial class
static struct Serial new() {
    init();
    return (struct Serial){
        .print = &print,
        .printChar = &printChar,
        .println = &println,
        .readChar = &readChar,
    };
}

const struct SerialClass Serial = {
    .new = &new,
};
