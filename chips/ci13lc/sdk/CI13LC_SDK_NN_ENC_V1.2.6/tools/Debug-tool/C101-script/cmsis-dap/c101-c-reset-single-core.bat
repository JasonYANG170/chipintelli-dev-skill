SET ROOT_PATH=%HOMEPATH%\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Debug-tool

SET OPENOCD_PATH=%ROOT_PATH%\OpenOCD\2022.12\bin
SET CONFIG_PATH=%ROOT_PATH%\C101-script\cmsis-dap\config

"%OPENOCD_PATH%\openocd.exe" -f "%CONFIG_PATH%\openocd_ci130x_reset_all.cfg"
"%OPENOCD_PATH%\openocd.exe" -f "%CONFIG_PATH%\openocd_dap_ci130x-2.cfg"

pause