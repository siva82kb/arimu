void read_handle_incoming_messages() {
    byte cmd;
    int plSz = jedi.readPayload();
    // Read handle incoming data
    if (plSz > 0) {
        switch (jedi.inPayload[0]) 
        {
            case STATUS:            // All states
                handle_status();
                break;
            case PING:              // All states
                // Send the device name back.
                handle_ping();
                break;
            case GETMICROS:         // All states
                // Send the current device micros count
                handle_getmicros();
                break;
            case SETTIME:           // DOCKSTNCOMM
                // Set current time and send the set time.
                handle_settime();
                break;
            case GETTIME:           // All states
                // Send currenttime.
                handle_gettime();
                break;
            case SETSUBJECT:        // DOCKSTNCOMM
                // Set subject name
                handle_setsubject();
                break;
            case GETSUBJECT:        // All states
                // Get subject name
                handle_getsubject();
                break;
            case LISTFILES:         // DOCKSTNCOMM
                // List files
                handle_listfiles();
                break;
            case GETFILEDATA:       // DOCKSTNCOMM
                // Send contents of a file.
                handle_getfiledata();
                break;
            case DELETEFILE:        // DOCKSTNCOMM
                // Delete file.
                handle_deletefile();
                break;
            case CURRENTFILENAME:   // NORMAL, EXPERIMENT, DOCKSTNCOMM
                // Get current file name
                handle_getcurrentfilename();
                break;
            case STARTNORMAL:       // NONE
                // Start normal
                handle_startnormal();
                break;
            case STOPNORMAL:        // NORMAL
                // Stop normal
                handle_stopnormal();
                break;
            case STARTEXPT:         // NONE
                // Start experiment
                handle_startexpt();
                break;
            case STOPEXPT:          // EXPERIMENT
                // Stop experiment
                handle_stopexpt();
                break;
            case STARTSTREAM:       // NONE, BADERR
                // Start experiment
                handle_startstream();
                break;
            case STOPSTREAM:        // STREAMING
                // Stop experiment
                handle_stopstream();
                break;
            case STARTDOCKSTNCOMM:  // NORMAL, EXPERIMENT
                // Start docking station mode
                handle_startdockstncomm();
                break;
            case STOPDOCKSTNCOMM:   // DOCKSTNCOMM
                // Stop docking station mode
                handle_stopdockstncomm();
                break;
            case SETTONONE:
                // Sets the device to NONE no matter what state we are in.
                handle_settonone();
                break;
            case DOCKSTNPING:
                // Regular pings to inform ARIMU about the existing connection
                // with the docking station.
                // Reset the Docking station watch dog timer.
                prgTimes.dkStnWd = prgTimes.curr;
                break;
        }
        jedi.payloadHandled();
    }
}


/* Handles STATUS.
 *  
 */
void handle_status() {
    _initNewPayload(STATUS);
    jedi.writePayload();
}

/* Handles PING.
 *  
 */
void handle_ping() {
    char* c = ARIMUNAME;
    _initNewPayload(PING);
    while (*c != NULL) 
    {
        jedi.addByte(*c);
        c++;
    }
    jedi.writePayload();
}

/* Handle GETMICROS.
 *  
 */
void handle_getmicros() {
    microsunion_t _microsTemp;
    _microsTemp.microsecs = micros();
    _initNewPayload(GETMICROS);
    for (int i = 0; i < 4; i++ ) {
        jedi.addByte(_microsTemp.bytes[i]);
    }
    jedi.writePayload();
}

/* Handles SETTIME.
 *  Can set time only under the folliowing states: 
 *  NONE, BADERR, NORMAL, STREAMING, DOCKSTNCOMM 
 *  Cannot set time under the following state:
 *  EXPERIMENT
 */
void handle_settime() {
    if (prgState != DOCKSTNCOMM) {
        return;
    }
    // Set time.
    intunion_t t[7];
    for (int i = 0; i < 7; i++) {
        t[i].bytes[0] = jedi.inPayload[1 + 4 * i];
        t[i].bytes[1] = jedi.inPayload[2 + 4 * i];
        t[i].bytes[2] = jedi.inPayload[3 + 4 * i];
        t[i].bytes[3] = jedi.inPayload[4 + 4 * i];
    }
    rtc.setTime(t[6].num, t[5].num, t[4].num,
                t[3].num, t[2].num, t[1].num,
                t[0].num); 
    // Update RTC Err Flag.
    updateRtcErrFlag();
    
    // Send the current time
    rtc.getTime();
    _initNewPayload(SETTIME);
    jedi.addInt(rtc.year);
    jedi.addInt(rtc.month);
    jedi.addInt(rtc.dayOfMonth);
    jedi.addInt(rtc.hour);
    jedi.addInt(rtc.minute);
    jedi.addInt(rtc.seconds);
    jedi.addInt(rtc.hundredths);
    // Micros data.
    microsunion_t _microsTemp;
    _microsTemp.microsecs = micros();
    for (int i = 0; i < 4; i++ ) {
        jedi.addByte(_microsTemp.bytes[i]);
    }
    jedi.writePayload();
    // Reset the Docking station watch dog timer.
    prgTimes.dkStnWd = prgTimes.curr;
}

