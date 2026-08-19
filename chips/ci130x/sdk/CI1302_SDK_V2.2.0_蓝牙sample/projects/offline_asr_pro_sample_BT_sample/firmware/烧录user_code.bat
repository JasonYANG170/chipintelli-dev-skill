@echo off
setlocal enabledelayedexpansion

set TOOLS_PATH=..\..\..\tools

echo project offline_asr_sample

@REM copy ..\..\bnpu_core\firmware\user_code\[1]code.bin  .\user_code

echo make user.bin
%TOOLS_PATH%\ci-tool-kit.exe merge user-file -i user_code


%TOOLS_PATH%\code_program.exe user_code\user_code.bin %1




