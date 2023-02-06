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
#include "lib/servoDriver.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * main.c
 */
void main(void) {
    volatile int i;
    /* Stop Watchdog timer */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    configHFXT();
    // Configure P1.1 for S1 input
    P1->SEL0 &= ~BIT1; // set P1.1 for GPIO
    P1->SEL1 &= ~BIT1;
    P1->DIR &= ~BIT1; // set P1.1 as input
    P1->OUT |= BIT1;  // enable internal pull-up resistor on P1.1
    P1->REN |= BIT1;

    initServoMotor();

    TIMER_A2->CCR[1] = SERVO_MIN_ANGLE;


    while (1) {
    }
}
