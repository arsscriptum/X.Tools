@echo off


:: ==============================================================================
:: 
::   DevConsole.bat
::   VERSION:  1.0.1
::
:: ==============================================================================
::   codecastor - made in quebec 2020 <codecastor@icloud.com>
:: ==============================================================================


color 0A
echo ==============================================================================
echo. 
echo   DevConsole.bat
echo   VERSION:  1.0.1
echo.
echo ==============================================================================
echo   codecastor - made in quebec 2020 <codecastor@icloud.com>
echo ==============================================================================

pushd %VS140COMNTOOLS%
call vsvars32.bat
call VsDevCmd.bat
popd
set | findstr WindowsSDK