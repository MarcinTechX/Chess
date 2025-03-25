@echo off
if exist build.local.bat (
    call build.local.bat
) 
if "%SFML_DIR%"=="" (
    echo Nie zdefiniowano ścierzki do SFML'a !!!
    echo   Utwórz plik build.local.bat z zawartością "set SFML_DIR=..."
    echo   Lub zdefiniuj zmienną środowiskową SFML_DIR
    exit /b 1
)
echo SFML_DIR: %SFML_DIR%

:::::::::: VARIABLES :::::::::::

set SOURCES=chess.cpp board.cpp piece.cpp pawn.cpp bishop.cpp king.cpp rook.cpp knight.cpp queen.cpp
set OUTPUT=dist\chess.exe

:::::::::: BUILD :::::::::::

@echo on
g++ %SOURCES% -o %OUTPUT% -I%SFML_DIR%\include -L%SFML_DIR%\lib -lsfml-graphics -lsfml-window -lsfml-system
@echo off

if ERRORLEVEL 1 (
    exit /b %errorlevel%
)

echo Compilation Success!

if "%1"=="" (
    exit /b 0
)

:::::::::: INSTALL :::::::::::
echo Installing...

:: Copy Images folder
xcopy Images dist\Images /Y /Q /I /E

:: potrzebne DLL przekopiuj ręcznie