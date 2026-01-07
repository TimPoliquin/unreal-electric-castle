// Copyright Alien Shores


#include "UI/Widget/Status/StatusEffectWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
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
		Image->SetBrush(UWidgetBlueprintLibrary::MakeBrushFromMaterial(ProgressMaterialDynamic));
	}
	return ProgressMaterialDynamic;
}
