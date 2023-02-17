/*! \file */
/*!
 * transmitter.h
 *
 * Description:
 *
 *  Created on: 02/12/23
 *      Author:
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
#define LISTEN_PIN BIT5 // P1.5
#define CAPTURE_PIN_PORT P3
#define CAPTURE_PIN BIT0 // P3.0

/*Inits timer, listen pin, and capture pin*/
extern void initReceiver(void);
/*Starts timer in Up Mode*/
extern void startTimer(void);
/*Puts timer in Stop Mode*/
extern void stopTimer(void);
/*Gets the timer value of the echo*/
extern int getEchoTime(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* RECIEVER_H_ */
