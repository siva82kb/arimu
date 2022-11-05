#include "variables.h"

void setup() {
    Serial.begin(115200);
    initIMU();
    
    // Update RTC error flag
    updateRtcErrFlag();
    
    // Iinit SD card logging stuff.
    initSD();
    dataFileParams.subjName = "noone";
    
    // Init timing variables.
    initTimers();
    
    // Update state.
    changeToState(NONE);
}

void loop() {
    prgTimes.curr = micros();
    
    // Read handle incoming messages
    read_handle_incoming_messages();

    // Update state machine
    updateProgramStateMachine();
    
    // Update display
    updateProgramStateMachineDisplay();
}
