@echo off
TITLE Debugger by JoostinOnline
echo Debugger by JoostinOnline (HacksDen.com)
echo ________________________________________
echo.
echo.

:: Gets the directory that powerpc-eabi-gdb.exe is installed to
setlocal
set devpath=%DEVKITPPC2:/=\%\
set devpath=%devpath:~1,1%:%devpath:~2%
if "%DEVKITPPC2%"=="" (echo DevkitPro is not installed! & goto :eof)

:: If no file was dragged/dropped onto the batch file, use the default filename.
if [%1]==[] goto setdefault

:: Make sure it is an elf file
if not "%~x1" == ".elf" (echo Error: Not an elf file! & goto eof)

:: Set the elf file to what was dragged/dropped onto the bat
set file=%1
goto debug

:setdefault
:: Make sure minecraft-wii-edition.elf exists
if not exist minecraft-wii-edition.elf (echo Error: minecraft-wii-edition.elf does not exist! & echo Try dragging-and-dropping an elf file onto Debug.bat & goto eof)
:: Set minecraft-wii-edition.elf as the file to debug
set file=minecraft-wii-edition.elf

:debug
echo Type "info line *0x#", replacing # with the stack dump entry
echo.
echo Example:  info line *0x800a96f8
echo.
echo.
%devpath%powerpc-eabi-gdb.exe %file%


:eof
echo.
echo Press any key to exit...
pause>NUL