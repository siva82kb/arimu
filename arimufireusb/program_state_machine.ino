/*
 * Sketch implementing the functions for handling the 
 * main program's state machine.
 */

void updateProgramStateMachine()
{
    switch(prgState) {
        case NONE:
            handleNone();
            break;
        case BADERR:
            handleBadError();
            break;
        case NORMAL:
        case EXPERIMENT:
            handleNormalExperiment();
            break;
        case STREAMING:
            handleStreaming();
            break;
        case DOCKSTNCOMM:
            handleDockStnComm();
            break;
    }
}

void updateProgramStateMachineDisplay() {
    switch(prgState) {
        case NONE:
            digitalWrite(BLUE_LED, LOW);
            digitalWrite(RED_LED, LOW);
            break;
        case BADERR:
            handleBarErrorStateDispay();
            break;
        case NORMAL:
            handleNormalStateDispay();
            break;
        case EXPERIMENT:
            handleExperimentStateDisplay();
            break;
        case STREAMING:
            handleStreamingStateDisplay();
            break;
        case DOCKSTNCOMM:
            // handleDockStnCommDisplay();
            digitalWrite(BLUE_LED, HIGH);
            digitalWrite(RED_LED, HIGH);
            break;
    }
}

// Handle BADERR State Display
void handleBarErrorStateDispay()
{
    long _delT = prgTimes.curr - prgTimes.errDisp;
    digitalWrite(BLUE_LED, ledState);
    digitalWrite(RED_LED, ledState);
    if ((ledState == HIGH) && (_delT >= ERRORON)) {
        ledState = LOW;
        prgTimes.errDisp = micros();
    } else if ((ledState == LOW) && (_delT >= ERROROFF)) {
        ledState = HIGH;
        prgTimes.errDisp = micros();
    }
}

// Handle NORMAL State Display
void handleNormalStateDispay()
{
    long _delT = prgTimes.curr - prgTimes.normDisp;
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(RED_LED, ledState);
    if ((ledState == HIGH) && (_delT >= NORMALON)) {
        ledState = LOW;
        prgTimes.normDisp = micros();
    } else if ((ledState == LOW) && (_delT >= NORMALOFF)) {
        ledState = HIGH;
        prgTimes.normDisp = micros();
    }
}

// Handle EXPERIMENT State Display
void handleExperimentStateDisplay() {
    long _delT = prgTimes.curr - prgTimes.exptDisp;
    digitalWrite(BLUE_LED, ledState);
    digitalWrite(RED_LED, LOW);
    if ((ledState == HIGH) && (_delT >= EXPTON)) {
        ledState = LOW;
        prgTimes.exptDisp = micros();
    } else if ((ledState == LOW) && (_delT >= EXPTOFF)) {
        ledState = HIGH;
        prgTimes.exptDisp = micros();
    }
}

// Handle STREAMING State Display
void handleStreamingStateDisplay() {
    long _delT = prgTimes.curr - prgTimes.strmDisp;
    digitalWrite(BLUE_LED, ledState);
    digitalWrite(RED_LED, !ledState);
    if ((ledState == HIGH) && (_delT >= STRMON)) {
        ledState = LOW;
        prgTimes.strmDisp = micros();
    } else if ((ledState == LOW) && (_delT >= STRMOFF)) {
        ledState = HIGH;
        prgTimes.strmDisp = micros();
    }
}

// Handles DOCKSTNCOMM State Display
void handleDockStnCommDisplay() {
}

// Handle NONE and BADERR states
void handleNone() {
    // Update state
    if (isABadError()) {
        changeToState(BADERR);
    } else {
        // Go to normal state.
        changeToState(NORMAL);
    }
}

// Handle NONE and BADERR states
void handleBadError() {
    // Update state
    if (isABadError()) {
        changeToState(BADERR);
    } else {
        changeToState(NONE);
    }
}

// Handle NORMAL and EXPERIMENT states
void handleNormalExperiment()
{
    // Read IMU data.
    long _delT = prgTimes.curr - prgTimes.normExpt;
    if (_delT >= NORMALDELAY) {
        readIMU();
        // Write to data buffer.
        _microsData.microsecs = prgTimes.normExpt;
        if (_dataBuffer.addLine(&_imuData, &_shortRtcData)) {
            writePageToFile();
        }
        prgTimes.normExpt = micros();
    }
}

// Handles STREAMING state
void handleStreaming () {
    // Read IMU data.
    long _delT = prgTimes.curr - prgTimes.normExpt;    
    if (_delT >= NORMALDELAY) {
        readIMU();
        // Send data via the serial port.
        _microsData.microsecs = prgTimes.normExpt;
        _streamImuDataOut(&_imuData, &_shortRtcData);
        prgTimes.normExpt = micros();
    }
}

// Handles DOCSTNCOMM state
void handleDockStnComm() {
    // Simply check if the watch dog timer is reset.
    long _delT = prgTimes.curr - prgTimes.dkStnWd;
    if (_delT >= DOCKSTNCOMMWD) {
        // Docking station not connected. Revert to None State.
        changeToState(NONE);
        _initNewPayload(STOPDOCKSTNCOMM);
        jedi.addByte(err.code);
        jedi.writePayload();
    }
}

/*
 * Function to handle changing of states.
 * This function will ensure everything that needs to be 
 * taken care of when chaning state.
 */
void changeToState(progState newState) {
    prgState = newState;
    if (dataFile.isOpen()) { closeDataFile(); }
    switch(prgState) {
        case NONE:
            dataFileParams.currFileName = "";
            break;
        case BADERR:
            dataFileParams.currFileName = "";
            break;
        case NORMAL:
            // Close and open a new file.
            openNewDataFile();
            break;
        case EXPERIMENT:
            // Close and open a new file.
            openNewDataFile();
            break;
        case DOCKSTNCOMM:
            dataFileParams.currFileName = "";
            break;
        case STREAMING:
            dataFileParams.currFileName = "";
            break;
    }
}
