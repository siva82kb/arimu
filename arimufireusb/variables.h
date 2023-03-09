/*
 * Header file that contains variable declarations for the IMU watch BLE program.
 * 
 * Author: Tanya Subash, Sivakumar Balasubramanian
 * Created on: 12 April 2022
 */
 
#ifndef VARIABLES_H
#define VARIABLES_H

#include "Arduino.h"
#include "ICM_20948.h"  
#include "RTC.h"
#include <SdFat.h>
#include "JediComm.h"
#include "DataBuffer.h"

#define DATA_FILE_INDEX_MAX     999
#define DATA_FILE_PREFIX        "data" 
#define DATA_FILE_SUFFIX        ".bin"
#define SD_MAX_FILE_SIZE        5 * 1024 * 1024
#define VALIDEPOCH              1658547828  // Saturday, July 23, 2022 9:13:48 AM GMT+05:30

//Allowable number of 0 byte binary files on the SD card
#define NULLTHRESHOLD           5

// Device name
//char* ARIMUNAME = "ARIMU_Right";
char* ARIMUNAME = "ARIMU_RIGHT";

//Metadata for sending files
const byte DATALINE = 0;
const byte EOFLINE = 1;
const byte ERRORBYTE = 2;

// IMU related parameters
#define IMU_INIT_RETRY_LIMIT    10

//Commands received by ARIMU
#define STATUS                  0
#define PING                    1
#define LISTFILES               2
#define GETFILEDATA             3
#define DELETEFILE              4
#define GETMICROS               5
#define SETTIME                 6
#define GETTIME                 7
#define STARTSTREAM             8
#define STOPSTREAM              9
#define SETSUBJECT              10
#define GETSUBJECT              11
#define STARTEXPT               12
#define STOPEXPT                13
#define STARTDOCKSTNCOMM        14
#define STOPDOCKSTNCOMM         15
#define DOCKSTNPING             16
#define STARTNORMAL             17
#define STOPNORMAL              18
#define SETTONONE               19
#define CURRENTFILENAME         128

//Other commands
#define NOFILE                  0
#define FILESEARCHING           1
#define FILEHEADER              2
#define FILECONTENT             3
#define FILEDELETED             4
#define FILENOTDELETED          5


// IMU Reading delay.
#define IMUDELAY                6000

// LED Blinking timings (microseconds)
#define ERRORON                 100000
#define ERROROFF                100000
#define NORMALDELAY             10000
#define NORMALON                50000
#define NORMALOFF               2450000
#define EXPTON                  100000
#define EXPTOFF                 900000
#define STRMON                  250000
#define STRMOFF                 250000
#define DOCKSTNCOMMWD           10000000

// Different program states
enum progState {
    NONE = 0,
    BADERR = 1,
    NORMAL = 2,
    EXPERIMENT = 3,
    DOCKSTNCOMM = 4,
    STREAMING = 5,
};

//OLA specifics
const byte PIN_IMU_POWER = 27; // The Red SparkFun version of the OLA (V10) uses pin 27
const byte PIN_IMU_INT = 37;
const byte PIN_IMU_CHIP_SELECT = 44;
const byte PIN_SPI_SCK = 5;
const byte PIN_SPI_CIPO = 6;
const byte PIN_SPI_COPI = 7;
const byte PIN_VIN_MONITOR = 34;
const byte PIN_MICROSD_CHIP_SELECT = 23;
const byte BLUE_LED = 29; //19;
const byte RED_LED = 19; //29;


// Error Status Structure.
typedef struct ErrorBits {
    unsigned ImuNotInitialized : 1;
    unsigned SdCardNotConnected : 1;
    unsigned RtcNotSet : 1;
    unsigned DataFileNotCreated : 1;
    unsigned DataFileNotReadable : 1;
    unsigned DataFileNotFound : 1;
};

uint8_t badErrBitLocs[] = {0, 1, 2, 255};

typedef union ErrorStatus {
    ErrorBits bits;
    uint8_t code;
};

// Error status
ErrorStatus err;
// Program state
progState prgState = NONE;

// SD card
SdFat32 sd;
File32 dataFile;
int lastDataLogSyncTime = 0;

// SD card logging related variables.
struct {
    String subjName = "noone";
    uint16_t fIndex = 0;
    String currFileName = ""; // We keep a record of this filename
                              // so that we can re-open it upon wakeup
                              // from sleep.
} dataFileParams;

bool ledState = LOW;
bool sdCardPresent = false;

// USB Serial Communication with JEDI protocol
JediComm jedi;

/*
 * Program timing
 */
struct {
    unsigned long curr = 0;
    unsigned long normExpt = 0;
    unsigned long errDisp = 0;    
    unsigned long normDisp = 0;
    unsigned long exptDisp = 0;
    unsigned long strmDisp = 0;
    unsigned long dkStnWd = 0;
} prgTimes;

/*
 * timer
 */
unsigned long datetime = 0;

/*
 * IMU data variables.
 */
DualDataBuffer _dataBuffer;
microsunion_t _microsData;
//epochunion_t _epochData;
rtcunion_t _rtcData;
shortrtcunion_t _shortRtcData;
imuunion_t _imuData;

#define USE_SPI                     // Uncomment this to use SPI
#define SPI_PORT SPI                // Your desired SPI port.       Used only when "USE_SPI" is defined
#define CS_PIN PIN_IMU_CHIP_SELECT  // Which pin you connect CS to. Used only when "USE_SPI" is defined. OLA uses pin 44.
#define WIRE_PORT Wire              // Your desired Wire port.      Used when "USE_SPI" is not defined
#define AD0_VAL   1                 // The value of the last bit of the I2C address. 
                                    // On the SparkFun 9DoF IMU breakout the default is 1, and when 
                                    // the ADR jumper is closed the value becomes 0
#ifdef USE_SPI
  ICM_20948_SPI ICM;  // If using SPI create an ICM_20948_SPI object
#else
  ICM_20948_I2C ICM;  // Otherwise create an ICM_20948_I2C object
#endif

#endif
