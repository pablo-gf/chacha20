@echo off
:: Clock-cycle benchmark for Windows (same as clock_cycles_tests.sh).
:: Usage: bench\clock_cycles_tests.bat <binary> [csv_file] [runs]
::   e.g. bench\clock_cycles_tests.bat build\chacha20-v256.exe results\v256.csv 100
setlocal enabledelayedexpansion

set bin=%~1
if "%bin%"=="" (
    echo Usage: %~nx0 ^<binary^> [csv_file] [runs]
    exit /b 1
)
set csv_file=%~2
if "%csv_file%"=="" set csv_file=generated_results.csv
set runs=%~3
if "%runs%"=="" set runs=100

set out_file=%TEMP%\chacha20_bench_%RANDOM%.txt

echo Test,Run,Clock Cycles> "%csv_file%"
for /L %%t in (1,1,5) do (
    for /L %%r in (1,1,%runs%) do (
        set clock_cycles=
        rem Write the output to a file first: putting a quoted path inside
        rem for /F's command makes cmd strip the wrong quotes.
        call "%bin%" --clock-ct %%t > "%out_file%"
        for /F "tokens=5" %%c in ('findstr /C:"Clock cycles per byte" "%out_file%"') do set clock_cycles=%%c
        if "!clock_cycles!"=="" echo Warning: no clock cycles for test %%t, run %%r
        echo %%t,%%r,!clock_cycles!>> "%csv_file%"
    )
    echo Test %%t: %runs% runs done
)
del "%out_file%" 2>nul
echo Results written to %csv_file%
endlocal
