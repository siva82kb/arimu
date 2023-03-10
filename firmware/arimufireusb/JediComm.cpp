
#include <Arduino.h>
#include "JediComm.h"

JediComm::JediComm()
{
    _state = WAITFORPACKET;
    _serport = NULL;
    _sz = 0;
}

// Set serial port.
void JediComm::setSerialPort(HardwareSerial *serPort)
{
    _state = WAITFORPACKET;
    _serport = serPort;
    _sz = 0;
}

// Communication is through USB
bool JediComm::isThroughUSB()
{
    return _serport == NULL;
}

// Reader functions
int JediComm::readPayload()
{
    // Read data from the appropriate serial port.
    while (isDataAvailable())
    {
        // Read current byte
        _currByte = readByte();
        switch (_state)
        {
            case WAITFORPACKET:
                if (_currByte == 0xFF)
                {
                    // First header found.
                    _state = HEADER1;
                }
                break;
            case HEADER1:
                if (_currByte == 0xFF)
                {
                    // Second header found.
                    _state = HEADER2;
                }
                else
                {
                    _state = WAITFORPACKET;
                }
                break;
            case HEADER2:
                // Get the size of the payload.
                // It cannot be zero or greater than maxPayloadSize.
                if (_currByte == 0x00 || _currByte > maxPayloadSize)
                {
                    _state = WAITFORPACKET;
                }
                else
                {
                    _currPlSz = _currByte - 1;
                    _plCntr = 0;
                    _state = PAYLOAD;
                    _inChkSum = 0xFF + 0xFF + _currByte;
                }
                break;
            case PAYLOAD:
                inPayload[_plCntr++] = _currByte;
                _inChkSum += _currByte;
                if (_plCntr == _currPlSz)
                {
                    _state = CHKSUM;
                }
                break;
            case CHKSUM:
                if (_inChkSum == _currByte)
                {
                    _state = WAITFORHANDLING;
                }
                else
                {
                    _state = WAITFORPACKET;
                }
                break;
            case WAITFORHANDLING:
                break;
        }
        // Return payload size only if the current state is WAITFORHANDLING
        if (_state == WAITFORHANDLING)
        {
            return _currPlSz;
        }
        else
        {
            return -1;
        }
    }
    return -1;
}

void JediComm::payloadHandled()
{
    _currPlSz = -1;
    _state = WAITFORPACKET;
}

float JediComm::readFloatFromPayload(int inx)
{
    floatunion_t _data;
    for (int i = 0; i < 4; i++)
    {
        _data.bytes[i] = inPayload[inx + i];
    }
    return _data.num;
}

int JediComm::readIntFromPayload(int inx)
{
    intunion_t _data;
    for (int i = 0; i < 2; i++)
    {
        _data.bytes[i] = inPayload[inx + i];
    }
    return _data.num;
}

uint16_t JediComm::readUInt16FromPayload(int inx)
{
    intunion_t _data;
    for (int i = 0; i < 2; i++)
    {
        _data.bytes[i] = inPayload[inx + i];
    }
    return _data.num;
}

byte JediComm::getInPayloadSize()
{
    return _currPlSz;
}

bool JediComm::isDataAvailable()
{
    if (_serport == NULL)
    {
        return (Serial.available() > 0);
    }
    else
    {
        return (_serport->available() > 0);
        // return (Serial4.available() > 0);
    }
}

byte JediComm::readByte()
{
    if (_serport == NULL)
    {
        return Serial.read();
    }
    else
    {
        return _serport->read();
    }
}

byte JediComm::getByteFromInPayload(byte inx)
{
    return inPayload[inx];
}

int JediComm::getIntFromInPayload(byte inx)
{
    intunion_t _int;
    for (int i = 0; i < 2; i++)
    {
        _int.bytes[i] = inPayload[inx + i];
    }
    return _int.num;
}

uint16_t JediComm::getUint16FromInPayload(byte inx)
{
    uint16union_t _uint16;
    for (int i = 0; i < 2; i++)
    {
        _uint16.bytes[i] = inPayload[inx + i];
    }
    return _uint16.num;
}

float JediComm::getFloatFromInPayload(byte inx)
{
    floatunion_t _float;
    for (int i = 0; i < 4; i++)
    {
        _float.bytes[i] = inPayload[inx + i];
    }
    return _float.num;
}



// Writer functions.
void JediComm::initPayload()
{
    _outChkSum = 0xFE;
    _sz = 0;
}

byte JediComm::getOutPayloadSize()
{
    return _sz;
}

byte JediComm::addFloat(float data)
{
    _float.num = data;
    for (int i = 0; i < 4; i++)
    {
        _outPayload[_sz++] = _float.bytes[i];
        _outChkSum += _float.bytes[i];
    }
    return _sz;
}

byte JediComm::addInt(int data)
{
    _int.num = data;
    for (int i = 0; i < 4; i++)
    {
        _outPayload[_sz++] = _int.bytes[i];
        _outChkSum += _int.bytes[i];
    }
    return _sz;
}

byte JediComm::addUInt(unsigned int data)
{
    _uint.num = data;
    for (int i = 0; i < 4; i++)
    {
        _outPayload[_sz++] = _uint.bytes[i];
        _outChkSum += _uint.bytes[i];
    }
    return _sz;
}

byte JediComm::addByte(byte data)
{
    _outPayload[_sz++] = data;
    _outChkSum += data;
}

byte JediComm::addUInt16(uint16_t data)
{
    _uint16.num = data;
    for (int i = 0; i < 2; i++)
    {
        _outPayload[_sz++] = _uint16.bytes[i];
        _outChkSum += _uint16.bytes[i];
    }
    return _sz;
}

byte JediComm::addInt16(int16_t data)
{
    _int16.num = data;
    for (int i = 0; i < 2; i++)
    {
        _outPayload[_sz++] = _int16.bytes[i];
        _outChkSum += _int16.bytes[i];
    }
    return _sz;
}

byte JediComm::outPayloadSize() {
    return _sz;
}

void JediComm::writePayload()
{
    // 255 | 255 | No. of bytes | Payload | Chksum
    byte header[] = {0xFF, 0xFF, 0x00};

    // Send packet.
    header[2] = _sz + 1;
    _outChkSum += header[2];

    // Send header
    if (_serport == NULL)
    {
        Serial.write(header, 3);
        Serial.write(_outPayload, _sz);
        Serial.write(_outChkSum);
    }
    else
    {
        _serport->write(header, 3);
        _serport->write(_outPayload, _sz);
        _serport->write(_outChkSum);
    }
}
