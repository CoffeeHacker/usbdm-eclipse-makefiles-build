@echo off
cls
set VERSION=1_0_1
set MSI_FILE=USBDM_Drivers_%VERSION%_Win_x64

msiexec /i %MSI_FILE% /l*v usbdmInstall.log