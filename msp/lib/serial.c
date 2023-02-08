// File Name: ece230_03_04ex0602template.c
// ece230ex0602 template file by Jianjian Song 1-22-2023
/*! \file */
/******************************************************************************
 * MSP432 Lab Exercise 6-2 - eUSCI_A0 UART echo at 57600 baud using BRCLK = 12MHz
 * ECE230-03, -04 Winter 2022-2023
 * Description: This demo echoes back characters received via a PC serial port.
 *      SMCLK/ DCO is used as a clock source. The auto-clock enable feature is used
 *      by the eUSCI and SMCLK is turned off when the UART is idle and turned on when
 *      a receive edge is detected.
 *
 * Author:
 * Last-modified:
 *
 *                 MSP432P411x
 *             -------------------
 *         /|\|                   |
 *          | |                   |
 *          --|RST                |
 *            |      P1.3/UCA0TXD |----> PC (echo)
 *            |      P1.2/UCA0RXD |<---- PC
 *            |                   |
 *            |                   |
 *
 *******************************************************************************/
#include "serial.h"
#include "msp.h"
#include <stdarg.h>
#include <stdlib.h>

// UART A0 IO pins
//  P1.3/UCA0TXD |----> RX PC (echo)
//   P1.2/UCA0RXD |<---- TX PC
#define UARTA0port P1
#define UARTA0pins BIT2 | BIT3

// UART A0 IO pins on MSP432P4111 chip
//  P1.3/UCA0TXD |----> RX PC (echo)
//   P1.2/UCA0RXD |<---- TX PC
#define UARTA0port P1
#define UARTA0pins BIT2 | BIT3

// UART A1 IO pins
//  P2.3/UCA1TXD |----> RX PC (echo)
//   P2.2/UCA1RXD |<---- TX PC
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
     * BRCLK = 12000000, Baud rate = 57600
     *
     * TODO calculate N and determine values for UCBRx, UCBRFx, and UCBRSx
     *          values used in next two TODOs
     */
    EUSCI_A0->BRW = ClockPrescalerValue * 1.05;

    // TODO set clock prescaler in eUSCI_A0 baud rate control register
    EUSCI_A0->MCTLW = (SecondModulationStage << 8) + (FirstModulationStage << 4) + 1; // enalble oversampling
    // TODO configure baud clock modulation in eUSCI_A0 modulation control register

    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;     // Clear eUSCI RX interrupt flag
    //    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;            // Enable USCI_A0 RX interrupt

    //    NVIC->ISER[0] |= (1)<<EUSCIA0_IRQn;
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
    /* Configure UART pins */
    // P1->SEL0 |= BIT2 | BIT3; // set 2-UART pins as secondary function
    // P1->SEL1 &= ~(BIT2 | BIT3);

    // /* Configure UART
    //  *  Asynchronous UART mode, 8O1 (8-bit data, odd parity, 1 stop bit),
    //  *  LSB first, SMCLK clock source
    //  */
    // EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset

    // // Enable odd parity; LSB first; 8-bit data; one stop bit;  UART mode; Asynchronous mode; SMCLK
    // // bit 15 = 1 to enable parity; bit14=0 Old parity; bit13=0 for LSB first;
    // // bit12=0 for 8-bit mode; bit11=0 for one stop bit; bits 10-9=00 for UART mode
    // // bit8=0 for asynchronous mode;    bits7-6 = 0b10 or 0b11 for SMCLK
    // //  TODO complete configuration of UART in eUSCI_A0 control register
    // // UCACTLW0 = UCPEN | UCPAR | UCSSEL_2 | USSEL__2;
    // EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_PEN | EUSCI_A_CTLW0_SSEL__SMCLK;
    // /* Baud Rate calculation
    //  * Refer to Section 24.3.10 of Technical Reference manual
    //  * BRCLK = 12000000, Baud rate = 57600
    //  *
    //  * TODO calculate N and determine values for UCBRx, UCBRFx, and UCBRSx
    //  *          values used in next two TODOs
    //  */
    // // TODO assign clock prescaler in eUSCI_A0 baud rate control register
    // EUSCI_A0->BRW = ;//13;

    // // TODO configure baud clock 1st and 2nd stage modulation in eUSCI_A0 modulation control register

    // EUSCI_A0->MCTLW = 37;

    // EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    // EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;     // Clear eUSCI RX interrupt flag
    // // EUSCI_A0->IE |= EUSCI_A_IE_RXIE;         // Enable USCI_A0 RX interrupt

    // // Enable global interrupt
    // // __enable_irq();

    // // Enable eUSCIA0 interrupt in NVIC module
    // // NVIC->ISER[0] = (1 << EUSCIA0_IRQn);
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

static void printChar(char c) {
    // Wait for TX buffer to be empty
    while (!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG))
        ;

    // Send character
    EUSCI_A0->TXBUF = c;
}

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

static void println(char *string, ...) {
    print(string);
    printChar('\r');
    printChar('\n');
}

static void readChar() {
    char receivedChar;
    if ((EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG) == EUSCI_A_IFG_RXIFG) {
        receivedChar = EUSCI_A0->RXBUF;
    } else {
        receivedChar = NULL;
    }
    return receivedChar;
}

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
