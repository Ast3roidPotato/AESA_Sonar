/*! \file */
#ifndef ADC_H_
#define ADC_H_

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C" {
#endif

#include "msp.h"
#include <stdint.h>

struct ADC {
    // initializes itself - don't need to expose init
    DIO_PORT_Odd_Interruptable_Type *port;
    uint16_t pinNum;
    void (*readPot)(void);
    int (*getPhase)(void);
};

extern const struct ADCClass {
    struct ADC (*new)(DIO_PORT_Odd_Interruptable_Type *port, uint16_t pinNum);
} ADC;

#ifdef __cplusplus
}
#endif

#endif
