// Copyright Alien Shores


#include "UI/Widget/Status/StatusEffectWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Character/Status/StatusEffectConfig.h"
#include "Components/Image.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"

void UStatusEffectWidget::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (UGameplayStatics::IsGamePaused(GetOwningPlayer()))
	{
		// do not tick when paused
		return;
	}
	if (Duration > 0 && Runtime <= Duration && IsValid(ProgressMaterialDynamic))
	{
		Runtime += InDeltaTime;
		ProgressMaterialDynamic->SetScalarParameterValue(MaterialProgressParameter, FMath::Lerp(1.f, 0.f, Runtime/Duration));
	}
}

FGameplayTag UStatusEffectWidget::GetStatusEffectTag() const
{
	return StatusEffectTag;
}

void UStatusEffectWidget::SetStatusEffectTag(const FGameplayTag& InStatusEffectTag)
{
	StatusEffectTag = InStatusEffectTag;
	const UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(GetWorld());
	const UStatusEffectConfig* StatusEffectConfig = GameDataSubsystem ? GameDataSubsystem->GetStatusEffectConfig() : nullptr;
	if (!StatusEffectConfig)
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] Could not get status effect config"), *GetName());
		return;
	}
	if (FStatusEffectRow StatusEffectRow; StatusEffectConfig->FindStatusEffectByTag(InStatusEffectTag, StatusEffectRow))
	{
		SetIcon(UWidgetBlueprintLibrary::MakeBrushFromTexture(StatusEffectRow.Icon));
		SetBackgroundColor(StatusEffectRow.EffectColor);
	}
}

void UStatusEffectWidget::SetDuration(const float InDuration)
{
	Duration = InDuration;
}

UMaterialInstanceDynamic* UStatusEffectWidget::CreateDynamicMaterial(UImage* Image)
{
	if (!Image)
	{
		return nullptr;
	}
	if (UMaterialInterface* MaterialInterface = UWidgetBlueprintLibrary::GetBrushResourceAsMaterial(Image->GetBrush()))
	{
		ProgressMaterialDynamic = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetOwningPlayer(), MaterialInterface);
		ProgressMaterialDynamic->SetScalarParameterValue(MaterialProgressParameter, 1.f);
		ProgressMaterialDynamic->SetVectorParameterValue(MaterialColorParameter, BackgroundColor);
		Image->SetBrush(UWidgetBlueprintLibrary::MakeBrushFromMaterial(ProgressMaterialDynamic));
	}
	return ProgressMaterialDynamic;
}

void UStatusEffectWidget::SetBackgroundColor_Implementation(const FLinearColor InBackgroundColor)
{
	BackgroundColor = InBackgroundColor;
	if (ProgressMaterialDynamic)
	{
		ProgressMaterialDynamic->SetVectorParameterValue(MaterialColorParameter, InBackgroundColor);
	}
}
