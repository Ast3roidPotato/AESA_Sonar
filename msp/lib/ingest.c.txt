
// Sample two analog channels 20Hz with Timer A1 Up mode interrupt
#include "msp.h"

#define Frequency20Hz 800   // 25ms*32/ms = 800
#define ADC_START_ADDRESS 1 // Conversion memory 1

// Potentiometer definition
#define PotPort P6
#define PotPin BIT0 // P6.0

// Photocell definition
#define PhotocellPort P6
#define PhotocellPin BIT1 // P6.1

volatile uint16_t POTvalue;       // digital value from POT
volatile uint16_t PhotoCellvalue; // digital value from POT
#define PHOTOCELL_THRETHOLD 2.5 * 4096 / 3.5
enum { CHANGED, NoCHANGE } POTstatus;

// Timer A1 CCR0 interrupt at 20Hz to read POT value
// Configure POT as an analog input
// Advanced: configure Photocell as an analog input. Configure ADC14 to get voltages from two channels
void ConfigureAnalogInputTimerA1(void) {

    PotPort->DIR &= ~PotPin;
    PotPort->SEL0 |= PotPin;
    PotPort->SEL1 |= PotPin;

    PhotocellPort->DIR &= ~PhotocellPin;
    PhotocellPort->SEL0 |= PhotocellPin;
    PhotocellPort->SEL1 |= PhotocellPin;

    /* Configure ADC (CTL0 and CTL1) registers for:
     *      clock source - default MODCLK, clock prescale 1:1,
     *      sample input signal (SHI) source - software controlled (ADC14SC),
     *      Pulse Sample mode with sampling period of 16 ADC14CLK cycles,
     *      Single-channel, single-conversion mode, 12-bit resolution,
     *      ADC14 conversion start address ADC14MEM1, and Low-power mode
     */
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
    //    ADC14->MCTL[2] |= ADC14_MCTLN_EOS;

    /* Configure Timer_A1 and CCRs */
    // Set initial period in CCR0 register. This assumes timer starts at 0
    TIMER_A1->CCR[0] = Frequency20Hz;
    // Configure CCR0 for Compare mode with interrupt enabled (no output mode - 0)
    // TODO configure CCR0
    TIMER_A1->CCTL[0] = 0X0010; // 0b0000 0000 0001 CCIE=1 0000
    // Configure Timer_A1 in UP Mode with source ACLK prescale 1:1 and no interrupt
    // TODO configure Timer_A1: ACLK, UP mode, TACLR=1 bit 2
    TIMER_A1->CTL = 0x0112; // 0b0000 0001 ACLK 0001 UP 0100

    /* Configure global interrupts and NVIC */
    // Enable TA1 TA1CCR0 compare interrupt
    // TODO enable interrupt by setting IRQ bit in NVIC ISER0 register

    NVIC->ISER[0] |= (1) << TA1_0_IRQn;
}

// Timer A1 CCR0 interrupt service routine
// This interrupt occurs at 20Hz to update POT value
void TA1_0_IRQHandler(void) {
    static uint16_t POTvalueOld;
    /* Not necessary to check which flag is set because only one IRQ
     *  mapped to this interrupt vector     */
    if (TIMER_A1->CCTL[0] & TIMER_A_CCTLN_CCIFG) {
        // TODO clear timer compare flag in TA3CCTL0
        TIMER_A1->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag
        POTvalueOld = POTvalue;
        ADC14->CTL0 |= 0b11; // start conversion
        // ADC14_IFGR0_IFG2 is set when ADC14MEM2 is loaded with conversion result
        // The flag is cleared when ADC14MEM2 register is read
        while ((ADC14->IFGR0 & ADC14_IFGR0_IFG2) == 0) {
        };                        // wait for sequence conversion to be over
        POTvalue = ADC14->MEM[1]; // IFG1 flag is cleared
        PhotoCellvalue = ADC14->MEM[2];
        if (POTvalueOld == POTvalue)
            POTstatus = NoCHANGE;
        else
            POTstatus = CHANGED;
    }
}

// Above Code From Teams
//  Below Code exposes it to the world

#include "ingest.h"

static uint16_t *values[] = {&POTvalue, &PhotoCellvalue};

static int analogRead(int pin) {
    // return *values[pin];
    if (pin == 0) {
        return POTvalue;
    } else if (pin == 1) {
        return PhotoCellvalue;
    } else {
        return 0;
    }
}

static void init() { ConfigureAnalogInputTimerA1(); }

static struct Ingest new() {
    struct Ingest ingest;
    ingest.analogRead = analogRead;
    ingest.init = init;
    init();
    return ingest;
}

const struct IngestClass Ingest = {.new = new};
