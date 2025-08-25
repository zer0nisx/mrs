@echo off
echo Compiling debug_signatures_advanced...

g++ -std=c++17 -O2 -o debug_signatures_advanced debug_signatures_advanced.cpp

if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    echo Run with: debug_signatures_advanced
) else (
    echo Compilation failed!
)

pause
