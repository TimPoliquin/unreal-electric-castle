// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelSelectorWidget.generated.h"

class UMapEntryWidget;
class UVerticalBox;
/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API ULevelSelectorWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> MapListContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Level Select")
	TSubclassOf<UMapEntryWidget> MapEntryWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Level Select")
	TArray<FString> ExcludedMapNames = {TEXT("LVL_Editor_Level_Select"), TEXT("Untitled"), TEXT("LVL_LoadingScreen")};

private:
	void PopulateMapList();

	FDelegateHandle OnFilesLoadedHandle; // Paths to exclude from the list (editor maps, dev maps, etc.)
};
