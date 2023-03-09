# ARm use IMU FIrmwaRE - ARIMUFire

ARIMU or _Arm use IMU_ is a wearable movement sensor based on the versatile, open-source data logger [OpenLog Artemis](https://www.sparkfun.com/products/16832) from SparkFun, developed by the [Biological Learning and Rehabilitation Group](http://biorehab.github.io) at the Department of Bioengineering, Christian Medical College Vellore, India.

We first describe the device ARIMU, and then provide the details of the current version of the firmware.

## ARIMU 
Arm Use IMU is essentially the OpenLog Artemis with custom firmware and enclosure. The current version of the device uses only the onboard IMU sensor, RTC, and the microSD card logging modules. 

## ARIMUFire 

ARIMUFire is a custom firmware written for the OpenLog Artemis to read and log IMU data to the SD card, to establish a custom two-way communication protocol through USB serial for:
1. streaming IMU data
2. reading and setting ARIMU variables and parameters, and
3. reading and managing files on the SD card. 

