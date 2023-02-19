/*! \file */
#include "msp.h"

/* Standard Includes */
#include "csHFXT.h"
//#include "ADC.h"
//#include "ingest.h"
//#include "lib/serial.h"
//#include "lib/servoDriver.h"
//#include "lib/transmitter.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "receiver.h"

void main(void) {
    /* Stop Watchdog timer */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    initReceiver();
 while(true) {
    if (((LISTEN_PIN_PORT -> IN) & LISTEN_PIN) == LISTEN_PIN) {
        startTimer();
        while (((LISTEN_PIN_PORT -> IN) & LISTEN_PIN) == LISTEN_PIN) {

        }
        stopTimer();
        int time = getEchoTime();
        int distance = getDistance();
        distance = distance;
    }
 }
}
