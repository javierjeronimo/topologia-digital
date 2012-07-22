@echo off
echo.
echo ================================================================================
echo Deleting previous results
del llave_0??.bmp
echo.
echo ================================================================================
echo                                                                        arana.bmp
..\bin\topologia_digital.exe operations.txt llave.bmp
pause