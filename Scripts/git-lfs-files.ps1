# 1. Get all file names currently tracked by LFS
$LFSNames = git lfs ls-files --name-only

# 2. Define your explicit protection list (Metahumans, etc.)
$ExplicitExclude = @(
    "Content/Assets/Characters/Metahuman/MHC_Barbarian.uasset",
    "Content/Assets/Characters/Metahuman/MHC_Egyptian.uasset",
    "Content/Assets/Characters/Metahuman/MHC_Futureman.uasset",
    "Content/Assets/Characters/Metahuman/MHC_Highlander.uasset",
    "Content/Assets/Characters/Metahuman/MHC_Hippie.uasset",
    "Content/Assets/Characters/Metahuman/MHC_Knight.uasset",
    "Content/Assets/Characters/Metahuman/MHC_Native.uasset",
    "Content/Assets/Characters/Metahuman/MHC_Roman.uasset"
)

Write-Host "Analyzing $($LFSNames.Count) files for size and exclusions..." -ForegroundColor Cyan

$TargetObjects = @()

foreach ($name in $LFSNames) {
    # Ensure we use the absolute path for Test-Path and Get-Item
    $absolutePath = Join-Path (Get-Location).Path $name
    $relativeGitPath = $name.Replace('\', '/')

    if (Test-Path -LiteralPath $absolutePath) {
        $file = Get-Item -LiteralPath $absolutePath
        
        # Check if file should be kept in LFS
        $isExplicitlyExcluded = $ExplicitExclude -contains $relativeGitPath
        $isTooLarge = $file.Length -ge 25MB

        if (-not $isExplicitlyExcluded -and -not $isTooLarge) {
            $TargetObjects += [PSCustomObject]@{
                Path = $relativeGitPath
                Size = $file.Length
            }
        }
        elseif ($isTooLarge) {
            Write-Host "Keeping in LFS (>= 50MB): $relativeGitPath ($([Math]::Round($file.Length/1MB, 2)) MB)" -ForegroundColor Yellow
        }
    } else {
        # Fallback: if file isn't on disk (common in some migration states), 
        # assume it's small and should be migrated unless explicitly excluded
        if ($ExplicitExclude -notcontains $relativeGitPath) {
             $TargetObjects += [PSCustomObject]@{
                Path = $relativeGitPath
                Size = 0
            }
        }
    }
}

# 3. Sort by Size (Ascending is the default)
$TargetObjects = $TargetObjects | Sort-Object Size

# 4. Save the paths to the migration target file
if ($TargetObjects.Count -gt 0) {
    $TargetObjects.Path | Out-File -FilePath "migrate_targets.txt" -Encoding utf8
    
    Write-Host "`nSuccess!" -ForegroundColor Green
    Write-Host "Total files to migrate: $($TargetObjects.Count)"
    Write-Host "Smallest file in list: $([Math]::Round($TargetObjects[0].Size/1KB, 2)) KB"
    Write-Host "Largest file in list:  $([Math]::Round($TargetObjects[-1].Size/1MB, 2)) MB"
} else {
    Write-Host "`nNo files found to migrate! Check your ExcludeList or file sizes." -ForegroundColor Red
}