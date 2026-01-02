# surgical_migrate.ps1
$BatchSize = 10
$AllFiles = Get-Content "migrate_targets.txt"
$Total = $AllFiles.Count

# Automatically detect the current branch name
$CurrentBranch = & git rev-parse --abbrev-ref HEAD

Write-Host "Starting surgical migration of $Total files on branch [$CurrentBranch]..." -ForegroundColor Cyan

for ($i = 0; $i -lt $Total; $i += $BatchSize) {
    $end = [Math]::Min($i + $BatchSize - 1, $Total - 1)
    $Batch = $AllFiles[$i..$end]
    $IncludeArg = $Batch -join ","

    Write-Host "`n--- Batch $([Math]::Floor($i / $BatchSize) + 1): Files $i to $end ---" -ForegroundColor White
    
    try {
        # 1. Run the migration
        & git lfs migrate export --include="$IncludeArg" --everything --yes

        # 2. Add changes and commit
        & git add .
        & git commit -m "Migrate batch $([Math]::Floor($i / $BatchSize) + 1) out of LFS" --allow-empty

        # 3. FORCE Push to overwrite the remote history with our rewritten version
        Write-Host "Force-pushing batch to GitHub..." -ForegroundColor Yellow
        & git push origin $CurrentBranch --force
        
        if ($LASTEXITCODE -ne 0) {
            throw "Git push failed with exit code $LASTEXITCODE"
        }

        Write-Host "Batch Complete." -ForegroundColor Green
    }
    catch {
        Write-Host "FATAL ERROR in batch at index $i : $($_.Exception.Message)" -ForegroundColor Red
        Write-Host "Script stopped. Please check your connection or GitHub status." -ForegroundColor Yellow
        break 
    }
}