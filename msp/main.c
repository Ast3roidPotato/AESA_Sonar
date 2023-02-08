/*! \file */
/******************************************************************************
 * MSP432 Lab Exercise 5-2
 *
 * Description: Using timers to generate PWM signals to drive a servo motor
 *
 * Author: Jacob Consalvi
 * Last-modified:
 *
 * An external HF crystal between HFXIN & HFXOUT is required for MCLK,SMCLK
 *
 *                 MSP432P411x
 *             -------------------
 *         /|\|                   |
 *          | |              P1.1 |<---- S1
 *          --|RST                |
 *            |      (TA2.1) P5.6 |----> Servo
 *            |                   |
 *            |              PJ.2 |------
 *            |                   |     |
 *            |                   |    HFXT @ 48MHz
 *            |                   |     |
 *            |              PJ.3 |------
 *
 *******************************************************************************/
#include "msp.h"

/* Standard Includes */
#include "csHFXT.h"
#include "lib/masterClock.h"
// #include "lib/serial.h"
#include "lib/servoDriver.h"
#include "lib/transmitter.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * main.c
 */
void main(void) {
    volatile int i;
    /* Stop Watchdog timer */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    struct MasterClock clock = MasterClock.new();
    // struct Serial serial = Serial.new();

    // serial.println("Starting up...");

    // Configure pin 5.6 to be digital output GPIO
    P5->SEL0 = 0;
    P5->SEL1 = 0;
    P5->DIR = 0xFF;
    P5->OUT = 0;

    int pulseDelay = 400000; // assume ~0.25us per tick
    int lastPulseTime = 0;
    int pulseCount = 0;
    int pulseCountTarget = 20;

    uint32_t oldTime = 0;
    uint32_t currentTime;

    // serial.println("Starting loop...");

    while (1) {
        currentTime = clock.now();
        if (currentTime - lastPulseTime > pulseDelay) {
            while (pulseCount < pulseCountTarget) {
                currentTime = clock.now();
                if (currentTime - oldTime > 46) {
                    P5->OUT ^= BIT6;
                    oldTime = currentTime;
                    pulseCount++;
                }
            }
            pulseCount = 0;
            lastPulseTime = currentTime;
        }
    }
}
