@echo off
echo fost-beanbag
@pushd %0\..

..\bjam %*

@IF ERRORLEVEL 1 (
    @echo fost-beanbag %* BUILD FAIL
    @popd
    @copy
) ELSE (
    @popd
)
