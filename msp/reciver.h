/*! \file */
/*!
 * transmitter.h
 *
 * Description: 
 *
 *  Created on: 02/12/23
 *      Author:
 */

#ifndef RECIEVER_H_
#define RECIEVER_H_

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

#define LISTEN_PIN_PORT P1
#define LISTEN_PIN 0b00000010 // P1.1

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* RECIEVER_H_ */
