@echo off
..\bjam toolset=msvc %*

IF NOT ERRORLEVEL 1 (
    call ..\boost-version.cmd
    for /r %BUILD_DIRECTORY% %%f in (*.pdb) do xcopy /D /Y %%f ..\dist\bin
)

IF NOT ERRORLEVEL 1 (
    ..\dist\bin\ftest ..\dist\bin\fost-jsondb-test-smoke.dll ..\dist\bin\fost-schema-test-smoke.dll
)
