/*
   JediComm.h defines the class for handling reading and writing of serial
   data in the BioRehab Group's JEDI forma.

   May the force be with you.

   Author: Sivakumar Balasubramanian
   Created on: 2021-05-14 10:00:16
*/

#ifndef JEDICOMM_H
#define JEDICOMM_H

#include <Arduino.h>
#include "CustomDataStructures.h"

enum ReaderState {
  WAITFORPACKET,
  HEADER1,
  HEADER2,
  PAYLOAD,
  CHKSUM,
  WAITFORHANDLING
};

class JediComm {
	public:
		static const byte maxPayloadSize = 255;
		byte inPayload[maxPayloadSize];
		JediComm();
		// Set serial port.
		void setSerialPort(HardwareSerial *serPort);
		bool isThroughUSB();
		// Reader functions
		int readPayload();
		void payloadHandled();
		float readFloatFromPayload(int);
		int readIntFromPayload(int);
		uint16_t readUInt16FromPayload(int);
		byte getInPayloadSize();
		bool isDataAvailable();
		byte readByte();
		byte getByteFromInPayload(byte inx);
		int getIntFromInPayload(byte inx);
		uint16_t getUint16FromInPayload(byte inx);
		float getFloatFromInPayload(byte inx);
		// Writer functions.
		void initPayload();
		byte addFloat(float data);
        byte addInt(int data);
        byte addUInt(unsigned int data);
		byte addByte(byte data);
		byte addUInt16(uint16_t data);
		byte addInt16(int16_t data);
		byte getOutPayloadSize();
		void writePayload();
        byte outPayloadSize();
	private:
		// Reader related stuff
		bool temp = false;
		HardwareSerial *_serport;
		byte _currByte;
		byte _currPlSz;
		byte _plCntr;
		byte _inChkSum;
		ReaderState _state;

		// Writer related stuff
		byte _outPayload[maxPayloadSize];
		floatunion_t _float;
		intunion_t _int;
        uintunion_t _uint;
        int16union_t _int16;
		uint16union_t _uint16;
		byte _sz;
		byte _outChkSum;
};

#endif
