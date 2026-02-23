// Copyright Alien Shores


#include "UI/Widget/LevelSelect/MapEntryWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UMapEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (MapButton)
	{
		MapButton->OnClicked.AddDynamic(this, &UMapEntryWidget::OnClicked);
	}
}

void UMapEntryWidget::InitializeEntry(const FString& InMapName, const FString& InMapPath)
{
	MapPath = InMapPath;
	if (MapNameText)
	{
		FString CleanMapPath = InMapPath;
		CleanMapPath.ReplaceInline(TEXT("/Game/Maps/"), TEXT(""));
		MapNameText->SetText(FText::FromString(CleanMapPath));
	}
}

void UMapEntryWidget::OnClicked()
{
	// This fires PreLoadMap → your loading screen appears automatically
	UGameplayStatics::OpenLevel(this, FName(*MapPath));
}
