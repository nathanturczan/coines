:: Script for converting all BIN files to CSV

@echo off
for %%i in (*.bin) do ( 
GLP_Decoder.exe -f %%i -p
GLP_Decoder.exe -f %%i -o %%~ni.csv
)