@echo off
echo.
echo ================================================================================
echo Deleting previous results
del *_0??.bmp
echo.
echo ================================================================================
echo                                                                        arana.bmp
..\bin\topologia_digital.exe operations.txt arana.bmp
echo.
echo ================================================================================
echo                                                                     cangrejo.bmp
..\bin\topologia_digital.exe operations.txt cangrejo.bmp
echo.
echo ================================================================================
echo                                                                      cartero.bmp
..\bin\topologia_digital.exe operations.txt cartero.bmp
echo.
echo ================================================================================
echo                                                                     chess3x3.bmp
..\bin\topologia_digital.exe operations.txt chess3x3.bmp
echo.
echo ================================================================================
echo                                                                     cuadrado.bmp
..\bin\topologia_digital.exe operations.txt cuadrado.bmp
echo.
echo ================================================================================
echo                                                                         F5x3.bmp
..\bin\topologia_digital.exe operations.txt F5x3.bmp
echo.
echo ================================================================================
echo                                                                         F5x4.bmp
..\bin\topologia_digital.exe operations.txt F5x4.bmp
echo.
echo ================================================================================
echo                                                                         F5x5.bmp
..\bin\topologia_digital.exe operations.txt F5x5.bmp
echo.
echo ================================================================================
echo                                                                   mano24bits.bmp
..\bin\topologia_digital.exe operations.txt mano24bits.bmp
echo.
echo ================================================================================
echo                                                                        rugby.bmp
..\bin\topologia_digital.exe operations.txt rugby.bmp
echo.
echo ================================================================================
echo                                                                     telefono.bmp
..\bin\topologia_digital.exe operations.txt telefono.bmp
echo.
echo ================================================================================
echo                                                                      rh_test.bmp
..\bin\topologia_digital.exe operations.txt rh_test.bmp
echo.
echo ================================================================================
echo                                                                  gh89a2_test.bmp
..\bin\topologia_digital.exe operations.txt gh89a2_test.bmp
echo.
echo ================================================================================
echo                                                                        llave.bmp
..\bin\topologia_digital.exe operations.txt llave.bmp
pause