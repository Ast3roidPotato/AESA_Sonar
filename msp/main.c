/*! \file */
#include "msp.h"

/* Standard Includes */
#include "csHFXT.h"
// #include "ADC.h"
// #include "ingest.h"
// #include "rgbLED.h"
#include "lib/serial.h"
// #include "lib/servoDriver.h"
// #include "lib/transmitter.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "receiver.h"

void main(void) {
    /* Stop Watchdog timer */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    initReceiver();
    // RGBLED_init();
    struct Serial bop = Serial.new();
    while (true) {
        if (((CAPTURE_PIN_PORT->IN) & CAPTURE_PIN) == CAPTURE_PIN) {
            startTimer();
            while (((CAPTURE_PIN_PORT->IN) & CAPTURE_PIN) == CAPTURE_PIN) {
            }
            stopTimer();
             int time = getEchoTime();
            int distance = getDistance();
            bop.print("%d", distance);
        }
    }
}
