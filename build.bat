@echo off
if exist build.local.bat (
    call build.local.bat
) 
if "%SFML_DIR%"=="" (
    echo Nie zdefiniowano scierzki do SFML'a !!!
    echo   Utworz plik "build.local.bat" z zawartoscia "set SFML_DIR=<tu wstaw scierzke>"
    echo   Lub zdefiniuj zmienna srodowiskowa SFML_DIR, ze scierzka do folderu SFML
    exit /b 1
)
echo SFML_DIR: %SFML_DIR%

:::::::::: VARIABLES :::::::::::

set SOURCES=chess.cpp board.cpp piece.cpp pawn.cpp bishop.cpp king.cpp rook.cpp knight.cpp queen.cpp colorconverter.cpp soundmanager.cpp
set OUTPUT=dist\chess.exe

:::::::::: BUILD :::::::::::

@echo on
g++ %SOURCES% -o %OUTPUT% -O3 -I%SFML_DIR%\include -L%SFML_DIR%\lib -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system %*
@echo off

if ERRORLEVEL 1 (
    exit /B %errorlevel%
)

echo Compilation Success!