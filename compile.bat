@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

SET SRC_DIR=src
SET OBJ_DIR=build
SET INCLUDE_DIR=include
SET OUT_DIR=bin
SET EXEC=easychip8.exe

SET SDL_DIR=include\SDL2-2.30.9
SET SDL_LIB_DIR=%SDL_DIR%\lib\x64
SET SDL_DLL=%SDL_LIB_DIR%\SDL2.dll
SET SDL_INCLUDE_DIR=%SDL_DIR%\include
SET SDL_LINK_FLAGS=-L"%SDL_LIB_DIR%" -lSDL2main -lSDL2

SET INCLUDE_FLAGS=-I"%INCLUDE_DIR%" -I"%SDL_INCLUDE_DIR%"

SET SOURCES=main.cpp EasyChip8.cpp Screen.cpp
SET OBJECTS=%OBJ_DIR%\main.o %OBJ_DIR%\EasyChip8.o %OBJ_DIR%\Screen.o

IF NOT EXIST "%OBJ_DIR%" (
    mkdir "%OBJ_DIR%"
)

IF NOT EXIST "%OUT_DIR%" (
    mkdir "%OUT_DIR%"
    IF EXIST "%SDL_DLL%" (
        move "%SDL_DLL%" "%OUT_DIR%"
    )
)

for %%f in (%SOURCES%) do (
    g++ -Wall %INCLUDE_FLAGS% -c "%SRC_DIR%\%%f" -o "%OBJ_DIR%\%%~nf.o"
)

g++ -o "%OUT_DIR%\%EXEC%" %OBJECTS% %SDL_LINK_FLAGS%

rmdir /S /Q "%OBJ_DIR%"

pause
