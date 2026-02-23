// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapEntryWidget.generated.h"

class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS(Abstract)
class ELECTRICCASTLE_API UMapEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeEntry(const FString& InMapName, const FString& InMapPath);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MapButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MapNameText;

private:
	UFUNCTION()
	void OnClicked();

	FString MapPath;
};
