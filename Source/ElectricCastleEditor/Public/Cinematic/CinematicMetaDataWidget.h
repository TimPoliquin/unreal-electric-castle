// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "CinematicMetaDataWidget.generated.h"

class UCinematicSequenceMetaData;
class ULevelSequence;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLEEDITOR_API UCinematicMetaDataWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cinematic Metadata")
	void MarkMetaDataDirty();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cinematic Metadata")
	void SaveCurrent();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Cinematic Metadata")
	void RefreshFromSelection(ULevelSequence* LevelSequence, UCinematicSequenceMetaData* MetaData);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Cinematic Metadata")
	void ShowEmptyOrInvalidSelection();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cinematic Metadata")
	UCinematicSequenceMetaData* GetOrCreateSequenceMetaData();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cinematic Metadata")
	TObjectPtr<ULevelSequence> CurrentLevelSequence;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cinematic Metadata")
	TObjectPtr<UCinematicSequenceMetaData> CurrentMetaData;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Cinematic Metadata")
	bool bIsDirty = false;

private:
	void HandleContentBrowserSelectionChanged(
		const TArray<FAssetData>& NewSelectedAssets, bool bIsPrimaryBrowser);
};
