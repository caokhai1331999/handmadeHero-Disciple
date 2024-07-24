@echo off
xcopy C:\"Users\klove\Desktop\Temporary folder\C++ Practice\First Window app" E:\"First Window app" /Y /S
REM xcopy C:\"Users\klove\Downloads\SFML" E:\"SFML" /D /Y /S
REM xcopy C:\"Users\klove\Downloads\SDL2" E:\"SDL2" /D /Y /S
xcopy C:\"Users\klove\Documents\repos" E:\"repos\" /D /Y /S
xcopy C:\Users\klove\AppData\Roaming\.ema* E:\"First Window app" /D /Y
sh.exe ./backupOnline.sh
