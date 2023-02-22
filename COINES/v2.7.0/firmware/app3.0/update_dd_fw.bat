:: This script updates APP3.0 board with Development Desktop firmware
:: Switching to bootloader mode is automatic !

@echo off
..\..\tools\app_switch\app_switch usb_dfu_bl
..\..\tools\usb-dfu\dfu-util --device -,108c:ab3d -a FLASH -D DevelopmentDesktop_2.0_Firmware_v1.9.bin -R
pause