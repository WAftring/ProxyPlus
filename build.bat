@echo off
if [%1] == [] (
	@echo To compile this application with debug run "build.bat debug"
	@echo To compile this application run "build.bat production"
	exit /B 1
)
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
md build
cd build
del * /Q
if %1==debug (
	cl /Zi /FC /EHsc /I..\src /MD ..\src\*.c /link kernel32.lib user32.lib advapi32.lib /out:"ProxyPlus.exe"
	cd ..
	copy example.ini build\config.ini
)
if %1==production (
	cl /FC /EHsc /I..\src /MD ..\src\*.c /link kernel32.lib user32.lib advapi32.lib /out:"ProxyPlus.exe"
)
