# Build script for Tactility in WSL
# Run this manually in PowerShell to avoid interruptions

Write-Host "Building Tactility firmware in WSL..." -ForegroundColor Green
wsl bash -c "cd /mnt/c/Users/cscot/OneDrive/Documents/GitHub/Tactility && source ~/esp/esp-idf/export.sh && idf.py build"

if ($LASTEXITCODE -eq 0) {
    Write-Host "`nBuild completed successfully!" -ForegroundColor Green
    Write-Host "Firmware is in build/firmware.bin" -ForegroundColor Cyan
} else {
    Write-Host "`nBuild failed with exit code $LASTEXITCODE" -ForegroundColor Red
}
