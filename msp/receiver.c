#include "receiver.h"
#include "msp.h"

int echoTime = 0;

void initReceiver() {
    initListenPin();
    initCaptureTimer();
    initCapturePin();
}

void initListenPin() {
    LISTEN_PIN_PORT -> DIR &= ~(LISTEN_PIN);
    LISTEN_PIN_PORT -> SEL0 &= ~(LISTEN_PIN);
    LISTEN_PIN_PORT -> SEL1 &= ~(LISTEN_PIN);
}

void initCapturePin() {
    CAPTURE_PIN_PORT -> DIR &= ~(LISTEN_PIN);
    LISTEN_PIN_PORT -> SEL0 &= ~(LISTEN_PIN);
    LISTEN_PIN_PORT -> SEL1 &= ~(LISTEN_PIN);
}

void initCaptureTimer() {
    TIMER_A0 -> CTL = 0x0;                  // Enable Timer A0 w /1, in stop mode
    TIMER_A0 -> CCTL[0] = 0x4111;           // Enable capture mode with inturrupt, 7.3
    NVIC->ISER[0] |= 1<<9;                  // Enable interrupt
   __enable_irq();                          // Enable global interrupt
}

void startTimer() {
    TIMER_A0 -> CTL |= BIT4;
}

void stopTimer() {
    TIMER_A0 -> CTL &= ~BIT4;
}

int getEchoTime() {
    return echoTime;
}

void TA0_0_IRQHandler(void) {
    stopTimer();
    echoTime = TIMER_A0->CCR[0];
    TIMER_A0->CTL &= ~(BIT0); // clear flag
}
