// Copyright Alien Shores


#include "Actor/Block/Components/BlockController.h"

#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "Actor/Block/AnimNotify/ParryBlurConfig.h"
#include "Camera/CameraComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"


// Sets default values for this component's properties
UBlockController::UBlockController()
{
	PrimaryComponentTick.bCanEverTick = true;
	BlockingTag = FElectricCastleGameplayTags::Get().Effect_State_Blocking;
}

void UBlockController::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
}

void UBlockController::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (IsValid(ParryBlurConfig))
	{
		ParryBlurConfig->Tick(DeltaTime);
		SetParryBlurStrength(ParryBlurConfig->GetBlurValue());
	}
}

bool UBlockController::IsBlocking() const
{
	return bIsBlocking;
}

void UBlockController::StartBlocking()
{
	if (bIsBlocking)
	{
		return;
	}
	SetComponentTickEnabled(true);
	bIsBlocking = true;
	BlockingEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyInfiniteEffectByTag(GetOwner(), BlockingTag);
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement())
		{
			RestoreMaxSpeed = CharacterMovement->GetMaxSpeed();
			CharacterMovement->MaxWalkSpeed = BlockingMovementSpeed;
		}
	}
}

void UBlockController::EndBlocking()
{
	if (!bIsBlocking)
	{
		return;
	}
	SetComponentTickEnabled(false);
	bIsBlocking = false;
	UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(GetOwner(), BlockingEffectHandle);
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement())
		{
			CharacterMovement->MaxWalkSpeed = RestoreMaxSpeed;
		}
	}
}


void UBlockController::InitializeParryCamera(UCameraComponent* CameraComponent)
{
	if (!IsValid(CameraComponent))
	{
		return;
	}
	if (IsValid(ParryCameraBlurMaterial) && !IsValid(ParryCameraBlurDynamicMaterial))
	{
		ParryCameraBlurDynamicMaterial = UMaterialInstanceDynamic::Create(ParryCameraBlurMaterial, GetOwner());
		FWeightedBlendable Blendable;
		Blendable.Object = ParryCameraBlurDynamicMaterial;
		Blendable.Weight = 1.f;
		CameraComponent->PostProcessSettings.WeightedBlendables.Array.Add(Blendable);
	}
}

void UBlockController::StartParryBlur(UParryBlurConfig* InParryBlurConfig)
{
	ParryBlurConfig = InParryBlurConfig;
	SetParryBlurStrength(0.f);
}

void UBlockController::EndParryBlur()
{
	ParryBlurConfig = nullptr;
	SetParryBlurStrength(0.f);
}

void UBlockController::SetParryBlurStrength(const float ParryBlurStrength) const
{
	if (!IsValid(ParryCameraBlurDynamicMaterial))
	{
		return;
	}
	ParryCameraBlurDynamicMaterial->SetScalarParameterValue(FName("BlurStrength"), ParryBlurStrength);
}
