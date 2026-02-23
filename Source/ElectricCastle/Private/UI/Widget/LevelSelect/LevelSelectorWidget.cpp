// Copyright Alien Shores


#include "UI/Widget/LevelSelect/LevelSelectorWidget.h"

#include "Components/VerticalBox.h"
#include "Engine/Engine.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/Paths.h"
#include "UI/Widget/LevelSelect/MapEntryWidget.h"

void ULevelSelectorWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PopulateMapList();
}

void ULevelSelectorWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void ULevelSelectorWidget::PopulateMapList()
{
	if (!MapListContainer)
	{
		return;
	}

	if (!MapEntryWidgetClass)
	{
		return;
	}

	MapListContainer->ClearChildren();

	// Find all .umap files under the project's Content directory
	TArray<FString> MapFiles;
	const FString ContentDir = FPaths::ProjectContentDir();
	const FString MapsDir = ContentDir / TEXT("Maps");


	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.FindFilesRecursively(MapFiles, *MapsDir, TEXT(".umap"));

	// Sort alphabetically
	MapFiles.Sort();

	int32 EntriesAdded = 0;

	for (const FString& MapFilePath : MapFiles)
	{
		// Get just the filename without extension as the display name
		const FString MapName = FPaths::GetBaseFilename(MapFilePath);

		// Apply exclusion filter
		const bool bExcluded = ExcludedMapNames.ContainsByPredicate([&](const FString& Excluded)
		{
			return MapName.Contains(Excluded, ESearchCase::IgnoreCase);
		});

		if (bExcluded)
		{
			continue;
		}

		// Convert the absolute file path to a game package path that OpenLevel expects:
		// C:/Project/Content/Maps/MyLevel.umap → /Game/Maps/MyLevel
		FString PackagePath = MapFilePath;
		FPaths::MakePathRelativeTo(PackagePath, *ContentDir);
		PackagePath = FString(TEXT("/Game/")) + FPaths::GetPath(PackagePath) / MapName;

		// Normalize any double slashes that appear when the map is directly in Content/
		PackagePath = PackagePath.Replace(TEXT("//"), TEXT("/"));

		UMapEntryWidget* Entry = CreateWidget<UMapEntryWidget>(this, MapEntryWidgetClass);
		if (!Entry)
		{
			continue;
		}

		Entry->InitializeEntry(MapName, PackagePath);
		MapListContainer->AddChild(Entry);
		++EntriesAdded;
	}
}
