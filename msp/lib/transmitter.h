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
    DIO_PORT_Even_Interruptable_Type *port;
    int pin;                  // pin number - just a normal integer, what you see on the board silkscreen
    const int *pulseTrainStartTime; // const pointer to the variable that holds the start time of the sonar chirp
    int pulsePeriod;
    uint32_t lastActiveTime;
    // returns 1 if the output was toggled, 0 if it wasn't toggled
    int (*doTransmit)(struct Transmitter *this, int tickDelay, uint32_t time);
};

extern const struct TransmitterClass {
    // phase shift address is the number of MASTER CLOCK TICKS to shift the phase needs external function fo convert
    struct Transmitter (*new)(DIO_PORT_Even_Interruptable_Type *port, int pin, int pulsePeriod, const int *pulseTrainStartTime);
} Transmitter;

#ifdef __cplusplus
}
#endif

#endif
