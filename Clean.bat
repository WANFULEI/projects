@echo Off
del /s /a *.lastbuildstate *.log *.exe.intermediate.manifest *.dll.intermediate.manifest *.obj vc100.pdb *.tlog *.vcxprojResolveAssemblyReference.cache *.ipch *.sdf 2>nul
FOR /R . %%d IN (.) DO rd /s /q "%%d\GeneratedFiles" 2>nul

rem del /s /a *.ncb *.sdf *.ipch *.netmodule *.aps *.ilk *.log *.obj *.tlog *.manifest *.lastbuildstate vc100.pdb 2>nul
rem FOR /R . %%d IN (.) DO rd /s /q "%%d\x64" 2>nul
FOR /R . %%d IN (.) DO rd /s /q "%%d\Debug" 2>nul
FOR /R . %%d IN (.) DO rd /s /q "%%d\Release" 2>nul
FOR /R . %%d IN (.) DO rd /s /q "%%d\ipch" 2>nul
rem FOR /R . %%d IN (.) DO rd /s /q "%%d\Bin" 2>nul
rem FOR /R . %%d IN (.) DO rd /s /q "%%d\Obj" 2>nul
rem If the Properties directory is empty, remove it
rem FOR /R . %%d in (.) do rd /q "%%d\Properties" 2> nul
