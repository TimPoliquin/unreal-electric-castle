// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "LevelSequence.h"
#include "GameplayTagContainer.h"
#include "CinematicMetaDataUtility.generated.h"

class UEditorUtilityWidgetBlueprint;
class UCinematicSequenceMetadata;

UCLASS()
class ELECTRICCASTLEEDITOR_API UCinematicMetaDataUtility : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	// Prints the current metadata of all selected sequences to the Output Log.
	UFUNCTION(CallInEditor, Category = "Cinematic Metadata")
	void LogCinematicMetaData();

	// Removes the UCinematicSequenceMetaData object from all selected sequences.
	UFUNCTION(CallInEditor, Category = "Cinematic Metadata")
	void ClearCinematicMetaData();

	// Returns the metadata from the first selected Level Sequence, or nullptr if none exists.
	// Used by the Editor Utility Widget to pre-populate its fields.
	UFUNCTION(BlueprintCallable, Category = "Cinematic Metadata")
	UCinematicSequenceMetadata* GetMetaDataFromFirstSelected() const;

	UFUNCTION(CallInEditor, Category = "Cinematic Metadata")
	void OpenMetaDataEditor();

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	TSoftObjectPtr<UEditorUtilityWidgetBlueprint> EditorWidget;

private:
	// Returns only the ULevelSequence assets from the current Content Browser selection.
	TArray<ULevelSequence*> GetSelectedLevelSequences() const;
};
