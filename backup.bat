@echo off
xcopy C:\"Users\klove\Desktop\Temporary folder\C++ Practice\First Window app" E:\"First Window app" /D /Y /Sxcopy C:\"Users\klove\Downloads\External_Libraries" E:\"External_Libraries\" /D /Y /S
xcopy C:\"Users\klove\Documents\repos" E:\"repos\" /D /Y /S
xcopy C:\Users\klove\AppData\Roaming\.ema* E:\"First Window app" /D /Y /S
sh.exe ./backupOnline.sh
