@echo off
echo Compiling improved main with enhanced debug...

REM Compilar con todas las dependencias necesarias
g++ -std=c++17 -O2 -I"Source/Mrs/Include" -I"Source/FileSystem/Include" -I"Source/Mrs/zlib/include" ^
    -o mrs_improved_debug ^
    main_improved_debug.cpp ^
    Source/FileSystem/Source/FileSystem.cpp ^
    Source/Mrs/Source/stdafx.cpp ^
    -L"Source/Mrs/zlib/lib" -lzlib

if %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    echo Run with: mrs_improved_debug
) else (
    echo Compilation failed!
    echo Make sure you have the correct directory structure and zlib library.
)

pause
