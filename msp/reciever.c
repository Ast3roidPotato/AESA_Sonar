#include "reciever.h"
#include "msp.h"

void initListenPin() {
    LISTEN_PIN_PORT -> DIR &= ~(LISTEN_PIN);
    LISTEN_PIN_PORT -> SEL0 &= ~(LISTEN_PIN);
    LISTEN_PIN_PORT -> SEL1 &= ~(LISTEN_PIN);
}

void initCaptureTimer() {
    
}