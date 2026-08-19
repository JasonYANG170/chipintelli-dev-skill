SET ROOT_PATH=..\..\..

SET OPENOCD_PATH=%ROOT_PATH%\OpenOCD\2022.12\bin
SET CONFIG_PATH=%ROOT_PATH%\C101-script\h-bird-debugger\cjtag\config

"%OPENOCD_PATH%\openocd.exe" -f "%CONFIG_PATH%\ftdi-dual-core-debug.cfg"

