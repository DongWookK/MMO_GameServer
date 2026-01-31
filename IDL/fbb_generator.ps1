param(
    [string]$Arg1,
    [string]$Arg2
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# === Path 설정 ===
$BaseDir    = Split-Path -Parent $MyInvocation.MyCommand.Path
$ExtDir     = "Common\"
$ProjectDir = "cpp\"
$TempDir    = "${Arg2}cpp_temp\"
$IDLDir     = "..\$ExtDir`idl_cpp\"

$DeleteFile = 0
$UpdateFile = 0
$NewFile    = 0
$errorValue = 0

$TempFull    = Join-Path $BaseDir $TempDir
$ProjectFull = Join-Path $BaseDir $ProjectDir
$IDLFull     = Join-Path $BaseDir $IDLDir

# === Temp 삭제 ===
if (Test-Path $TempFull) {
    Remove-Item $TempFull -Recurse -Force
}

# === flatc 실행 ===
$flatc = Join-Path $BaseDir "flatc.exe"
$flatcArgs = @(
    "-c",
    "--cpp-std", "c++17",
    "--gen-object-api",
    "--gen-compare",
    "--natural-utf8",
    "-o", $TempFull
)

Get-ChildItem -Path $BaseDir -Recurse -Filter *.fbs | ForEach-Object {
    try {
        & $flatc @flatcArgs $_.FullName
    }
    catch {
        Write-Host "================================================================"
        Write-Host "error occurred while processing '$($_.FullName)' file."
        Write-Host
        $errorValue = 1
    }
}

if ($errorValue -ne 0) {
    Write-Host "`n`n`nfailed to generate file.`n`n"

    if ($Arg1 -eq "msbuild") {
        exit $errorValue
    }

    Read-Host "Press Enter to continue"
    return
}

# === STEP : delete removed files ===
Write-Host "================================================================"
Write-Host "STEP : delete removed files."
Write-Host "================================================================"

Get-ChildItem $ProjectFull -Recurse -File | ForEach-Object {
    $tempFile = Join-Path $TempFull $_.Name
    if (-not (Test-Path $tempFile)) {
        Remove-Item $_.FullName -Force
        Write-Host "$($_.FullName) deleted"
        $DeleteFile++
    }
}

Write-Host "`n`n"

# === STEP : apply updated files ===
Write-Host "================================================================"
Write-Host "STEP : apply updated files."
Write-Host "================================================================"

Get-ChildItem $ProjectFull -Recurse -File | ForEach-Object {
    $tempFile = Join-Path $TempFull $_.Name
    if (Test-Path $tempFile) {
        if (-not (Compare-Object (Get-Content $_.FullName) (Get-Content $tempFile))) {
            return
        }
        Copy-Item $tempFile $_.FullName -Force
        Write-Host "$($_.FullName) updated"
        $UpdateFile++
    }
}

Write-Host "`n`n"

# === STEP : add new files ===
Write-Host "================================================================"
Write-Host "STEP : add new files."
Write-Host "================================================================"

Get-ChildItem $TempFull -Recurse -File | ForEach-Object {
    $dst = Join-Path $ProjectFull $_.Name
    if (-not (Test-Path $dst)) {
        Copy-Item $_.FullName $ProjectFull -Force
        Write-Host "$($_.FullName) new file copied."
        $NewFile++
    }
}

Write-Host "`n`n"

# === STEP : remove temp directory ===
Write-Host "================================================================"
Write-Host "STEP : remove temp directory."
Write-Host "================================================================"

Remove-Item $TempFull -Recurse -Force

Write-Host "`n`n"

# === STEP : copy common directory ===
Write-Host "================================================================"
Write-Host "STEP : copy common directory."
Write-Host "================================================================"

if (Test-Path $IDLFull) {
    Remove-Item $IDLFull -Recurse -Force
}

Copy-Item "$ProjectFull*" $IDLFull -Recurse -Force

Write-Host "`n`n"

# === Summary ===
Write-Host "================================================================"
Write-Host "delete file : $DeleteFile"
Write-Host "update file : $UpdateFile"
Write-Host "New file    : $NewFile"
Write-Host "================================================================"
Write-Host "`n`n"

if ($Arg1 -eq "msbuild") {
    exit 0
}

Start-Sleep -Seconds 3

if ($Arg1 -eq "exit") {
    exit
}
