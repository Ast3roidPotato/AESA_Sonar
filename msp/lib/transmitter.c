#include "transmitter.h"

static void init(DIO_PORT_Odd_Interruptable_Type *port, int pin) {
    // Configure target pin as GPIO output
    port->SEL0 &= ~(1 << pin);
    port->SEL1 &= ~(1 << pin);
    port->DIR |= 1 << pin;
    // Set target pin to low
    port->OUT &= ~(1 << pin);
}

/*
//
*/
static int doTransmit(struct Transmitter *this, int tickDelay, uint32_t time) {
    // if the time is equal to the start time of the pulse train, then this is the first pulse in the chirp, and should be offset by the tick delay
    // The rest of the pulses need to be offset by the pulse period to maintain the correct frequency
    if (*(this->pulseTrainStartTime) == time) {
        this->lastActiveTime = tickDelay + time;
    }

    // if the time is greater than the last time we were active plus the pulse period
    if (time > this->lastActiveTime + this->pulsePeriod) {
        // toggle the pin
        this->port->OUT ^= 1 << this->pin;
        // set the last active time to the current time
        this->lastActiveTime = time;
        // return 1 to indicate that the pin was toggled
        return 1;
    }
    // return 0 to indicate that the pin was not toggled
    return 0;
}

static struct Transmitter new(DIO_PORT_Odd_Interruptable_Type *port, int pin, int pulsePeriod, const int *pulseTrainStartTime) {
    struct Transmitter t;
    t.port = port;
    t.pin = pin;
    t.pulseTrainStartTime = pulseTrainStartTime;
    t.pulsePeriod = pulsePeriod;
    t.lastActiveTime = 0;
    t.doTransmit = doTransmit;
    init(port, pin);
    return t;
}

const struct TransmitterClass Transmitter = {new};
