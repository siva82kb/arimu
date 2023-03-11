# ARm use IMU FIrmwaRE - ARIMUFire

ARIMU or _Arm use IMU_ is a wearable movement sensor based on the versatile, open-source data logger [OpenLog Artemis](https://www.sparkfun.com/products/16832) from SparkFun, developed by the [Biological Learning and Rehabilitation Group](http://biorehab.github.io) at the Department of Bioengineering, Christian Medical College Vellore, India.

We first describe the device ARIMU, and then provide the details of the current version of the firmware.

## ARIMU 
Arm Use IMU is essentially the OpenLog Artemis with custom firmware and enclosure. The current version of the device uses only the onboard IMU sensor, RTC, and microSD card logging modules. ARIMU's schematic is shown in the following figure.

<p align="center">
  <img width="300" src="figs/arimu-schem.png">
</p>

The current version of ARIMU uses a 600mAh Lithium Polymer battery. The OpenLog board, the battery, and the switch are placed in a 3D-printed enclosure with holders for straps. The 3D model of the enclosure, its dimensions, and the arrangement of the different components inside the enclosure are shown below.

<p align="center">
  <img width="300" src="figs/arimu-3dmodel.gif">
</p>

The 3D model file for the ARIMU enclosure and its components can be found here.

Here is a video of the physical prototype of a fully assembled ARIMU with the straps for donning it around the wrist.


## ARIMUFire 

ARIMU does not use the native firmware that comes with the OpenLog. This was done for three reasons:
1. To allow the OpenLog to operate in logging or data streaming (via USB serial, BLE or any other wireless protocol through add-on hardware) modes. 
2. To be able to read and set custom device parameters through USB serial.
3. To be able to read and manage files on the SD card through USB serial.

The custom firmware - ARIMUFire - is implemented to allow for all these features and a few more. 

### Device parameters
There are several device parameters that are used in the current firmware.
- **Device name**: A name can be assigned to individual devices.
- **Subject name**: The user to which the device can be assigned; this can read and set through a USB serial protocol.
- **Max. size of a file**: There is maximum size for individual files logged onto the SD card. If the size of the current file exceed this, the device creates a new file.
- **Date time threshold**: A threshold for the datetime is set, and whenever the RTC value is below this, then the device will set and error flag to indicate that the RTC is out of sync.

### Data logging 
At every loop execution, ARIMUFire logs the RTC timestamp, the three values of the accelerometer, and the three values of the gyroscope. The data is logged in a binary format, which has a header, and the data packets with timestamps, accelerometer, and gyroscope values. Each data packet is 19 bytes long as shown below:
<style type="text/css">
.tg  {border-collapse:collapse;border-spacing:0;}
.tg td{border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:12px;
  overflow:hidden;padding:10px 5px;word-break:normal;}
.tg th{border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:12px;
  font-weight:normal;overflow:hidden;padding:10px 5px;word-break:normal;}
.tg .tg-yj5y{background-color:#efefef;border-color:inherit;text-align:center;vertical-align:top}
.tg .tg-7btt{border-color:inherit;font-weight:bold;text-align:center;vertical-align:top}
</style>
<table class="tg">
<thead>
  <tr>
    <th class="tg-7btt" colspan="7">Timestamp (7 bytes)</th>
    <th class="tg-7btt" colspan="3">Accelerometer (6 bytes)</th>
    <th class="tg-7btt" colspan="3">Gyroscope (6 bytes)</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td class="tg-7btt">Year</td>
    <td class="tg-7btt">Month</td>
    <td class="tg-7btt">Day</td>
    <td class="tg-7btt">Hour</td>
    <td class="tg-7btt">Minute</td>
    <td class="tg-7btt">Second</td>
    <td class="tg-7btt">10milsec</td>
    <td class="tg-7btt">ax<br></td>
    <td class="tg-7btt">ay<br></td>
    <td class="tg-7btt">az<br></td>
    <td class="tg-7btt">gx<br></td>
    <td class="tg-7btt">gy<br></td>
    <td class="tg-7btt">gz<br></td>
  </tr>
  <tr>
    <td class="tg-yj5y">1 byte</td>
    <td class="tg-yj5y">1 byte</td>
    <td class="tg-yj5y">1 byte</td>
    <td class="tg-yj5y">1 byte</td>
    <td class="tg-yj5y">1 byte</td>
    <td class="tg-yj5y">1 byte</td>
    <td class="tg-yj5y">1 byte</td>
    <td class="tg-yj5y">2 bytes</td>
    <td class="tg-yj5y">2 bytes</td>
    <td class="tg-yj5y">2 bytes<br></td>
    <td class="tg-yj5y">2 bytes</td>
    <td class="tg-yj5y">2 bytes</td>
    <td class="tg-yj5y">2 bytes</td>
  </tr>
</tbody>
</table>

The individual data filenames have the following format: "{Subject name}\_data\_{File number}.bin"

### Device LEDs
There is a red and blue LED on the OpenLog which are used to indicate the mode of operation of the ARIMU:
<style type="text/css">
.tg  {border-collapse:collapse;border-spacing:0;}
.tg td{border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:12px;
  overflow:hidden;padding:10px 5px;word-break:normal;}
.tg th{border-color:black;border-style:solid;border-width:1px;font-family:Arial, sans-serif;font-size:12px;
  font-weight:normal;overflow:hidden;padding:10px 5px;word-break:normal;}
.tg .tg-v0hj{background-color:#efefef;border-color:inherit;font-weight:bold;text-align:center;vertical-align:top}
.tg .tg-baqh{text-align:center;vertical-align:top}
.tg .tg-c3ow{border-color:inherit;text-align:center;vertical-align:top}
</style>
<table class="tg">
<thead>
  <tr>
    <th class="tg-v0hj">Mode</th>
    <th class="tg-v0hj">Red LED</th>
    <th class="tg-v0hj">Blue LED</th>
  </tr>
</thead>
<tbody>
  <tr>
    <td class="tg-c3ow">RTC error/SD card error<br></td>
    <td class="tg-c3ow">Fast blinking</td>
    <td class="tg-c3ow">Fast blinking</td>
  </tr>
  <tr>
    <td class="tg-c3ow">Normal SD logging</td>
    <td class="tg-c3ow">Off</td>
    <td class="tg-c3ow">Blinks every<br>5 sec</td>
  </tr>
  <tr>
    <td class="tg-baqh">Data communication<br>(via USB serial)</td>
    <td class="tg-baqh">On</td>
    <td class="tg-baqh">On</td>
  </tr>
</tbody>
</table>