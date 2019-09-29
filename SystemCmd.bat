cd %~dp0
psexec -i -s cmd.exe /k "cd %~dp0\x64\Debug & whoami & HardwareScanner.exe & exit"
pause