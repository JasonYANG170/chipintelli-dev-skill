@echo off
setlocal enabledelayedexpansion

set SDK_PATH=..\..\..

for /f "tokens=1-3" %%i in (../project_file/makefile) do (
	if "%%i" equ "SDK_PATH" (
		set SDK_PATH=%%k
	)
)
set SDK_PATH=!SDK_PATH:/=\!
set TOOLS_PATH=!SDK_PATH!\tools

del /f /s /q  user_file\[60000]*.xls.bin


echo make asr.bin
%TOOLS_PATH%\ci-tool-kit.exe merge asr-file -i asr


echo make user_file.bin
cd user_file\cmd_info
call cmd_info.bat
cd ..\..
copy user_file\cmd_info\*.bin user_file\
%TOOLS_PATH%\ci-tool-kit.exe merge user-file -i user_file


goto:eof
@echo on
