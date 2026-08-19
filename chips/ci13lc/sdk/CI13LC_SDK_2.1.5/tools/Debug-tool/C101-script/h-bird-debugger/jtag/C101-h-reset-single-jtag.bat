SET ROOT_PATH=%HOMEPATH%\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Debug-tool

SET OPENOCD_PATH=%ROOT_PATH%\OpenOCD\2022.12\bin
SET CONFIG_PATH=%ROOT_PATH%\C101-script\h-bird-debugger\jtag\config

"%OPENOCD_PATH%\openocd.exe" -f "%CONFIG_PATH%\ftdi-reset-all.cfg"
"%OPENOCD_PATH%\openocd.exe" -f "%CONFIG_PATH%\ftdi-single-core-debug.cfg"
