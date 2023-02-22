# BLE Nordic UART Service (NUS) Terminal

`ble-nus-term` is a PC based tool written in Python for interacting with BLE Nordic UART service.

This tool works with Bosch Sensortec Application Board 3.0 running BLE enabled COINES examples and other BLE devices running Nordic UART service. 

Works with latest Bluetooth v4.0 USB dongles and recent notebook PCs with Bluetooth. Tested with CSR8510 dongle in Windows 10 (Build 16299 and above) and Ubuntu 20.04 LTS.

```
C:\>python ble-nus-term.py -d <device_mac_address>
```

---

## Usage

### Requirements
- [Python 3](https://www.python.org/downloads/)
- [bleak](https://pypi.org/project/bleak/) library
- [aioconsole](https://pypi.org/project/aioconsole/) library

### Install dependencies in `requirements.txt`
```
$ pip install -r requirements.txt
```
### Scan for BLE devices
```
C:\Tools> python ble-nus-term.py -l

Application Board 3.0 BLE NUS tool
Bosch Sensortec GmbH (C) 2020

Scanning for devices...
APP3.0 Board (NUS)        D7:A3:CE:8E:36:14      -56 dB
APP3.0 Board (NUS)        F5:0C:EF:B7:D5:35      -63 dB
Unknown                   C8:28:32:E1:39:CA      -68 dB
Unknown                   EA:23:43:92:06:15      -57 dB
```

### Connect to NUS enabled BLE device

Download a BLE enabled COINES example before running the below command.

```
C:\Tools> python ble-nus-term.py -d D7:A3:CE:8E:36:14
Application Board 3.0 BLE DFU Terminal
Bosch Sensortec GmbH (C) 2020

Connected to D7:A3:CE:8E:36:14

BMA400 Initialization Success!
Chip ID 0x90
t[s]:0.0206     data[m/s2]: ax:3.0837   ay:1.8483       az:8.7915
t[s]:0.0406     data[m/s2]: ax:3.0933   ay:1.8771       az:8.8873
....
....
....

```

### Show help
```
C:\tools> python ble-nus-term.py -h
Application Board 3.0 BLE NUS Terminal
Bosch Sensortec GmbH (C) 2020

usage: ble-nus-term.py [-h] [-l] [-d DEVICE_MAC_ADDR]

optional arguments:
  -h, --help          show this help message and exit
  -l, --list          Scan for BLE devices

required arguments:
  -d DEVICE_MAC_ADDR  Specify device MAC address
```
---

## Background information

BLE in the days of Windows 7, 8 required the use of specialized dongle like [Silicon Labs BLED112](https://www.silabs.com/wireless/bluetooth/bluegiga-low-energy-legacy-modules/device.bled112) , [TI CC2540 BLE USB](http://www.ti.com/tool/TIDC-CC2540-BLE-USB), [Nordic nRF51 dongle](https://www.nordicsemi.com/Software-and-tools/Development-Kits/nRF51-Dongle), etc ., which is locked to vendor specific software.

With the advent of Windows 10 Fall Creator's Update (Build 16299/Version 1709), BLE is well supported natively. This tool is an attempt to push the use of native Bluetooth hardware built in notebook PC and USB dongles without getting locked into vendor specific hardware and software.

---
