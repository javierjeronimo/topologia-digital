#!/bin/sh
echo
echo ================================================================================
echo Deleting previous results
rm llave_0??.bmp
echo
echo ================================================================================
echo                                                                        llave.bmp
../bin/topologia_digital operations.txt llave.bmp
