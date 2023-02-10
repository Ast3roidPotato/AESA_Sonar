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

#define TX_ARRAY_PORT P5
#define TX_ARRAY_PORT_2 P6
#define PULSE_HALF_PERIOD 1                                         // 17             // Optimally 50, but is lower due to timing errors
#define PULSE_DELAY_COMPENSATION 0                                  // Compensate for the time it takes to execute the loop
#define PULSE_DELAY_COMPENSATION_START 8 * PULSE_DELAY_COMPENSATION // Compensate for the time it takes to execute the loop

int mapAnalogReadToTickDelay(int analogRead) { return (analogRead - 2045) >> 2; }

void main(void) {
    int i;
    /* Stop Watchdog timer */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    struct MasterClock clock = MasterClock.new();
    // struct ADC potInput = ADC.new(P6,0);
    struct Ingest analog = Ingest.new();
    // struct Serial serial = Serial.new();

    // serial.println("Starting up...");

    int pulseDelay = 400000; // assume ~0.25us per tick
    int lastPulseTime = 0;
    // int pulseCount = 0;
    int pulseCountTarget = 10;

    int phaseTickOffset = 0;
    int pulseTrainStartTime = 0;
    const int *pulseTrainPtr = &pulseTrainStartTime;

    // uint32_t oldTime = 0;
    uint32_t currentTime;

    // struct Transmitter testSingleTransmitter = Transmitter.new(TX_ARRAY_PORT, 0, PULSE_HALF_PERIOD, pulseTrainPtr);

    struct Transmitter transmitterArray[8];
    for (i = 0; i < 8; ++i) {
        if (i == 3)
            continue;
        int j = i > 3 ? i - 1 : i;
        // serial.println("Initializing transmitter %d", i);
        transmitterArray[j] = Transmitter.new(P5, i, PULSE_HALF_PERIOD, pulseTrainPtr);
    }

    transmitterArray[7] = Transmitter.new(P3, 7, PULSE_HALF_PERIOD, pulseTrainPtr);

    int toggleCountArray[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    // serial.println("Starting loop...");
    while (1) {
        // serial.println("Pot Phase: %d ", mapAnalogReadToTickDelay(analog.analogRead(0)));
        phaseTickOffset = mapAnalogReadToTickDelay(analog.analogRead(0));
        // phaseTickOffset = 0;
        currentTime = clock.now();
        if (currentTime - lastPulseTime > pulseDelay) {
            pulseTrainStartTime = currentTime;
            int minToggles = 9999;
            int cumulativePhaseOffset;
            while (minToggles == 9999 || minToggles <= pulseCountTarget - 1) {
                cumulativePhaseOffset = 0;
                minToggles = 9999;
                if (phaseTickOffset >= 0) {
                    // int delayCompensation = PULSE_DELAY_COMPENSATION_START;
                    for (i = 0; i < 8; ++i) {
                        if (toggleCountArray[i] < pulseCountTarget) {
                            if (transmitterArray[i].doTransmit(&transmitterArray[i], cumulativePhaseOffset, currentTime)) {
                                ++toggleCountArray[i];
                                if (toggleCountArray[i] < minToggles) {
                                    minToggles = toggleCountArray[i];
                                }
                                // serial.println("TX %d toggled.", i);
                            }
                        }
                        // delayCompensation -= PULSE_DELAY_COMPENSATION;
                        cumulativePhaseOffset += phaseTickOffset;
                    }
                } else {
                    phaseTickOffset *= -1;
                    // reverse above loop - only do AFTER testing to confirm that it works for positive phase shifts
                    // int delayCompensation = PULSE_DELAY_COMPENSATION_START;
                    for (i = 7; i >= 0; --i) {
                        if (toggleCountArray[i] < pulseCountTarget) {
                            if (transmitterArray[i].doTransmit(&transmitterArray[i], cumulativePhaseOffset, currentTime)) {
                                ++toggleCountArray[i];
                                if (toggleCountArray[i] < minToggles) {
                                    minToggles = toggleCountArray[i];
                                }
                                // serial.println("TX %d toggled.", i);
                            }
                        }
                        // delayCompensation -= PULSE_DELAY_COMPENSATION;
                        cumulativePhaseOffset += phaseTickOffset;
                    }
                }

                // pulseCount += testSingleTransmitter.doTransmit(&testSingleTransmitter, 0, currentTime);
                // serial.println("Pulse Count: %d", pulseCount);
                currentTime = clock.now();
            }

            // TX_ARRAY_PORT->OUT ^= 0xFF;
            // pulseCount = 0;
            lastPulseTime = currentTime;
            memset(toggleCountArray, 0, sizeof(toggleCountArray));
        }
    }
}
