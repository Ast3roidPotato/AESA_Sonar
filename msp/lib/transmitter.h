/*! \file */
#ifndef TX_H_
#define TX_H_

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

struct Transmitter {
    // initializes itself - don't need to expose init
    int pin;
    uint16_t *phaseShiftAddress;
    void (*doTransmit)(struct Transmmitter *this);
};

extern const struct TransmitterClass {
    struct Transmitter (*new)(void);
} Transmitter;

#ifdef __cplusplus
}
#endif

#endif
