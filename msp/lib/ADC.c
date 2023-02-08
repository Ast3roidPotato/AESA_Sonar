#include "ADC.h"
#include <std.h>

volatile int currentPotVal = 0;                 // holds potentiometer input, updates when read

void init() {
    // do init things
    ADC14->CTL0 = ADC14_CTL0_SHP                // Pulse Sample Mode
                    | ADC14_CTL0_SHT0__16       // 16 cycle sample-and-hold time (for ADC14MEM1)
                    | ADC14_CTL0_PDIV__1        // Predivide by 1
                    | ADC14_CTL0_DIV__1         // /1 clock divider
                    | ADC14_CTL0_SHS_0          // ADC14SC bit sample-and-hold source select
                    | ADC14_CTL0_SSEL__MODCLK   // clock source select MODCLK
                    | ADC14_CTL0_CONSEQ_0       // Single-channel, single-conversion mode
                    | ADC14_CTL0_ON;            // ADC14 on

    ADC14->CTL1 = ADC14_CTL1_RES__12BIT         // 12-bit conversion results
            | (0x1 << ADC14_CTL1_CSTARTADD_OFS) // ADC14MEM1 - conversion start address
            | ADC14_CTL1_PWRMD_2;               // Low-power mode

    ADC14->MCTL[1] = 0x1;                       // Configure ADC14MCTL1 as storage register for result              

    ADC14->CTL0 |= 0x3;                         // start sampling

}

static void new(uint32_t port, uint16_t pinNum) {
    struct ADC retval;                          // create ADC object
    ADC.port = port;                            // port of ADC object
    ADC.pinNum = pinNum;                        // pin ADC is in
    ADC.readPot = readPot;                      // readPot method
    ADC.getPhase = getPhase;                    // get phase angle from pot pos
    init();                                     // initialized ADC registers 
    return retval;
}

static void readPot() {
    currentPotVal = ADC14->MEM[1];
}

// period is 25us 
// range from -250 to +250, max Digital Val = 4095
static int getPhase() {
    readPot();
    float angle = 8.19*currentPotVal - 250.0
    return (int) angle;
}


const struct ADCClass ADC = {.new = new};
