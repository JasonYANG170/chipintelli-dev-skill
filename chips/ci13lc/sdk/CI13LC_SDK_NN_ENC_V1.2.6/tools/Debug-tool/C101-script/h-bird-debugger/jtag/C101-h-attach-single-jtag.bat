SET OPENOCD_PATH=%HOMEPATH%\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Debug-tool\OpenOCD\2022.12\bin
SET CONFIG_PATH=%HOMEPATH%\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Debug-tool\C101-script\h-bird-debugger\jtag\config

"%OPENOCD_PATH%\openocd.exe" -f "%CONFIG_PATH%\ftdi-dual-core-debug.cfg"


