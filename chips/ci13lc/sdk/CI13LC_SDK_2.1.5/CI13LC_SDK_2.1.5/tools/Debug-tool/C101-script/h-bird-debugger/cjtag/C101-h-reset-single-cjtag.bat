
SET ROOT_PATH=%~dp0..\..\..

SET OPENOCD_PATH=%ROOT_PATH%\OpenOCD\2022.12\bin
SET CONFIG_PATH=%ROOT_PATH%\C101-script\h-bird-debugger\cjtag\config

tasklist|findstr -i "openocd.exe"
if ERRORLEVEL 0 (
	taskkill /F  /T /IM openocd.exe
)
chdir

@echo off

"%OPENOCD_PATH%\openocd.exe" -f "%CONFIG_PATH%\ftdi-reset-all.cfg"
"%OPENOCD_PATH%\openocd.exe" -f "%CONFIG_PATH%\ftdi-single-core-debug.cfg"

