// Copyright Alien Shores


#include "Player/Form/PlayerFormChangeComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Form/PlayerFormConfig.h"
#include "Tags/ElectricCastleGameplayTags.h"

UPlayerFormChangeComponent::UPlayerFormChangeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerFormChangeComponent::ChangeForm_Async(const FGameplayTag& FormTag)
{
	if (FormTag.MatchesTagExact(CurrentFormTag))
	{
		return;
	}
	const FPlayerFormConfigRow& Row = GetPlayerFormConfigRow(FormTag);
	if (!Row.IsValid())
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Failed to change form -- invalid game data or form config! %s"), *GetOwner()->GetName(), *GetName(), *FormTag.ToString());
		return;
	}
	const FGameplayTag OldFormTag = CurrentFormTag;
	FLoadSoftObjectPathAsyncDelegate FormLoad;
	FormLoad.BindLambda([this, FormTag, OldFormTag](FSoftObjectPath ObjectPath, UObject* Loaded)
	{
		const UPlayerFormConfig* FormConfig = GetPlayerFormConfig();
		const FPlayerFormConfigRow& Row = GetPlayerFormConfigRow(FormTag);
		if (Row.IsLoaded())
		{
			FPlayerFormChangeEventPayload EventPayload;
			EventPayload.OldFormTag = OldFormTag;
			EventPayload.NewFormTag = FormTag;
			EventPayload.CharacterMesh = Row.CharacterMesh.Get();
			EventPayload.AnimationBlueprintClass = Row.AnimationBlueprint.Get();
			EventPayload.PortraitImage = Row.PortraitImage.Get();
			EventPayload.FormAttributes = Row.FormAttributes;
			if (FormConfig)
			{
				EventPayload.HealthChangeEffect = FormConfig->GetHealthChangeEffect();
				EventPayload.ManaChangeEffect = FormConfig->GetManaChangeEffect();
			}
			CurrentFormTag = FormTag;
			OnPlayerFormChange.Broadcast(EventPayload);
		}
	});
	if (!Row.CharacterMesh.IsNull())
	{
		Row.CharacterMesh.LoadAsync(FormLoad);
	}
	if (!Row.AnimationBlueprint.IsNull())
	{
		Row.AnimationBlueprint.LoadAsync(FormLoad);
	}
	if (!Row.PortraitImage.IsNull())
	{
		Row.PortraitImage.LoadAsync(FormLoad);
	}
}

void UPlayerFormChangeComponent::FormChange_PlayEffect(const FPlayerFormChangeEventPayload& Payload)
{
	if (FormChangeEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			FormChangeEffect,
			GetOwner()->GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::Type::SnapToTarget,
			true,
			true
		);
	}
	if (FormChangeSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			FormChangeSound,
			GetOwner()->GetActorLocation(),
			GetOwner()->GetActorRotation()
		);
	}
}

void UPlayerFormChangeComponent::FormChange_UpdateCharacterMesh(const FPlayerFormChangeEventPayload& Payload)
{
	GetMesh()->SetSkeletalMesh(Payload.CharacterMesh);
	GetMesh()->SetAnimInstanceClass(Payload.AnimationBlueprintClass);
}

void UPlayerFormChangeComponent::FormChange_UpdateAbilities(const FPlayerFormChangeEventPayload& Payload)
{
}

void UPlayerFormChangeComponent::FormChange_UpdateAttributes(const FPlayerFormChangeEventPayload& Payload)
{
	if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = Cast<UElectricCastleAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		bool AttributeFound;
		int32 CharacterLevel = IElectricCastleAbilitySystemInterface::GetCharacterLevel(GetOwner());
		const float OldMaxHealth = AbilitySystemComponent->GetGameplayAttributeValue(UElectricCastleAttributeSet::GetMaxHealthAttribute(), AttributeFound);
		const float OldMaxMana = AbilitySystemComponent->GetGameplayAttributeValue(UElectricCastleAttributeSet::GetMaxManaAttribute(), AttributeFound);
		if (CurrentFormEffectHandle.IsValid())
		{
			UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(GetOwner(), CurrentFormEffectHandle);
		}
		const float StandardMaxHealth = AbilitySystemComponent->GetGameplayAttributeValue(UElectricCastleAttributeSet::GetMaxHealthAttribute(), AttributeFound);
		const float StandardMaxMana = AbilitySystemComponent->GetGameplayAttributeValue(UElectricCastleAttributeSet::GetMaxManaAttribute(), AttributeFound);

		CurrentFormEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffect(
			GetOwner(),
			Payload.FormAttributes,
			CharacterLevel
		);
		const float NewMaxHealth = AbilitySystemComponent->GetGameplayAttributeValue(UElectricCastleAttributeSet::GetMaxHealthAttribute(), AttributeFound);
		const float NewMaxMana = AbilitySystemComponent->GetGameplayAttributeValue(UElectricCastleAttributeSet::GetMaxManaAttribute(), AttributeFound);
		const float HealthModifier = (StandardMaxHealth / OldMaxHealth) * NewMaxHealth;
		const float ManaModifier = (StandardMaxMana / OldMaxMana) * NewMaxMana;
		if (Payload.HealthChangeEffect)
		{
			UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffectWithMagnitude(GetOwner(), Payload.HealthChangeEffect, 1, FElectricCastleGameplayTags::Get().Effect_Magnitude, HealthModifier);
		}
		if (Payload.ManaChangeEffect)
		{
			UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffectWithMagnitude(GetOwner(), Payload.ManaChangeEffect, 1, FElectricCastleGameplayTags::Get().Effect_Magnitude, ManaModifier);
		}
	}
}


// Called when the game starts
void UPlayerFormChangeComponent::BeginPlay()
{
	Super::BeginPlay();
	CharacterMeshComponent = Cast<ACharacter>(GetOwner())->GetMesh();
}

USkeletalMeshComponent* UPlayerFormChangeComponent::GetMesh() const
{
	if (!CharacterMeshComponent.IsExplicitlyNull())
	{
		return CharacterMeshComponent.Get();
	}
	return nullptr;
}

UPlayerFormConfig* UPlayerFormChangeComponent::GetPlayerFormConfig() const
{
	if (const UElectricCastleGameDataSubsystem* GameData = UElectricCastleGameDataSubsystem::Get(this))
	{
		return GameData->GetPlayerFormConfig();
	}
	return nullptr;
}

FPlayerFormConfigRow UPlayerFormChangeComponent::GetPlayerFormConfigRow(const FGameplayTag& FormTag) const
{
	if (const UPlayerFormConfig* FormConfig = GetPlayerFormConfig())
	{
		return FormConfig->GetPlayerFormConfigRowByTag(FormTag);
	}
	return FPlayerFormConfigRow(EPlayerForm::None);
}
