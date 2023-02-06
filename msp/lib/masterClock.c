#include "masterClock.h"

volatile uint64_t clockTick = 0;

void init() {
    // do init things
}

static uint64_t now() { return clockTick; }

static void new() {
    struct MasterClock retval;
    retval.now = now;
    init();
    return retval;
}

const struct MasterClockClass MasterClock = {.new = new};
