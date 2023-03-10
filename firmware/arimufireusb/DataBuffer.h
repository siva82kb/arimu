/*
 * Class and functions to implement the Data Buffer functionality
 * for ARIMU.
 * 
 * Author: Sivakumar Balasubramanian
 * Date: 13 July 2022
 * Email: siva82kb@gmail.com
 */

#ifndef DATABUFFER_H
#define DATABUFFER_H

#include "Arduino.h"

// IMU data packet
typedef union {
  short accgyr[6];
  uint8_t bytes[12];
} imuunion_t;

// Time epoch data
typedef union {
    unsigned long epoch;
    uint8_t bytes[4];
} epochunion_t;

// RTC data
typedef union {
    int values[7];
    uint8_t bytes[7 * 4];
} rtcunion_t;

// RTC short data
typedef struct {
    byte values[7];
} shortrtcunion_t;

// micros data
typedef union {
    unsigned long microsecs;
    uint8_t bytes[4];
} microsunion_t;

class DualDataBuffer {
    public:
        static const uint8_t packetSize = 7 + 12;
        static const uint8_t noOfPages = 2;
        static const uint8_t linesPerPage = 200;
        static const uint16_t pageSize = packetSize * linesPerPage;
        const uint8_t *readyPage;
        DualDataBuffer();
        void initPages();
        bool addLine(imuunion_t *imuData,
                     rtcunion_t *rtcData,
                     microsunion_t *microsData);
        bool addLine(imuunion_t *imuData,
                     shortrtcunion_t *shortRtcData);
        void doneWithReadyPages();
    private:
        bool _ready;
        byte _pageNo = 0;
        byte _lineNo = 0;
        uint8_t _dataPages[noOfPages][pageSize];
};


#endif
