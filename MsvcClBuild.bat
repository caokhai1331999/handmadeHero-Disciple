mkdir build
pushd build

del *.obj *.pdb

rem remember to add these to use address sanitizer /EHsc /fsanitize=address
@echo off

for %%f in (..\*.cpp) do (
    cl /FC /Zi -Fe:"win32Game" %%f -link user32.lib gdi32.lib
 )

if %ERRORLEVEL% EQU 0 (
   @echo Announce: " compilation succeeded (^ w ^) "
) else (
   @echo                                  @   @          
   @echo Announce: " compilation failed \_(-_-)_/ "
)

popd


rem mkdir build
rem pushd build

rem @echo off

rem for %%f in (..\*.cpp ..\*.h) do (
rem     cl /FC /Zi /EHsc /fsanitize=address -Fe:"win32Game" %%f -link user32.lib gdi32.lib
rem )

rem if %ERRORLEVEL% EQU 0 (
rem     @echo Compilation succeeded (^ w ^)
rem ) else (
rem     @echo                      
rem     @echo Compilation failed \_(-_-)_/
rem )

rem popd
