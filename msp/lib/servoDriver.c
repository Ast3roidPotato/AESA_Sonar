/*! \file */
/*!
 * servoDriver.c
 *
 * Description: Servo motor driver for MSP432P4111 Launchpad.
 *              Assumes SMCLK configured with 48MHz HFXT as source.
 *              Uses Timer_A2 and P5.6 (TA2.1)
 *
 *  Created on:
 *      Author:
 */

#include "servoDriver.h"
#include "msp.h"

/* Global Variables  */
uint16_t pulseWidthTicks = SERVO_MIN_ANGLE;

void initServoMotor(void) {
    // TODO configure servo pin (P5.6) for primary module function (TA2.1),
    //  output, initially LOW
    P5->SEL0 |= BIT6; // set P5.6 for TA2.1
    P5->SEL1 &= ~BIT6;
    P5->DIR |= BIT6; // set P5.6 as output
    P5->OUT &= ~BIT6; // set P5.6 initially LOW


    /* Configure Timer_A2 and CCR1 */
    // Set period of Timer_A2 in CCR0 register for Up Mode
    TIMER_A2->CCR[0] = SERVO_TMR_PERIOD;
    // Set initial positive pulse-width of PWM in CCR1 register
    TIMER_A2->CCR[1] = SERVO_MIN_ANGLE;

    // configure TA2CCR1 for Compare mode, Reset/Set output mode, with interrupt disabled
    TIMER_A2->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7;

    // Configure Timer_A2 in Up Mode, with source SMCLK, prescale 16:1, and
    //  interrupt disabled  -  tick rate will be 3MHz (for SMCLK = 48MHz)
    // configure Timer_A2 (requires setting control AND expansion register)
    TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP | TIMER_A_CTL_ID__4;
    TIMER_A2->EX0 = TIMER_A_EX0_TAIDEX_4;
}

void incrementTenDegree(void) {
    // update pulse-width for <current angle> + <10 degrees>
    pulseWidthTicks += TEN_DEGREE_TICKS;
    if (pulseWidthTicks > SERVO_MAX_ANGLE) {
        pulseWidthTicks = SERVO_MIN_ANGLE;
    } 
    // update CCR1 register to set new positive pulse-width
    TIMER_A2->CCR[1] = pulseWidthTicks; 

}

void setServoAngle(int8_t angle) {
    // NOT NEEDED FOR EXERCISE - but would be useful function for driver
    //  to set angle to any value
    TIMER_A2->CCR[1] = (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE) * (((double)angle)/180.0);
}