/* Handles GETTIME.
 *  
 */
void handle_gettime() {
    rtc.getTime();
    _initNewPayload(GETTIME);
    jedi.addInt(rtc.year);
    jedi.addInt(rtc.month);
    jedi.addInt(rtc.dayOfMonth);
    jedi.addInt(rtc.hour);
    jedi.addInt(rtc.minute);
    jedi.addInt(rtc.seconds);
    jedi.addInt(rtc.hundredths);
    // Micros data.
    microsunion_t _microsTemp;
    _microsTemp.microsecs = micros();
    for (int i = 0; i < 4; i++ ) {
        jedi.addByte(_microsTemp.bytes[i]);
    }
    jedi.writePayload();
}

/* Handle SETSUBJECT
 *  Can set subject only under the folliowing states: 
 *  NONE, BADERR, NORMAL, STREAMING, DOCKSTNCOMM 
 *  Cannot set subject under the following state:
 *  EXPERIMENT
 */
void handle_setsubject() {
    if (prgState != DOCKSTNCOMM) {
        return;
    }
    dataFileParams.subjName = (char*)jedi.inPayload + 1;
    _initNewPayload(SETSUBJECT);
    const char *c = dataFileParams.subjName.c_str();
    while (*c != NULL) {
        jedi.addByte(*c);
        c++;
    }
    jedi.writePayload();
    // Reset the Docking station watch dog timer.
    prgTimes.dkStnWd = prgTimes.curr;
}

/* Handle GETSUBJECT
 *  
 */
void handle_getsubject() {
    _initNewPayload(GETSUBJECT);
    const char *c = dataFileParams.subjName.c_str();
    while (*c != NULL) {
        jedi.addByte(*c);
        c++;
    }
    jedi.writePayload();
}

/* Handle LISTFILES
 *  Can do this only under the folliowing states: 
 *  DOCKSTNCOMM
 */
void handle_listfiles() {
    if (prgState != DOCKSTNCOMM) {
        return;
    }
    int nullFile = 0;
    char _fname[50];
    String _fnamestr;
    File32 entry;
    byte _pyldSize = 0;
    
    // Init list of files response.
    _initNewPayload(LISTFILES);
    jedi.addByte((byte)'[');        // Start Of List (SOL)
    if (isSDPresent())
    {
        err.bits.SdCardNotConnected = 0;
        File32 root = sd.open("/");
        while (true) 
        {
            // Read next filename
            entry =  root.openNextFile();
            // NULL file.
            if (!entry) { nullFile++; }
            if (nullFile > NULLTHRESHOLD) { break; }
            
            // Not a NULL file.
            entry.getName(_fname, 50);
            
            if (String(_fname).indexOf(DATA_FILE_SUFFIX) > 0) {
                _fnamestr = String(_fname) + String(",");
                if (!_addStringToPayload(_fnamestr.c_str())) {
                    // Payload too full to add string. Send the current
                    // payload and start a new one.
                    jedi.writePayload();
                    // Start the new packet.
                    _initNewPayload(LISTFILES);
                    _addStringToPayload(_fnamestr.c_str());
                }
            }
            entry.close();
            if (String(_fname) == "") { break; }
        }
        // Send the terminal part of the LISTFILES response.
        _fnamestr = String("]");
        if (!_addStringToPayload(_fnamestr.c_str())) {
            // Payload too full to add string. Send the current
            // payload and start a new one.
            jedi.writePayload();
            _initNewPayload(LISTFILES);
            _addStringToPayload(_fnamestr.c_str());
        }
        jedi.writePayload();
        root.close();
    }
    else {
        err.bits.SdCardNotConnected = 1;
        _initNewPayload(LISTFILES);
        jedi.writePayload();
    }
    // Reset the Docking station watch dog timer.
    // This is to ensure that when the program comes out
    // of this loop, the timer has not expired.
    prgTimes.dkStnWd = micros();
}

