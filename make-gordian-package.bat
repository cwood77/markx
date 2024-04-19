@echo off

if "%1" == "" goto usage

echo ** this script requires 'gordian' in path **
echo ** and ADMINISTRATOR permissions...       **
echo ** bail now if that isn't true            **
pause

rmdir /S /Q bin\out\markx-%1-win64-rel >nul 2>&1
mkdir bin\out\markx-%1-win64-rel
copy bin\out\release\console.dll bin\out\markx-%1-win64-rel\.
copy bin\out\release\file.dll bin\out\markx-%1-win64-rel\.
copy bin\out\release\lang_html.dll bin\out\markx-%1-win64-rel\.
copy bin\out\release\lang_markdown.dll bin\out\markx-%1-win64-rel\.
copy bin\out\release\markx.exe bin\out\markx-%1-win64-rel\.
copy bin\out\release\model.dll bin\out\markx-%1-win64-rel\.
copy bin\out\release\pass.dll bin\out\markx-%1-win64-rel\.
copy bin\out\release\pass_lib.dll bin\out\markx-%1-win64-rel\.
copy bin\out\release\tcatbin.dll bin\out\markx-%1-win64-rel\.
gordian --pack bin\out\markx-%1-win64-rel

mkdir C:\ProgramData\cdwe\gordian\freewebstore\packages
mkdir C:\ProgramData\cdwe\gordian\freewebstore\cat\m
echo # vi: ft=json>C:\ProgramData\cdwe\gordian\freewebstore\cat\m\markx-%1-win64-rel.sst
echo {>>C:\ProgramData\cdwe\gordian\freewebstore\cat\m\markx-%1-win64-rel.sst
echo    "name": "markx",>>C:\ProgramData\cdwe\gordian\freewebstore\cat\m\markx-%1-win64-rel.sst
echo    "version": %1,>>C:\ProgramData\cdwe\gordian\freewebstore\cat\m\markx-%1-win64-rel.sst
echo }>>C:\ProgramData\cdwe\gordian\freewebstore\cat\m\markx-%1-win64-rel.sst

copy bin\out\markx-%1-win64-rel.ar.z.s C:\ProgramData\cdwe\gordian\freewebstore\packages\.

gordian --store add markx-%1-win64-rel
rmdir /S /Q bin\out\markx-%1-win64-rel >nul 2>&1
echo ************************************************************
echo * your turn to upload the HTML, catalog, and split package *
echo ************************************************************
goto end

:usage
echo usage: make-gordian-package {version}
echo    note that gordian needs to be in the PATH

:end
