/*
 * Set of functions to handle SD card logging for ARIMU.
 */

void initSD() {
    // SD.begin should return true if a valid SD card is present
    if (not isSDPresent()) {
        err.bits.SdCardNotConnected = 1;
    }
    else {
        err.bits.SdCardNotConnected = 0;
    }
}

bool isSDPresent() {
  return sd.begin(PIN_MICROSD_CHIP_SELECT);
}

String nextDataFileName()
{
    String _newfname;
    while (dataFileParams.fIndex < DATA_FILE_INDEX_MAX) {
        _newfname = dataFileParams.subjName + String("_")
                    + DATA_FILE_PREFIX
                    + String("_")
                    + String(dataFileParams.fIndex)
                    + DATA_FILE_SUFFIX;
        if (!sd.exists(_newfname)) {
            return _newfname;
        }
        dataFileParams.fIndex++;
    }
    return String("");
}

String nextDataFileNameWithRtc()
{
    // Check the state to determine the exact filename
    String _newfname;
    _newfname = dataFileParams.subjName
                + String("_")
                + DATA_FILE_PREFIX
                + String("_")
                + ARIMUNAME
                + String("_")
                + String(rtc.getEpoch())
                + DATA_FILE_SUFFIX;
    return _newfname;
}

String nextDataFileNameWithRtcForExpt()
{
    // Check the state to determine the exact filename
    String _newfname;
    _newfname = dataFileParams.subjName
                + String("_")
                + DATA_FILE_PREFIX
                + String("_")
                + ARIMUNAME
                + String("_expt_")
                + String(rtc.getEpoch())
                + DATA_FILE_SUFFIX;
    return _newfname;
}

void openNewDataFile() {
    // Get the right file name.
    dataFileParams.currFileName = _getNextFileName();
    const char* _fname = dataFileParams.currFileName.c_str();
    if (dataFile.open(_fname, O_CREAT | O_WRITE) == false) {
        err.bits.DataFileNotCreated = 1;
        return;
    }
    // File opened.
    err.bits.DataFileNotCreated = 0;
    _writeFileHeader();
    dataFile.sync();
}

String _getNextFileName() {
    switch(prgState) {
        case NORMAL:
            return nextDataFileNameWithRtc();
        case EXPERIMENT:
            return nextDataFileNameWithRtcForExpt();
    }
    return String("default.bin");
}

void _writeFileHeader() {
    intunion_t _int;
    microsunion_t _micros;
    rtc.getTime();
    _micros.microsecs = micros();
    // Device name
    dataFile.write(ARIMUNAME, strlen(ARIMUNAME));
    dataFile.write(",", 1);
    // Subject nmae
    dataFile.write(dataFileParams.subjName.c_str(),
                   dataFileParams.subjName.length());
    dataFile.write(",", 1);
    // RTC time
    _int.num = rtc.year + 2000;
    dataFile.write(_int.bytes, 4);
    _int.num = rtc.month;
    dataFile.write(_int.bytes, 4);
    _int.num = rtc.dayOfMonth;
    dataFile.write(_int.bytes, 4);
    _int.num = rtc.hour;
    dataFile.write(_int.bytes, 4);
    _int.num = rtc.minute;
    dataFile.write(_int.bytes, 4);
    _int.num = rtc.seconds;
    dataFile.write(_int.bytes, 4);
    _int.num = rtc.hundredths;
    dataFile.write(_int.bytes, 4);
    // Micros data
    dataFile.write(_micros.bytes, 4);
    dataFile.sync();
    // Date created
    dataFile.timestamp(T_CREATE, (rtc.year + 2000), rtc.month, rtc.dayOfMonth,
                       rtc.hour, rtc.minute, rtc.seconds);
}

// Get the number of file names.
uint16_t get_filecount() {
    int nullFile = 0;
    uint16_t _fcnt = 0;
    char _fname[50];
    File32 entry;

    if (isSDPresent())
    {
        File32 root = sd.open("/");
        while (true) 
        {
            entry =  root.openNextFile();
            
            // NULL file.
            if (!entry) { nullFile++; }
            if (nullFile > NULLTHRESHOLD) { break; }
            
            // Not a NULL file.
            entry.getName(_fname, 50);
            if (String(_fname).indexOf(DATA_FILE_SUFFIX) > 0) {
                _fcnt++;
            }
            entry.close();
            if (String(_fname) == "") { break; }
        }
        root.close();
    }
    return _fcnt;
}

void _print_allfilenames() {
    int nullFile = 0;
    char _fname[50];
    File32 entry;
    if (isSDPresent())
    {
        File32 root = sd.open("/");
        while (true) 
        {
            entry =  root.openNextFile();
            
            // NULL file.
            if (!entry) { nullFile++; }
            if (nullFile > NULLTHRESHOLD) { break; }
            
            // Not a NULL file.
            entry.getName(_fname, 50);
            if (String(_fname).indexOf(DATA_FILE_SUFFIX) > 0) {
                Serial.println(String(_fname).c_str());
            }
            entry.close();
            if (String(_fname) == "") { break; }
        }
        root.close();
    }
}

void writePageToFile() {
    if (_dataBuffer.readyPage!= NULL) {
        if (dataFile.size() > (SD_MAX_FILE_SIZE))  {
            closeDataFile();
            openNewDataFile();
        } else {
            dataFile.write(_dataBuffer.readyPage,
                           _dataBuffer.pageSize);
            dataFile.sync();
        }
    }
}

void closeDataFile() {
    // Write micros when closing.
    microsunion_t _micros;
    _micros.microsecs = micros();
    dataFile.write(_micros.bytes, 4);
    dataFile.sync();
    dataFile.close();
}
