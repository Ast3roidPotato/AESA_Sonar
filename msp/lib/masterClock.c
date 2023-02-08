#include "masterClock.h"
#include "../csHFXT.h"

volatile uint64_t clockTick = 0;

void init() {
    // do init things
    configHFXT();
    //Timer_A0 in continuous mode at 1mhz
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;
    

}

static uint64_t now() { return clockTick; }

static void new() {
    struct MasterClock retval;
    retval.now = now;
    init();
    return retval;
}

const struct MasterClockClass MasterClock = {.new = new}; //Initializes MasterClockClass
