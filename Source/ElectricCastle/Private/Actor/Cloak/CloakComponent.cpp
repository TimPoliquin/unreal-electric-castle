// Copyright Alien Shores


#include "Actor/Cloak/CloakComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Tags/ElectricCastleGameplayTags.h"
#include "Utils/PostProcessUtils.h"

// Sets default values for this component's properties
UCloakComponent::UCloakComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UCloakComponent::BeginPlay()
{
	Super::BeginPlay();
	if (UMaterialInstanceDynamic* DynamicMaterial = UPostProcessUtils::MakePostProcessMaterialDynamic(GetOwner(), StealthMaterial))
	{
		StealthMaterialDynamic = DynamicMaterial;
		StealthMaterialDynamic->SetScalarParameterValue(CloakMaterialStencilParameterName, CloakMaterialStencilCode);
		StealthMaterialDynamic->SetScalarParameterValue(CloakMaterialParameterName, 0.f);
	}
	else
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Failed to create dynamic post process material"), *GetOwner()->GetName(), *GetName());
	}
}

void UCloakComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!StealthMaterialDynamic)
	{
		return;
	}
	if (FadeDirection > ECloakFadeDirection::None && CloakMaterialChangeProgress < CloakMaterialChangeDuration)
	{
		CloakMaterialChangeProgress = FMath::Clamp(CloakMaterialChangeProgress + DeltaTime, 0,
		                                           CloakMaterialChangeDuration);
		const float Alpha = FMath::Clamp(
			FadeDirection == ECloakFadeDirection::FadeOut
				? CloakMaterialChangeProgress / CloakMaterialChangeDuration
				: 1 - CloakMaterialChangeProgress / CloakMaterialChangeDuration,
			CloakMaterialMinAlpha,
			1.f
		);
		StealthMaterialDynamic->SetScalarParameterValue(CloakMaterialParameterName, Alpha);
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Display, TEXT("[%s:%s] Setting material alpha: %s: %f"), *GetOwner()->GetName(), *GetName(), *CloakMaterialParameterName.ToString(), Alpha);
		}
	}
	else
	{
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Display, TEXT("[%s:%s] Disabling tick"), *GetOwner()->GetName(), *GetName());
		}
		SetComponentTickEnabled(false);
		if (FadeDirection == ECloakFadeDirection::FadeIn)
		{
			UpdateParentStencilValues(false);
		}
		FadeDirection = ECloakFadeDirection::None;
	}
}

void UCloakComponent::StartCloak()
{
	if (bIsCloaked)
	{
		return;
	}
	bIsCloaked = true;
	if (CloakEffect)
	{
		CloakEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffect(GetOwner(), CloakEffect);
	}
	else
	{
		CloakEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyInfiniteEffectByTag(
			GetOwner(), FElectricCastleGameplayTags::Get().Effect_State_Cloaked);
	}
	CloakMaterialChangeProgress = 0.f;
	FadeDirection = ECloakFadeDirection::FadeOut;
	UpdateParentStencilValues(true);
	SetComponentTickEnabled(true);
}

void UCloakComponent::EndCloak()
{
	if (!bIsCloaked)
	{
		return;
	}
	bIsCloaked = false;
	CloakMaterialChangeProgress = 0.f;
	FadeDirection = ECloakFadeDirection::FadeIn;
	UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(GetOwner(), CloakEffectHandle, false, true);
	SetComponentTickEnabled(true);
}

void UCloakComponent::UpdateParentStencilValues(const bool bInEnableCustomDepth)
{
	TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents;
	GetOwner()->GetComponents(PrimitiveComponents);
	for (UPrimitiveComponent* Prim : PrimitiveComponents)
	{
		Prim->SetRenderCustomDepth(bInEnableCustomDepth);
		Prim->SetCustomDepthStencilValue(bInEnableCustomDepth ? CloakMaterialStencilCode : 0);
		if (bDebug)
		{
			UE_LOG(LogElectricCastle, Display, TEXT("[%s:%s] Setting stencil value on %s: %d"), *GetOwner()->GetName(), *GetName(), *Prim->GetName(), CloakMaterialStencilCode);
		}
	}
}
