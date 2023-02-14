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

#define TX_ARRAY_PORT P4
#define PULSE_HALF_PERIOD 42                                        // 17             // Optimally 50, but is lower due to timing errors
#define PULSE_DELAY_COMPENSATION 0                                  // Compensate for the time it takes to execute the loop
#define PULSE_DELAY_COMPENSATION_START 8 * PULSE_DELAY_COMPENSATION // Compensate for the time it takes to execute the loop

int mapAnalogReadToTickDelay(int analogRead) { return (analogRead - 2045) >> 3; }

void main(void) {
    int i;
    /* Stop Watchdog timer */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    struct MasterClock clock = MasterClock.new();
    // struct ADC potInput = ADC.new(P6,0);
    struct Ingest analog = Ingest.new();
    // struct Serial serial = Serial.new();

    // serial.println("Starting up...");

    TX_ARRAY_PORT->SEL0 &= ~0xFF;
    TX_ARRAY_PORT->SEL1 &= ~0xFF;
    TX_ARRAY_PORT->DIR |= 0xFF;
    TX_ARRAY_PORT->OUT &= ~0xFF;

    // int pulseDelay = 400000; // assume ~0.25us per tick
    int pulseDelay = 400000; // assume ~0.25us per tick
    int lastPulseTime = 0;
    // int pulseCount = 0;
    int pulseCountTarget = 10;

    int phaseTickOffset = 0;
    int pulseTrainStartTime = 0;
    const int *pulseTrainPtr = &pulseTrainStartTime;

    P1->SEL0 &= ~BIT0;
    P1->SEL1 &= ~BIT0;
    P1->DIR |= BIT0;
    // P1->OUT |= BIT0;

    // uint32_t oldTime = 0;
    uint32_t currentTime;

    // struct Transmitter testSingleTransmitter = Transmitter.new(TX_ARRAY_PORT, 0, PULSE_HALF_PERIOD, pulseTrainPtr);

    int toggleCountArray[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    uint32_t lastActiveTime0 = 0;
    uint32_t lastActiveTime1 = 0;
    uint32_t lastActiveTime2 = 0;
    uint32_t lastActiveTime3 = 0;
    uint32_t lastActiveTime4 = 0;
    uint32_t lastActiveTime5 = 0;
    uint32_t lastActiveTime6 = 0;
    uint32_t lastActiveTime7 = 0;

    // serial.println("Starting loop...");
    while (1) {
        // serial.println("Pot Phase: %d ", mapAnalogReadToTickDelay(analog.analogRead(0)));
        // phaseTickOffset = mapAnalogReadToTickDelay(analog.analogRead(0));
        phaseTickOffset = 1;
        currentTime = clock.now();
        if (currentTime - lastPulseTime > pulseDelay) {
            P1->OUT |= BIT0;
            // serial.println("Pulse!");
            pulseTrainStartTime = currentTime;
            int minToggles = 9999;
            // int cumulativePhaseOffset;

            TX_ARRAY_PORT->OUT = 0;

            if (phaseTickOffset < 0) {

                phaseTickOffset = -phaseTickOffset;

                lastActiveTime7 = currentTime;
                lastActiveTime6 = currentTime + phaseTickOffset * 1;
                lastActiveTime5 = currentTime + phaseTickOffset * 2;
                lastActiveTime4 = currentTime + phaseTickOffset * 3;
                lastActiveTime3 = currentTime + phaseTickOffset * 4;
                lastActiveTime2 = currentTime + phaseTickOffset * 5;
                lastActiveTime1 = currentTime + phaseTickOffset * 6;
                lastActiveTime0 = currentTime + phaseTickOffset * 7;
            } else {
                lastActiveTime0 = currentTime;
                lastActiveTime1 = currentTime + phaseTickOffset;
                lastActiveTime2 = currentTime + phaseTickOffset * 2;
                lastActiveTime3 = currentTime + phaseTickOffset * 3;
                lastActiveTime4 = currentTime + phaseTickOffset * 4;
                lastActiveTime5 = currentTime + phaseTickOffset * 5;
                lastActiveTime6 = currentTime + phaseTickOffset * 6;
                lastActiveTime7 = currentTime + phaseTickOffset * 7;
            }

            // serial.println("Set lastActiveTime to %d", currentTime);

            while (minToggles == 9999 || minToggles <= pulseCountTarget) {
                // serial.println("Inside while loop. minToggles: %d", minToggles);
                // cumulativePhaseOffset = 0;
                minToggles = 9999;
                // int delayCompensation = PULSE_DELAY_COMPENSATION_START;

                int j;
                // for (j = 0; j < 10000; j++) ;

                int portOutToggle = 0;
                if (toggleCountArray[0] <= pulseCountTarget && currentTime > lastActiveTime0 + PULSE_HALF_PERIOD) {
                    // serial.println("Toggle 0");
                    portOutToggle |= 0b01;
                    lastActiveTime0 = currentTime;
                    ++toggleCountArray[0];
                    if (toggleCountArray[0] < minToggles) {
                        minToggles = toggleCountArray[0];
                    }
                }
                if (toggleCountArray[1] < pulseCountTarget && currentTime > lastActiveTime1 + PULSE_HALF_PERIOD) {
                    // serial.println("Toggle 1");
                    portOutToggle |= 0b10;
                    lastActiveTime1 = currentTime;
                    ++toggleCountArray[1];
                    if (toggleCountArray[1] < minToggles) {
                        minToggles = toggleCountArray[1];
                    }
                }
                if (toggleCountArray[2] < pulseCountTarget && currentTime > lastActiveTime2 + PULSE_HALF_PERIOD) {
                    // serial.println("Toggle 2");
                    portOutToggle |= 0b100;
                    lastActiveTime2 = currentTime;
                    ++toggleCountArray[2];
                    if (toggleCountArray[2] < minToggles) {
                        minToggles = toggleCountArray[2];
                    }
                }
                if (toggleCountArray[3] < pulseCountTarget && currentTime > lastActiveTime3 + PULSE_HALF_PERIOD) {
                    // serial.println("Toggle 3");
                    portOutToggle |= 0b1000;
                    lastActiveTime3 = currentTime;
                    ++toggleCountArray[3];
                    if (toggleCountArray[3] < minToggles) {
                        minToggles = toggleCountArray[3];
                    }
                }
                if (toggleCountArray[4] < pulseCountTarget && currentTime > lastActiveTime4 + PULSE_HALF_PERIOD) {
                    // serial.println("Toggle 4");
                    portOutToggle |= 0b10000;
                    lastActiveTime4 = currentTime;
                    ++toggleCountArray[4];
                    if (toggleCountArray[4] < minToggles) {
                        minToggles = toggleCountArray[4];
                    }
                }
                if (toggleCountArray[5] < pulseCountTarget && currentTime > lastActiveTime5 + PULSE_HALF_PERIOD) {
                    // serial.println("Toggle 5");
                    portOutToggle |= 0b100000;
                    lastActiveTime5 = currentTime;
                    ++toggleCountArray[5];
                    if (toggleCountArray[5] < minToggles) {
                        minToggles = toggleCountArray[5];
                    }
                }
                if (toggleCountArray[6] < pulseCountTarget && currentTime > lastActiveTime6 + PULSE_HALF_PERIOD) {
                    // serial.println("Toggle 6");
                    portOutToggle |= 0b1000000;
                    lastActiveTime6 = currentTime;
                    ++toggleCountArray[6];
                    if (toggleCountArray[6] < minToggles) {
                        minToggles = toggleCountArray[6];
                    }
                }
                if (toggleCountArray[7] < pulseCountTarget && currentTime > lastActiveTime7 + PULSE_HALF_PERIOD) {
                    // serial.println("Toggle 7");
                    portOutToggle |= 0b10000000;
                    lastActiveTime7 = currentTime;
                    ++toggleCountArray[7];
                    if (toggleCountArray[7] < minToggles) {
                        minToggles = toggleCountArray[7];
                    }
                }

                TX_ARRAY_PORT->OUT ^= portOutToggle;
                // serial.println("Port Out Toggle: %d", portOutToggle);
                // serial.println("Port Out: %d", P4->OUT);
                // pulseCount += testSingleTransmitter.doTransmit(&testSingleTransmitter, 0, currentTime);
                // serial.println("Pulse Count: %d", pulseCount);
                currentTime = clock.now();
            }

            // TX_ARRAY_PORT->OUT ^= 0xFF;
            // pulseCount = 0;
            lastPulseTime = currentTime;
            memset(toggleCountArray, 0, sizeof(toggleCountArray));
            P1->OUT &= ~BIT0;
        }
    }
}
