/*! \file */
#include "msp.h"

/* Standard Includes */
#include "csHFXT.h"
#include "lib/ADC.h"
#include "lib/ingest.h"
#include "lib/masterClock.h"
#include "lib/serial.h"
#include "lib/servoDriver.h"
#include "lib/transmitter.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "receiver.h"

void main(void) {
    /* Stop Watchdog timer */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    initReceiver();
    if ((LISTEN_PIN_PORT -> IN & LISTEN_PIN) == LISTEN_PIN) {
        startTimer();
    }
}