/* Handles GETFILEDATA
 *  Can do this only under the folliowing states: 
 *  DOCKSTNCOMM
 */
void handle_getfiledata() {
    if (prgState != DOCKSTNCOMM) {
        return;
    }
    int nullFile = 0;
    char *fname = (char*)jedi.inPayload + 1;
    char sdfname[50];
    bool _found = 0;
    File32 entry;
    if (isSDPresent())
    {
        // Update the other side that file search is in
        // progress.
        _initNewPayload(GETFILEDATA);
        jedi.addByte(FILESEARCHING);
        jedi.writePayload();
        
        // Search
        err.bits.SdCardNotConnected = 0;
        File32 root = sd.open("/");
        while (true) 
        {
            entry =  root.openNextFile();
            // NULL file.
            if (!entry) { nullFile++; }
            if (nullFile > NULLTHRESHOLD) { break; }

            // Not a NULL file.s
            entry.getName(sdfname, 50);
            if (String(sdfname) == String(fname)) {
                _found = true;
                break;
            }
            entry.close();
            if (String(sdfname) == "") { break; }
        }
        if (_found) {
            // Send file data.
            _sendFileData(fname);
        } else {
            // Send 0 to indicate that the file was not found.
            _initNewPayload(GETFILEDATA);
            jedi.addByte(NOFILE);
            jedi.writePayload();
        }
        root.close();
    }
    else {
        err.bits.SdCardNotConnected = 1;
        _initNewPayload(GETFILEDATA);
        jedi.addByte(NOFILE);
        jedi.writePayload();
    }
    // Reset the Docking station watch dog timer.
    // This is to ensure that when the program comes out
    // of this loop, the timer has not expired.
    prgTimes.dkStnWd = micros();
}

/* Handles DELETEFILE
 *  Can do this only under the folliowing states: 
 *  DOCKSTNCOMM
 */
void handle_deletefile() {
    if (prgState != DOCKSTNCOMM) {
        return;
    }
    char *fname = (char*)jedi.inPayload + 1;
    if (isSDPresent()) {
        _initNewPayload(DELETEFILE);
        if (sd.remove(fname)) {
            jedi.addByte(FILEDELETED);
        } else {
            jedi.addByte(FILENOTDELETED);
        }
        jedi.writePayload();
    }
    else {
        err.bits.SdCardNotConnected = 1;
        _initNewPayload(DELETEFILE);
        jedi.writePayload();
    }
    // Reset the Docking station watch dog timer.
    prgTimes.dkStnWd = prgTimes.curr;
}

/* Handle CURRENTFILENAME
 *  
 */
void handle_getcurrentfilename() {
    _initNewPayload(CURRENTFILENAME);
    const char *c = dataFileParams.currFileName.c_str();
    while (*c != NULL) {
        jedi.addByte(*c);
        c++;
    }
    jedi.writePayload();
}

/* Handles STARTNORMAL
 *  
 */
void handle_startnormal() {
    if (prgState != NONE) {
        return;
    }
    changeToState(NORMAL);
    _initNewPayload(STARTNORMAL);
    jedi.writePayload();
}

/* Handles STOPNORMAL
 *  
 */
void handle_stopnormal() {
    if (prgState != NORMAL) {
        return;
    }
    changeToState(NONE);
    _initNewPayload(STOPNORMAL);
    jedi.writePayload();
}

/* Handles STARTEXPT
 *  
 */
void handle_startexpt() {
    if (prgState != NONE) {
        return;
    }
    changeToState(EXPERIMENT);
    _initNewPayload(STARTEXPT);
    jedi.writePayload();
}

/* Handles STOPEXPT
 *  
 */
void handle_stopexpt() {
    if (prgState != EXPERIMENT) {
        return;
    }
    changeToState(NONE);
    _initNewPayload(STOPEXPT);
    jedi.writePayload();
}

/* Handles STARTSTREAM
 *  Can start stream only under the folliowing states: 
 *  NONE, BADERR, NORMAL
 *  Cannot start stream under the following state:
 *  EXPERIMENT, DOCKSTNCOMM
 */
void handle_startstream() {
    if (prgState == DOCKSTNCOMM) {
        return;
    }
    changeToState(STREAMING);
    _initNewPayload(STARTSTREAM);
    jedi.writePayload();
}

