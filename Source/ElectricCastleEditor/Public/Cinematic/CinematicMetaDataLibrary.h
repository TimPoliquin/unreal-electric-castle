// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CinematicMetaDataLibrary.generated.h"

class ULevelSequence;
enum class ECinematicSelectionState : uint8;
class UCinematicSequenceMetadata;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLEEDITOR_API UCinematicMetaDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Returns metadata from the first selected Level Sequence, or nullptr.
	UFUNCTION(BlueprintCallable, Category = "Cinematic Metadata")
	static UCinematicSequenceMetadata* GetMetaDataFromFirstSelected();

	// Writes the given metadata object's values to all selected Level Sequences.
	UFUNCTION(BlueprintCallable, Category = "Cinematic Metadata")
	static void ApplyMetaDataToSelected(ULevelSequence* LevelSequence, UCinematicSequenceMetadata* SourceMetaData);

	UFUNCTION(BlueprintCallable, Category = "Cinematic Metadata")
	static void CopyMetaData(UCinematicSequenceMetadata* SourceMetaData, UCinematicSequenceMetadata* TargetMetaData);

	UFUNCTION(BlueprintCallable, Category = "Cinematic Metadata")
	static ULevelSequence* GetFirstSelectedLevelSequence();
};
