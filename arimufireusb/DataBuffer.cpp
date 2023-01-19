#include <Arduino.h>
#include "DataBuffer.h"


DualDataBuffer::DualDataBuffer()
{
    _pageNo = 0;
    _lineNo = 0;
    readyPage = NULL;
}

void DualDataBuffer::initPages()
{
    _pageNo = 0;
    _lineNo = 0;
    readyPage = NULL;
}

bool DualDataBuffer::addLine(imuunion_t *imuData,
                             rtcunion_t *rtcData,
                             microsunion_t *microsData)
{
    // Add to the current page.
    // Write time data.
    int inx = packetSize * _lineNo;
    for (int i = 0; i < 28; i++) {
        _dataPages[_pageNo][inx + i] = rtcData->bytes[i];
    }
    inx += 28;
    // Write micros data.
    for (int i = 0; i < 4; i++) {
        _dataPages[_pageNo][inx + i] = microsData->bytes[i];
    }
    inx += 4;
    // Write IMU data.
    for (int i = 0; i < 12; i++) {
        _dataPages[_pageNo][inx + i] = imuData->bytes[i];
    }
    _lineNo++;
    
    // Check if the page is full.
    if (_lineNo == linesPerPage) {
        readyPage = _dataPages[_pageNo];
        _pageNo = (_pageNo + 1) % 2;    // Change page no.
        _lineNo = 0;                    // Reset line no.
        return true;
    }
    return false;
}

bool DualDataBuffer::addLine(imuunion_t *imuData, shortrtcunion_t *shortRtcData)
{
    // Add to the current page.
    // Write time data.
    int inx = packetSize * _lineNo;
    for (int i = 0; i < 7; i++) {
        _dataPages[_pageNo][inx + i] = shortRtcData->values[i];
    }
    inx += 7;
    // Write IMU data.
    for (int i = 0; i < 12; i++) {
        _dataPages[_pageNo][inx + i] = imuData->bytes[i];
    }
    _lineNo++;
    
    // Check if the page is full.
    if (_lineNo == linesPerPage) {
        readyPage = _dataPages[_pageNo];
        _pageNo = (_pageNo + 1) % 2;    // Change page no.
        _lineNo = 0;                    // Reset line no.
        return true;
    }
    return false;
}

void DualDataBuffer::doneWithReadyPages()
{
    readyPage = NULL;
}
