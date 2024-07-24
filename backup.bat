@echo off
xcopy C:\"Users\klove\Desktop\Temporary folder\C++ Practice\First Window app" E:\"First Window app" /Y /S
REM xcopy C:\"Users\klove\Downloads\SFML" E:\"SFML" /Y /S
REM xcopy C:\"Users\klove\Downloads\SDL2" E:\"SDL2" /Y /S
xcopy C:\"Users\klove\Documents\repos" E:\"repos\*.bat" /Y /S
xcopy C:\Users\klove\AppData\Roaming\.ema* E:\"First Window app" /Y
sh.exe ./backupOnline.sh
