/*! \file */
#ifndef MASTERCLOCK_H_
#define MASTERCLOCK_H_

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

struct MasterClock {
    //initializes itself - don't need to expose init
    uint64_t (*now)(void);

};

extern const struct MasterClockClass {
    struct MasterClock (*new)(void);
} MasterClock;

#ifdef __cplusplus
}
#endif

#endif
