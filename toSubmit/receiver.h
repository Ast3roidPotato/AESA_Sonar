/*! \file */
/*!
 * transmitter.h
 *
 * Description: Headerfile to run the reciever pin, timer, and handle calculations. This file sets up pin 3.0 to listen
 * for the echo, starts a timer to count how long the pulse width is, and calculates distance. 
 *
 *  Created on: 02/12/23
 *      Author: ECE230 example code, Jacob Consolvi, Evelyn Elmer, and Abigail Kurfman
 */

#ifndef RECEIVER_H_
#define RECEIVER_H_

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include "msp.h"
#include "csHFXT.h"

#define LISTEN_PIN_PORT P1
#define LISTEN_PIN BIT5 // P1.1
#define CAPTURE_PIN_PORT P3
#define CAPTURE_PIN BIT0 // P7.3

#define SPEED_OF_SOUND 343000           // speed of sound in mm/s
#define TIMER_TICKS_SEC 3000000         // ticks of clock / sec

/*Inits timer, listen pin, and capture pin*/
extern void initReceiver(void);
/*Starts timer in Up Mode*/
extern void startTimer(void);
/*Puts timer in Stop Mode*/
extern void stopTimer(void);
/*Gets the timer value of the echo*/
extern int getEchoTime(void);
/*Calculates distance traveled by sonar beam*/
extern int getDistance(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* RECIEVER_H_ */
