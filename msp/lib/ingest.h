/*! \file */
/*!
 * ingest.h
 */

#ifndef INGEST_H_
#define INGEST_H_

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

struct Ingest {
    void (*init)(void);
    int (*analogRead)(int pin);
};

extern const struct IngestClass {
    struct Ingest (*new)(void);
} Ingest;

#ifdef __cplusplus
}
#endif

#endif /* INGEST_H_ */
