/*
Final ECE230 Project: MSP reciever.c
Authors: Jacob Consolvi, Evelyn Elmer, and Abigail Kurfman
Description: Source code to run the reciever pin, timer, and handle calculations. This file sets up pin 3.0 to listen
 * for the echo, starts a timer to count how long the pulse width is, and calculates distance. 
Last Revised: 2/21/23 
*/
#include "receiver.h"
#include "msp.h"
#include "csHFXT.h"

volatile int echoTime = 0;
int overflow = 1;

// set up pins and timer clock to capture positive pulse width
void initReceiver() {
    configHFXT();               // confiure SMCLK to 48 MHz
    initListenPin();    
    initCaptureTimer();         // sets up timer to correct presets
    initCapturePin();           // configures Digital I/O pin
}

// extra method -- NOT USED IN FINAL PROJECT
void initListenPin() {
    LISTEN_PIN_PORT -> DIR &= ~(LISTEN_PIN);
    LISTEN_PIN_PORT -> SEL0 &= ~(LISTEN_PIN);
    LISTEN_PIN_PORT -> SEL1 &= ~(LISTEN_PIN);
}

// Sets pin to be a Digital input 
void initCapturePin() {
    CAPTURE_PIN_PORT -> DIR &= ~(LISTEN_PIN);
    CAPTURE_PIN_PORT -> SEL0 &= ~(LISTEN_PIN);
    CAPTURE_PIN_PORT -> SEL1 &= ~(LISTEN_PIN);
}

// Configures timer SLMCLK in stop mode
void initCaptureTimer() {
    TIMER_A1 -> CTL = 0b1011000010;                  // Enable Timer A1 in stop mode
    TIMER_A1 -> CCTL[1] = 0b0100000100010000;        // Enable capture mode with inturrupt
    TIMER_A1 -> EX0 = 0b11;                          // final prescaler of 1:16
    NVIC->ISER[0] |= 1<<11 | 1<<10;                  // Enable interrupt
   __enable_irq();                                   // Enable global interrupt
}

// Starts timer in continuous mode
void startTimer() {
    TIMER_A1 -> CTL |= BIT5;
}

// stops timer and stores time it counted. If overflow, add correction time
void stopTimer() {
    TIMER_A1 -> CTL &= ~BIT5;
    uint32_t rawTime = TIMER_A1->R;
    echoTime = rawTime + (0xFFFD*overflow);
}

// getter for Echotime, also sets timer counter to 0 and resets overflow var
int getEchoTime() {
    TIMER_A1->R = 0;
    overflow = 0;
    return echoTime;
}

// Calculates distance based off of the current echo time
int getDistance() {
    int dis = (SPEED_OF_SOUND*echoTime) / TIMER_TICKS_SEC;
    int actualDis = (int) ( (float) dis*1.27*3.0);          // calibration numbers for enviorment
    return actualDis;
}

// Capture inturrupt handler, NOT USED IN FINAL PROJECT
void TA1_0_IRQHandler() {
    stopTimer();
    echoTime = TIMER_A1->CCR[0];
    TIMER_A1->CCTL[1] &= ~(BIT0); // clear flag
}

// Inturrupt if timer overflows, adds one to overflow count to factor into distance calc
void TA1_N_IRQHandler() {
    overflow += 1;
    TIMER_A1->CTL &= ~(BIT0); // clear flag
}
