@echo off
echo Compiling debug_file_read...

g++ -std=c++17 -O2 -o debug_file_read debug_file_read.cpp

if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    echo Run with: debug_file_read
) else (
    echo Compilation failed!
)

pause
