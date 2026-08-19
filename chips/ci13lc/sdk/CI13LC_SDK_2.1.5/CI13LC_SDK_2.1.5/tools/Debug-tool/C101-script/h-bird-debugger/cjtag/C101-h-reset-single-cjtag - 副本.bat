
SET ROOT_PATH="..\..\.."

SET OPENOCD_PATH=%ROOT_PATH%\OpenOCD\2022.12\bin
SET CONFIG_PATH=%ROOT_PATH%\C101-script\h-bird-debugger\cjtag\config

tasklist|findstr -i "openocd.exe"
if ERRORLEVEL 0 (
	taskkill /F  /T /IM openocd.exe
)
chdir

@echo off
@REM for /l %%i in (1,1,2000000) do echo %%i >nul


"%OPENOCD_PATH%\openocd.exe" -f "%CONFIG_PATH%\ftdi-reset-all.cfg"
@rem "%OPENOCD_PATH%\openocd.exe" -f "%CONFIG_PATH%\ftdi-single-core-debug.cfg"

@REM for /l %%i in (1,1,2000000) do echo %%i >nul
