@echo off

:::::::::: INSTALL :::::::::::
echo Installing...

:: Copy Assets folders
xcopy Images dist\Images /Y /Q /I /E
xcopy Sounds dist\Sounds /Y /Q /I /E

:: potrzebne DLL przekopiuj ręcznie