/* Handles STOPSTREAM
 *  Can stop stream only under the folliowing states: 
 *  NONE, BADERR, NORMAL
 *  Cannot stop stream under the following state:
 *  EXPERIMENT, DOCKSTNCOMM
 */
void handle_stopstream() {
    if (prgState != STREAMING) {
        return;
    }
    changeToState(NONE);
    _initNewPayload(STOPSTREAM);
    jedi.writePayload();
}

/* Handles STARTDOCKSTNCOMM
 *  
 */
void handle_startdockstncomm() {
    prgTimes.dkStnWd = prgTimes.curr;
    changeToState(DOCKSTNCOMM);
    _initNewPayload(STARTDOCKSTNCOMM);
    jedi.writePayload();
}

/* Handles STOPDOCKSTNCOMM
 *  
 */
void handle_stopdockstncomm() {
    if (prgState != DOCKSTNCOMM) {
        return;
    }
    changeToState(NONE);
    _initNewPayload(STOPDOCKSTNCOMM);
    jedi.writePayload();
}

/* Handles SETTOTNONE
 * 
 */
void handle_settonone() {
    changeToState(NONE);
    _initNewPayload(SETTONONE);
    jedi.writePayload();
}

/*
 * Initialize new payload.
 */
void _initNewPayload(byte cmd) {
    jedi.initPayload();
    jedi.addByte(cmd);
    jedi.addByte(prgState);
    jedi.addByte(err.code);
}

/* Function add a string to the payload when there is 
 * enough spaace left. It return true if it was added,  
 * else it returns false.
 */
bool _addStringToPayload(const char* str) {
    // Check if the new string will fit in the 
    // payload.
    if (255 - jedi.outPayloadSize() >= strlen(str)) {
        const char* c = str;
        while(*c != NULL) {
            jedi.addByte(*c);
            c++;
        }
        return true;
    }
    return false;
}

/* Function to send the contents of a file on the SD card.
 */
void _sendFileData(char* fname) {
    // First send the file size.
    File32 _dataFile;
    if (_dataFile.open(fname, O_READ) == false) {
        err.bits.DataFileNotReadable = 1;
        // No file to send.
        _initNewPayload(GETFILEDATA);
        jedi.addByte(NOFILE);
        jedi.writePayload();
        return;
    }
    // Get file size.
    err.bits.DataFileNotReadable = 0;
    unsigned int _fsz = _dataFile.size();
    unsigned int _sentsz = 0;
    /*
     * 1. SEND FILE SIZE.
     */
    _initNewPayload(GETFILEDATA);
    jedi.addByte(FILEHEADER);
    jedi.addUInt(_fsz);
    jedi.writePayload();
    /*
     * 2. SEND FILE SEGMENTS.
     */
    uint8_t _linesPerSeg = 12;
    uint8_t _bytesToRead = _linesPerSeg * _dataBuffer.packetSize;
    uint8_t _bytesRead;
    unsigned int _bytesSent = 0;
    uint8_t _prcnt; 
    byte _fileData[_bytesToRead];
    // Read segment.
    _bytesRead = _dataFile.read(_fileData, _bytesToRead);
    while (true) {
        _bytesSent += _bytesRead;
        _prcnt = _bytesSent * 255 / _fsz;
        _sendOneFileSegment(_fileData, _bytesRead, _prcnt);
        if (_bytesRead < _bytesToRead) {
            // Read everything.
            break;
        }
        // Read the next segment.
        _bytesRead = _dataFile.read(_fileData, _bytesToRead);
    }
    _dataFile.close();
}

/*
 * Function to send a single segment of data read from the 
 * file.
 */
void _sendOneFileSegment(byte *fileData, uint8_t bytesRead, uint8_t prcnt) {
    _initNewPayload(GETFILEDATA);
    jedi.addByte(FILECONTENT);
    jedi.addByte(prcnt);
    for (int i = 0; i < bytesRead; i++) {
        jedi.addByte(*(fileData + i));
    }
    jedi.writePayload();
}

/*
 * Send IMU data stream in the STREAMING mode.
 */
void _streamImuDataOut(imuunion_t *imuData,
                       shortrtcunion_t *shortRtcData) {
    _initNewPayload(STARTSTREAM);
    // Epoch data
    for (int i = 0; i < 7; i++) {
        jedi.addByte(shortRtcData->values[i]);
    }
    // IMU data.
    for (int i = 0; i < 12; i++) {
        jedi.addByte(imuData->bytes[i]);
    }
    jedi.writePayload();
}
