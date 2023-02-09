#include "ADC.h"

static volatile int currentPotVal = 0; // holds potentiometer input, updates when read

static void init(uint32_t port, uint16_t pinNum) {
    // do init things

    ((DIO_PORT_Odd_Interruptable_Type *)port)->DIR &= ~(1 << pinNum);
    ((DIO_PORT_Odd_Interruptable_Type *)port)->SEL0 |= (1 << pinNum);
    ((DIO_PORT_Odd_Interruptable_Type *)port)->SEL1 |= (1 << pinNum);

    // P5->SEL0 |= BIT4;
    // P5->SEL1 |= BIT4;

    ADC14->CTL0 = ADC14_CTL0_SHP            // Pulse Sample Mode
                  | ADC14_CTL0_SHT0__16     // 16 cycle sample-and-hold time (for ADC14MEM1)
                  | ADC14_CTL0_PDIV__1      // Predivide by 1
                  | ADC14_CTL0_DIV__1       // /1 clock divider
                  | ADC14_CTL0_SHS_0        // ADC14SC bit sample-and-hold source select
                  | ADC14_CTL0_SSEL__MODCLK // clock source select MODCLK
                                            //                  | ADC14_CTL0_CONSEQ_0       // Single-channel, single-conversion mode
                  | ADC14_CTL0_ON;          // ADC14 on

    ADC14->CTL0 |= ADC14_CTL0_MSC; /*!< ADC14 multiple sample and conversion */

    ADC14->CTL0 |= ADC14_CTL0_CONSEQ_1; // Sequence of channels

    ADC14->CTL1 = ADC14_CTL1_RES__12BIT               // 12-bit conversion results
                  | (0x1 << ADC14_CTL1_CSTARTADD_OFS) // ADC14MEM1 - conversion start address
                  | ADC14_CTL1_PWRMD_2;               // Low-power mode

    // TODO Configure ADC14MCTL1 as storage register for result
    //          Single-ended mode with Vref+ = Vcc and Vref- = Vss,
    //          Input channel - A15, and comparator window disabled
    ADC14->MCTL[1] = 0x000F; // channel bits 4-0 = 0b01111 for A15, P6.0
    ADC14->MCTL[2] = 0x000E; // channel bits 4-0 = 0b10000 for A14, P6.1

    ADC14->MCTL[2] |= 0b10000000; // turn on End of Sequence bit

    // ADC14->CTL0 = ADC14_CTL0_SHP            // Pulse Sample Mode
    //               | ADC14_CTL0_SHT0__16     // 16 cycle sample-and-hold time (for ADC14MEM1)
    //               | ADC14_CTL0_PDIV__1      // Predivide by 1
    //               | ADC14_CTL0_DIV__1       // /1 clock divider
    //               | ADC14_CTL0_SHS_0        // ADC14SC bit sample-and-hold source select
    //               | ADC14_CTL0_SSEL__MODCLK // clock source select MODCLK
    //               | ADC14_CTL0_CONSEQ_0     // Single-channel, single-conversion mode
    //               | ADC14_CTL0_ON;          // ADC14 on

    // ADC14->CTL1 = ADC14_CTL1_RES__12BIT               // 12-bit conversion results
    //               | (0x1 << ADC14_CTL1_CSTARTADD_OFS) // ADC14MEM1 - conversion start address
    //               | ADC14_CTL1_PWRMD_2;               // Low-power mode

    // ADC14->MCTL[1] = 0x1; // Configure ADC14MCTL1 as storage register for result

    // ADC14->CTL0 |= 0x3; // start sampling
}

static void readPot() { currentPotVal = ADC14->MEM[1]; }

// period is 25us
// range from -250 to +250, max Digital Val = 4095
static int getPhase() {
    readPot();
    // float angle = (8.19 * currentPotVal) - 250.0;
    // return (int)angle;
    return currentPotVal;
}

static struct ADC new(uint32_t port, uint16_t pinNum) {
    struct ADC retval;          // create ADC object
    retval.port = port;         // port of ADC object
    retval.pinNum = pinNum;     // pin ADC is in
    retval.readPot = readPot;   // readPot method
    retval.getPhase = getPhase; // get phase angle from pot pos
    init(port, pinNum);         // initialized ADC registers
    return retval;
}

const struct ADCClass ADC = {.new = new};
