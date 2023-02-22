/*
Final ECE230 Project: MSP Main.c
Authors: Jacob Consolvi, Evelyn Elmer, and Abigail Kurfman
Description: This code sets up and runs a UART communication rptotocal with the ESP. The msp also recieves 
an echo signal from a SR04 module, times how long it is high, and calculates distance based on that.
Last Revised: 2/21/23 
*/

/*! \file */
#include "msp.h"

/* Standard Includes */
#include "csHFXT.h"
#include "lib/serial.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "receiver.h"

void main(void) {
    /* Stop Watchdog timer */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    initReceiver();                     // initialize pin to recieve echo

    struct Serial bop = Serial.new();   // create an object of serial class

    while (true) {
        // if capture pin is high, i.e. recieving a positive pulse from SR04...
        if (((CAPTURE_PIN_PORT->IN) & CAPTURE_PIN) == CAPTURE_PIN) {
            // start timer
            startTimer();
            // wait until end of pulse
            while (((CAPTURE_PIN_PORT->IN) & CAPTURE_PIN) == CAPTURE_PIN) {
            }
            // stop timer
            stopTimer();
            // get time of pulse width
             int time = getEchoTime();
             // get distance traveled by sonar beam
            int distance = getDistance();
            // send distance to ESP for display
            bop.print("%d", distance);
        }
    }
}
