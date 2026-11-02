param([string]$KeilC51Root = 'D:/keil-C51/C51')
$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    $compilerRoot = $KeilC51Root
    & "$compilerRoot/BIN/C51.exe" main.c "INCDIR($compilerRoot/INC)" OBJECTEXTEND
    if ($LASTEXITCODE -ne 0) { throw 'main.c compilation failed' }
    & "$compilerRoot/BIN/C51.exe" game.c OBJECTEXTEND
    if ($LASTEXITCODE -ne 0) { throw 'game.c compilation failed' }
    & "$compilerRoot/BIN/C51.exe" display.c "INCDIR($compilerRoot/INC)" OBJECTEXTEND
    if ($LASTEXITCODE -ne 0) { throw 'display.c compilation failed' }
    & "$compilerRoot/BIN/BL51.exe" main.obj,game.obj,display.obj TO experiment1
    if ($LASTEXITCODE -ne 0) { throw 'Link failed' }
    & "$compilerRoot/BIN/OH51.exe" experiment1
    if ($LASTEXITCODE -ne 0) { throw 'HEX conversion failed' }
} finally { Pop-Location }
