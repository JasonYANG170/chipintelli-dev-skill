SET ROOT_PATH=%HOMEPATH%\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Debug-tool


SET OPENOCD_PATH=%ROOT_PATH%\OpenOCD\2022.12\bin
SET CONFIG_PATH=%ROOT_PATH%\CI130x-script\h-bird-debugger\config

@rem "%OPENOCD_PATH%\openocd.exe" -f "%CONFIG_PATH%\ftdi-reset-all.cfg"
@rem "%OPENOCD_PATH%\openocd.exe" -f "%CONFIG_PATH%\ftdi-single-core-debug.cfg"
"%OPENOCD_PATH%\openocd.exe" -f "%CONFIG_PATH%\ftdi-reset-and-single.cfg