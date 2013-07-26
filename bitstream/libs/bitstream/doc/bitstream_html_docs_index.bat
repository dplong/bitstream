echo off
rem Runs bjam jamfile.v2 and pipes the output to named log file,
rem including the date and time, so that each log file has a unique name,
rem for example: checks_23-Mar-2010_1751_html.log
rem The DOS time format is assumed 12:34 and the : separator is not used.
set t=%time% /T
set tim=%t:~0,2%%t:~3,2%
rem pick just hours and minutes.
rem time may include leading space, like " 915", so remove space.
set tim=%tim: =%
I:
cd boost-sandbox\SOC\2011\checks\libs\checks\doc
echo on
bjam html --enable-index > Checks_html_docs_index_%date%_%tim%.log
if not ERRORLEVEL 0 (echo Errorlevel is %ERRORLEVEL%) else (echo OK)
pause