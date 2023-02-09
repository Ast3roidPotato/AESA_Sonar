/*! \file */
/******************************************************************************
 * MSP432 Lab Exercise 5-2
 *
 * Description: Using timers to generate PWM signals to drive a servo motor
 *
 * Author: Jacob Consalvi
 * Last-modified:
 *
 * An external HF crystal between HFXIN & HFXOUT is required for MCLK,SMCLK
 *
 *                 MSP432P411x
 *             -------------------
 *         /|\|                   |
 *          | |              P1.1 |<---- S1
 *          --|RST                |
 *            |      (TA2.1) P5.6 |----> Servo
 *            |                   |
 *            |              PJ.2 |------
 *            |                   |     |
 *            |                   |    HFXT @ 48MHz
 *            |                   |     |
 *            |              PJ.3 |------
 *
 *******************************************************************************/
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

#define TX_ARRAY_PORT P5
#define PULSE_HALF_PERIOD 46 // Optimally 50, but is lower due to timing errors

int mapAnalogReadToTickDelay(int analogRead) { return (analogRead - 2045) / 4; }

void main(void) {
    int i;
    /* Stop Watchdog timer */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    struct MasterClock clock = MasterClock.new();
    // struct ADC potInput = ADC.new(P6,0);
    struct Ingest analog = Ingest.new();
    // struct Serial serial = Serial.new();

    // serial.println("Starting up...");

    // Configure pin 5.6 to be digital output GPIO
    P5->SEL0 = 0;
    P5->SEL1 = 0;
    P5->DIR = 0xFF;
    P5->OUT = 0;

    int pulseDelay = 400000; // assume ~0.25us per tick
    int lastPulseTime = 0;
    int pulseCount = 0;
    int pulseCountTarget = 20;

    int phaseTickOffset = 0;
    int pulseTrainStartTime = 0;
    const int *pulseTrainPtr = &pulseTrainStartTime;

    // uint32_t oldTime = 0;
    uint32_t currentTime;

    // serial.println("Starting loop...");
    // struct Transmitter testSingleTransmitter = Transmitter.new(TX_ARRAY_PORT, 0, PULSE_HALF_PERIOD, pulseTrainPtr);

    struct Transmitter transmitterArray[8];
    for (i = 0; i < 8; ++i) {
        transmitterArray[i] = Transmitter.new(TX_ARRAY_PORT, i, PULSE_HALF_PERIOD, pulseTrainPtr);
    }

    int toggleCountArray[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    while (1) {
        // serial.println("Pot Phase: %d", mapAnalogReadToTickDelay(analog.analogRead(0)));
        phaseTickOffset = mapAnalogReadToTickDelay(analog.analogRead(0));
        currentTime = clock.now();
        if (currentTime - lastPulseTime > pulseDelay) {
            pulseTrainStartTime = currentTime;
            while (pulseCount < pulseCountTarget) {
                currentTime = clock.now();
                int cumulativePhaseOffset = 0;
                int totalToggles = 0;
                if (phaseTickOffset > 0) {
                    for (i = 0; i < 8; ++i) {
                        if (toggleCountArray[i] < 2 * pulseCountTarget) {
                            if (transmitterArray[i].doTransmit(&transmitterArray[i], cumulativePhaseOffset, currentTime)) {
                                ++toggleCountArray[i];
                                ++totalToggles;
                            }
                        }
                        cumulativePhaseOffset += phaseTickOffset;
                    }
                    if (totalToggles >= 16) { // makes sure that every transmitter has completed their pulse.
                        ++pulseCount;
                    }
                } else {
                    // reverse above loop - only do AFTER testing to confirm that it works for positive phase shifts
                }

                // if (currentTime - oldTime > 46) {
                //     P5->OUT ^= BIT0;
                //     pulseCount++;
                //     oldTime = currentTime;
                // }
                // testSingleTransmitter.doTransmit(&testSingleTransmitter, 0, currentTime);
            }
            pulseCount = 0;
            lastPulseTime = currentTime;
            for (i = 0; i < 8; ++i) {
                toggleCountArray[i] = 0;
            }
        }
    }
}
