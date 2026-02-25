// Copyright Alien Shores


#include "Cinematic/CinematicMetaDataWidget.h"
#include "ContentBrowserModule.h"
#include "Cinematic/CinematicMetaDataLibrary.h"
#include "Cinematic/CinematicMetaDataUtility.h"
#include "Cinematic/Metadata/CinematicSequenceMetaData.h"

void UCinematicMetaDataWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FContentBrowserModule& CBModule =
		FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	CBModule.GetOnAssetSelectionChanged().AddUObject(
		this, &UCinematicMetaDataWidget::HandleContentBrowserSelectionChanged);
}

void UCinematicMetaDataWidget::NativeDestruct()
{
	Super::NativeDestruct();

	FContentBrowserModule& CBModule =
		FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	CBModule.GetOnAssetSelectionChanged().RemoveAll(this);
}

void UCinematicMetaDataWidget::MarkMetaDataDirty_Implementation()
{
	bIsDirty = true;
}

void UCinematicMetaDataWidget::SaveCurrent_Implementation()
{
	if (CurrentLevelSequence && CurrentMetaData && bIsDirty)
	{
		UCinematicMetaDataLibrary::ApplyMetaDataToSelected(CurrentLevelSequence, CurrentMetaData);
	}
	bIsDirty = false;
}

UCinematicSequenceMetaData* UCinematicMetaDataWidget::GetOrCreateSequenceMetaData_Implementation()
{
	if (!CurrentLevelSequence)
	{
		return nullptr;
	}
	if (UCinematicSequenceMetaData* Metadata = CurrentLevelSequence->FindMetaData<UCinematicSequenceMetaData>())
	{
		return Metadata;
	}
	return NewObject<UCinematicSequenceMetaData>(CurrentLevelSequence);
}

void UCinematicMetaDataWidget::HandleContentBrowserSelectionChanged(
	const TArray<FAssetData>& NewSelectedAssets, bool bIsPrimaryBrowser)
{
	if (!bIsPrimaryBrowser) { return; }

	TArray<ULevelSequence*> LoadedAssets;
	for (const FAssetData& AssetData : NewSelectedAssets)
	{
		if (ULevelSequence* Asset = Cast<ULevelSequence>(AssetData.GetAsset()))
		{
			LoadedAssets.Add(Asset);
		}
	}

	if (!CurrentLevelSequence || CurrentLevelSequence && !LoadedAssets.Contains(CurrentLevelSequence))
	{
		// Selection has changed!
		// first, save the current values
		if (CurrentLevelSequence && CurrentMetaData)
		{
			SaveCurrent();
		}
		// then, determine the new selection
		if (LoadedAssets.Num() > 0)
		{
			CurrentLevelSequence = LoadedAssets[0];
			CurrentMetaData = GetOrCreateSequenceMetaData();
			RefreshFromSelection(CurrentLevelSequence, CurrentMetaData);
		}
		else
		{
			ShowEmptyOrInvalidSelection();
		}
	}
}
