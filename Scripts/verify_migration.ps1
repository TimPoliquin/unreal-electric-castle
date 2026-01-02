# verify_safe.ps1
$SizeThresholdMB = 100
$thresholdBytes = $SizeThresholdMB * 1MB

Write-Host "--- Detailed Verification (Git-Aware) ---" -ForegroundColor Cyan

# 1. Get list of files Git LFS is currently tracking
$lfsFiles = & git lfs ls-files --name-only

# 2. Get ONLY tracked files (automatically obeys .gitignore) 
$trackedFiles = & git ls-files

$largeInLFS = @()
$largeNotInLFS = @()
$smallInGit = 0
$smallStillInLFS = @()

foreach ($gitPath in $trackedFiles) {
    if (-not (Test-Path -LiteralPath $gitPath)) { continue }
    
    $fileInfo = Get-Item -LiteralPath $gitPath
    $size = $fileInfo.Length
    $isInLFS = $lfsFiles -contains $gitPath

    if ($size -gt $thresholdBytes) {
        if ($isInLFS) {
            $largeInLFS += $gitPath
        } else {
            $largeNotInLFS += "$gitPath ($([Math]::Round($size / 1MB, 2)) MB)"
        }
    } else {
        # File is under 100MB
        if ($isInLFS) {
            $smallStillInLFS += "$gitPath ($([Math]::Round($size / 1KB, 2)) KB)"
        } else {
            $smallInGit++
        }
    }
}

Write-Host "`n--- FINAL REPORT ---" -ForegroundColor Cyan
Write-Host "1. Small Files Successfully in Git:  $smallInGit" -ForegroundColor Green
Write-Host "2. Large Files Safely in LFS (>100MB): $($largeInLFS.Count)" -ForegroundColor Green

if ($smallStillInLFS.Count -gt 0) {
    Write-Host "`n3. SMALL FILES STILL IN LFS (Migration Missed These): $($smallStillInLFS.Count)" -ForegroundColor Yellow
    # Uncomment the line below if you want to see the full list of small files in LFS
    # $smallStillInLFS | ForEach-Object { Write-Host "  [-] $_" }
}

if ($largeNotInLFS.Count -gt 0) {
    Write-Host "`n4. CRITICAL: LARGE FILES NOT IN LFS: $($largeNotInLFS.Count)" -ForegroundColor Red
    $largeNotInLFS | ForEach-Object { Write-Host "  [!] $_" }
} else {
    Write-Host "`n4. SUCCESS: No tracked files exceed the 100MB limit outside of LFS." -ForegroundColor Green
}