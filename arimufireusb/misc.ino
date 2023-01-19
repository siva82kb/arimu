/*
 * Set of miscellaneous functions for handling other ARIMU 
 * fucntionalities.s
 */

// Read current time
void readTime() {
  rtc.getTime();
   datetime = rtc.getEpoch();
  _rtcData.values[0] = rtc.year;
  _rtcData.values[1] = rtc.month;
  _rtcData.values[2] = rtc.dayOfMonth;
  _rtcData.values[3] = rtc.hour;
  _rtcData.values[4] = rtc.minute;
  _rtcData.values[5] = rtc.seconds;
  _rtcData.values[6] = rtc.hundredths;
}

// Update the short RTC bytes array based on the current RTC data.
void update_short_rtc_bytes() {
    // This function assumes that the short RTC
    // bytes.
    for (int i = 0; i < 7; i++) {
        _shortRtcData.values[i] = (byte)_rtcData.values[i];
    }
}

// Update RTC flag.
void updateRtcErrFlag() {
    readTime();
    if (datetime > VALIDEPOCH) {
        err.bits.RtcNotSet = 0;
    } else {
        err.bits.RtcNotSet = 1;
    }
}

// Check if we have a bad error.
bool isABadError() 
{
    for(int i = 0; badErrBitLocs[i] != 255; i++){
        if ((err.code >> badErrBitLocs[i]) % 2 == 1) {
            return true;
        }
    }
    return false;
}

// Init Timers
void initTimers() {
    prgTimes.curr = micros();
    prgTimes.errDisp = micros();
    prgTimes.normExpt = micros();
    prgTimes.normDisp = micros();
    prgTimes.exptDisp = micros();
    prgTimes.strmDisp = micros();
    prgTimes.dkStnWd = micros();
}
