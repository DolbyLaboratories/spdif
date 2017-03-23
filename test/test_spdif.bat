::--------------------------------------------------------------------------------------------------------------
:: Copyright (c) 2017, Dolby Laboratories Inc.
:: All rights reserved.

:: Redistribution and use in source and binary forms, with or without modification, are permitted
:: provided that the following conditions are met:

:: 1. Redistributions of source code must retain the above copyright notice, this list of conditions
::    and the following disclaimer.
:: 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
::    and the following disclaimer in the documentation and/or other materials provided with the distribution.
:: 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or
::    promote products derived from this software without specific prior written permission.

:: THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
:: WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
:: PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
:: ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
:: PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
:: HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
:: (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
:: OF THE POSSIBILITY OF SUCH DAMAGE.
::--------------------------------------------------------------------------------------------------------------
@echo off
::  Usage: test_spdif [binary directory]
::  NOTE: This must be run in the test directory
::  test_spdif.bat will look for the build binary in the directory specified
::  on the command line or, by default, it will look in ..\Release\Win32\bin\
::  This batch script is only intended to be used under Windows and was tested
::  using Windows 7

SetLocal EnableDelayedExpansion
if ERRORLEVEL 1 goto delayederror

if "%1" == "/?" goto usage

if "%1" == "" (
	set BINDIR=..\Release\Win32
) else (
	set BINDIR=..\%1
)

cd %~d0%~p0

if "%2" == "coverage" (
	set DP_COVERAGE_SESSION_FILENAME=sum.dpcov
	set DP_COVERAGE_MERGE_FILENAME=sum.dpmrg
)

if not exist input\ goto direrror

if not exist output\ mkdir output
if not exist output\ref\ mkdir output\ref

if not exist %BINDIR%\spdif.exe goto direrror

set BIN=%cd%\%BINDIR%\spdif.exe
set REF=%cd%\ref\spdif.exe
set LOG=%cd%\output\test_spdif.log
set IN=%cd%\input
set OUT=%cd%\output
set OUTREF=%cd%\output\ref

:: clean outputs
if exist %OUT%\*.sdf del %OUT%\*.sdf
if exist %OUT%\*.ac3 del %OUT%\*.ac3
if exist %OUT%\*.ddp del %OUT%\*.ddp
if exist %LOG% del %LOG%

set fail=0

echo Testing AC3 inputs...
echo Testing AC3 inputs... >> %LOG%

for %%c in (%IN%\*.ac3) do (
	%BIN% -i%%c -o%OUT%\%%~nc.sdf >> %LOG% 2>>&1
	echo %BIN% -i%%c -o%OUT%\%%~nc.sdf >> %LOG%

	%BIN% -d -i%OUT%\%%~nc.sdf -o%OUT%\%%~nc_out.ac3 >> %LOG% 2>>&1
	echo %BIN% -d -i%OUT%\%%~nc.sdf -o%OUT%\%%~nc_out.ac3 >> %LOG%

	if not exist %OUTREF%\%%~nc.sdf (
		%REF% -i%%c -o%OUTREF%\%%~nc.sdf >> %LOG% 2>>&1
		echo %REF% -i%%c -o%OUTREF%\%%~nc.sdf >> %LOG% 2>>&1
	)

	fc /B %OUTREF%\%%~nc.ac3 %OUT%\%%~nc_out.ac3  
	echo fc /B %OUTREF%\%%~nc.ac3 %OUT%\%%~nc_out.ac3  >> %LOG%
	if ERRORLEVEL 1 (
		set fail=1
		echo %%~nc [ident]	[FAILED]
		echo %%~nc [ident]	[FAILED] >> %LOG%
	) else (
		echo %%~nc [ident]	[PASSED]
		echo %%~nc [ident]	[PASSED] >> %LOG%
	)

	fc /B %OUTREF%\%%~nc.sdf %OUT%\%%~nc.sdf  
	echo fc /B %OUTREF%\%%~nc.sdf %OUT%\%%~nc.sdf  >> %LOG%
	if ERRORLEVEL 1 (
		set fail=1
		echo %%~nc [ref]	[FAILED]
		echo %%~nc [ref]	[FAILED] >> %LOG%
	) else (
		echo %%~nc [ref]	[PASSED]
		echo %%~nc [ref]	[PASSED] >> %LOG%
	)
)

:alternative

echo. 
echo Testing alternative format
for %%c in (%IN%\classical*.ac3) do (
	%BIN% -a -i%%c -o%OUT%\%%~nc_alt.sdf >> %LOG% 2>>&1
	echo %BIN% -a -i%%c -o%OUT%\%%~nc_alt.sdf >> %LOG%

	%BIN% -a -d -i%OUT%\%%~nc_alt.sdf -o%OUT%\%%~nc_out_alt.ac3 >> %LOG% 2>>&1
	echo %BIN% -a -d -i%OUT%\%%~nc_alt.sdf -o%OUT%\%%~nc_out_alt.ac3 >> %LOG%

	if not exist %OUTREF%\%%~nc_alt.sdf (
		%REF% -i%%c -a -o%OUTREF%\%%~nc_alt.sdf >> %LOG% 2>>&1
		echo %REF% -a -i%%c -o%OUTREF%\%%~nc_alt.sdf >> %LOG% 2>>&1
	)
	fc /B %OUTREF%\%%~nc_alt.sdf %OUT%\%%~nc_alt.sdf  
	echo fc /B %OUTREF%\%%~nc.sdf %OUT%\%%~nc.sdf  >> %LOG%
	if ERRORLEVEL 1 (
		set fail=1
		echo %%~nc [ref]	[FAILED]
		echo %%~nc [ref]	[FAILED] >> %LOG%
	) else (
		echo %%~nc [ref]	[PASSED]
		echo %%~nc [ref]	[PASSED] >> %LOG%
	)
)

echo.
echo Testing DDP inputs...
echo Testing DDP inputs... >> %LOG%

for %%c in (%IN%\*.ec3) do (
	%BIN% -i%%c -o%OUT%\%%~nc.sdf >> %LOG% 2>>&1
	echo %BIN% -i%%c -o%OUT%\%%~nc.sdf >> %LOG%

	%BIN% -d -i%OUT%\%%~nc.sdf -o%OUT%\%%~nc_out.ec3 >> %LOG% 2>>&1
	echo %BIN% -d -i%OUT%\%%~nc.sdf -o%OUT%\%%~nc_out.ec3 >> %LOG%

	if not exist %OUTREF%\%%~nc.sdf (
		%REF% -i%%c -o%OUTREF%\%%~nc.sdf >> %LOG% 2>>&1
		echo %REF% -i%%c -o%OUTREF%\%%~nc.sdf >> %LOG% 2>>&1 
	)

	fc /B %OUTREF%\%%~nc.ec3 %OUT%\%%~nc_out.ec3  
	echo fc /B %OUTREF%\%%~nc.ec3 %OUT%\%%~nc_out.ec3  >> %LOG%
	if ERRORLEVEL 1 (
		set fail=1
		echo %%~nc [ident]	[FAILED]
		echo %%~nc [ident]	[FAILED] >> %LOG%
	) else (
		echo %%~nc [ident]	[PASSED]
		echo %%~nc [ident]	[PASSED] >> %LOG%
	)

	fc /B %OUTREF%\%%~nc.sdf %OUT%\%%~nc.sdf  
	echo fc /B %OUTREF%\%%~nc.sdf %OUT%\%%~nc.sdf  >> %LOG%
	if ERRORLEVEL 1 (
		set fail=1
		echo %%~nc [ref]	[FAILED]
		echo %%~nc [ref]	[FAILED] >> %LOG%
	) else (
		echo %%~nc [ref]	[PASSED]
		echo %%~nc [ref]	[PASSED] >> %LOG%
	)

)

:errorhandling

echo.
echo Testing errorhandling...

%BIN% -i%IN%\error\classical_14_a7_invsync.ac3 -o%OUT%\errortest.sdf >> %LOG% 2>>&1
echo %BIN% -i%IN%\error\classical_14_a7_invsync.ac3 -o%OUT%\errortest.sdf >> %LOG%
if not errorlevel 1 (
	set fail=1
	echo Invalid Sync Word  [FAILED]
	echo Invalid Sync Word  [FAILED] >> %LOG%
) else (
	echo Invalid Sync Word  [PASSED]
	echo Invalid Sync Word  [PASSED] >> %LOG%
)

%BIN% -i%IN%\error\classical_766_a7.ec3 -o%OUT%\errortest.sdf >> %LOG% 2>>&1
echo %BIN% -i%IN%\error\classical_766_a7.ec3 -o%OUT%\errortest.sdf >> %LOG%
if errorlevel 1 (
	set fail=1
	echo Boundary case for large frame size [1532]  [FAILED]
	echo Boundary case for large frame size [1532]  [FAILED] >> %LOG%
) else (
	echo Boundary case for large frame size [1532]  [PASSED]
	echo Boundary case for large frame size [1532]  [PASSED] >> %LOG%
)


%BIN% -i%IN%\error\errtesterr.ec3 -o%OUT%\errortest.sdf >> %LOG% 2>>&1
echo %BIN% -i%IN%\error\errtesterr.ec3 -o%OUT%\errortest.sdf >> %LOG%
if not errorlevel 1 (
	set fail=1
	echo file not found  [FAILED]
	echo file not found  [FAILED] >> %LOG%
) else (
	echo file not found  [PASSED]
	echo file not found  [PASSED] >> %LOG%
)


%BIN% -i%IN%\error\empty.ec3 -o%OUT%\errortest.sdf >> %LOG% 2>>&1
echo %BIN% -i%IN%\error\empty.ec3 -o%OUT%\errortest.sdf >> %LOG%
if not errorlevel 1 (
	set fail=1
	echo empty file  [FAILED]
	echo empty file  [FAILED] >> %LOG%
) else (
	echo empty file  [PASSED]
	echo empty file  [PASSED] >> %LOG%
)

%BIN% -a -i%IN%\female_256_a2.ec3 -o%OUT%\errortest.sdf >> %LOG% 2>>&1
echo %BIN% -a -i%IN%\female_256_a2.ec3 -o%OUT%\errortest.sdf >> %LOG% 2>>&1
if not errorlevel 1 (
	set fail=1
	echo invalid alternative  [FAILED]
	echo invalid alternative  [FAILED] >> %LOG%
) else (
	echo invalid alternative  [PASSED]
	echo invalid alternative  [PASSED] >> %LOG%
)


%BIN% -i%IN%\error\classical_10_a2_invfrcode.ac3 -o%OUT%\errortest.sdf >> %LOG% 2>>&1
echo %BIN% -i%IN%\error\classical_10_a2_invfrcode.ac3 -o%OUT%\errortest.sdf >> %LOG% 2>>&1
if not errorlevel 1 (
	set fail=1
	echo invalid frame code  [FAILED]
	echo invalid frame code  [FAILED] >> %LOG%
) else (
	echo invalid frame code  [PASSED]
	echo invalid frame code  [PASSED] >> %LOG%
)


%BIN% -i%IN%\error\classical_10_a2_invsamprate.ac3 -o%OUT%\errortest.sdf >> %LOG% 2>>&1
echo %BIN% -i%IN%\error\classical_10_a2_invsamprate.ac3 -o%OUT%\errortest.sdf >> %LOG% 2>>&1
if not errorlevel 1 (
	set fail=1
	echo invalid sample rate  [FAILED]
	echo invalid sample rate  [FAILED] >> %LOG%
) else (
	echo invalid sample rate  [PASSED]
	echo invalid sample rate  [PASSED] >> %LOG%
)


%BIN% -i%IN%\error\classical_16_32_a7_invbsid.ac3 -o%OUT%\errortest.sdf >> %LOG% 2>>&1
echo %BIN% -i%IN%\error\classical_16_32_a7_invbsid.ac3 -o%OUT%\errortest.sdf >> %LOG% 2>>&1
if not errorlevel 1 (
	set fail=1
	echo invalid bsid  [FAILED]
	echo invalid bsid  [FAILED] >> %LOG%
) else (
	echo invalid bsid  [PASSED]
	echo invalid bsid  [PASSED] >> %LOG%
)




if %fail% == 1 (
	echo.
	echo ========================== 
	echo Spdif FAILED TEST 
	echo ========================== 
	echo.
	echo Binary location: %BIN% 
	echo Results written to %OUT%\test_spdif.log 
	exit /b 1
)

if %fail% == 0 (
	echo.
	echo ========================== 
	echo Spdif binary PASSED TEST 
	echo ========================== 
	echo.
	echo Binary location: %BIN%
	echo Results written to %OUT%\test_spdif.log 
	exit /b 0
)

:direrror

echo.
echo ===========================================================
echo ERROR: test_spdif must be run in the root test directory.
echo ===========================================================
echo. 

exit /b 1

:delayederror
echo.
echo ERROR: test_spdif requires at least Windows 7
echo.

exit /b 1

:usage

echo.
echo Usage: test_spdif [binary directory]
echo NOTE: This must be run in the test directory
echo test_spdif.bat will look for the build binary in the directory specified
echo on the command line or, by default, it will look in ..\Release\Win32\bin\
echo This batch script is only intended to be used under Windows and was tested
echo using Windows 7
echo.

exit /b 1

:end
