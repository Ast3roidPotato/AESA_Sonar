#include "masterClock.h"
#include "../csHFXT.h"
// #include "serial.h"

static volatile uint32_t overFlows = 0;
static volatile uint32_t currTime = 0;

// static struct Serial serial;

uint16_t getCurrentTimerValue() { return TIMER_A0->R; }

static void init() {
    // do init things
    configHFXT();
    // serial = Serial.new();

    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR | TIMER_A_CTL_IE | TIMER_A_CTL_ID__4;
    TIMER_A0->EX0 = TIMER_A_EX0_IDEX__3;
    TIMER_A0->CCR[0] = 0xFFFF;
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn)&31);

    __enable_irq();
}

void TA0_N_IRQHandler() {
    // serial.println("tick from interupt");
    TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;
    currTime += 0xFFFF;
}

// Gives time since power on in quarter microsecond increments
static uint32_t now() {
    return (currTime + TIMER_A0->R);
}

static struct MasterClock new() {
    struct MasterClock retval;
    retval.now = now;
    init();
    return retval;
}

const struct MasterClockClass MasterClock = {.new = new}; // Initializes MasterClockClass
