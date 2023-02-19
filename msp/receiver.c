#include "receiver.h"
#include "msp.h"
#include "csHFXT.h"

volatile int echoTime = 0;
int overflow = 0;


void initReceiver() {
    configHFXT();
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
    CAPTURE_PIN_PORT -> SEL0 &= ~(LISTEN_PIN);
    CAPTURE_PIN_PORT -> SEL1 &= ~(LISTEN_PIN);
}

void initCaptureTimer() {
    TIMER_A1 -> CTL = 0b1011000010;                  // Enable Timer A0 w /1, in stop mode
    TIMER_A1 -> CCTL[1] = 0b0100000100010000;        // Enable capture mode with inturrupt, 7.3
    TIMER_A1 -> EX0 = 0b11;
    NVIC->ISER[0] |= 1<<11 | 1<<10;                           // Enable interrupt
   __enable_irq();                                   // Enable global interrupt
}

void startTimer() {
    TIMER_A1 -> CTL |= BIT5;
}

void stopTimer() {
    TIMER_A1 -> CTL &= ~BIT5;
    int rawTime = TIMER_A1->R;
    echoTime = rawTime + (0xFFFF*overflow);
}

int getEchoTime() {
    return echoTime;
}

int getDistance() {
    return (SPEED_OF_SOUND*echoTime)/TIMER_TICKS_SEC;
}

void TA1_0_IRQHandler() {
    stopTimer();
    echoTime = TIMER_A1->CCR[0];
    TIMER_A1->CCTL[1] &= ~(BIT0); // clear flag
}

void TA1_N_IRQHandler() {
    overflow += 1;
    TIMER_A1->CTL &= ~(BIT0); // clear flag
}
