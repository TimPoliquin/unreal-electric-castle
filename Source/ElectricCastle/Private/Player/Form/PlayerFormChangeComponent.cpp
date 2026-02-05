// Copyright Alien Shores


#include "Player/Form/PlayerFormChangeComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "Character/ElectricCastlePlayerCharacter.h"
#include "DSP/BufferDiagnostics.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerEquipmentComponent.h"
#include "Player/Form/PlayerFormPrimaryAsset.h"
#include "Tags/ElectricCastleGameplayTags.h"

UPlayerFormChangeComponent::UPlayerFormChangeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UPlayerFormChangeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlayerFormChangeComponent, CurrentFormTag);
}

void UPlayerFormChangeComponent::ChangeForm_Implementation(const FGameplayTag& NewFormTag)
{
	if (GetOwner()->HasAuthority() && !CurrentFormTag.MatchesTagExact(NewFormTag))
	{
		const FGameplayTag OldValue = CurrentFormTag;
		CurrentFormTag = NewFormTag;
		OnRep_CurrentFormTag(OldValue);
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
	if (const UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(this))
	{
		if (const UPlayerFormPrimaryAsset* FormAsset = GameDataSubsystem->GetPlayerFormConfigByTag(Payload.NewFormTag))
		{
			OnFormDataLoaded(FormAsset);
		}
	}
}

void UPlayerFormChangeComponent::OnRep_CurrentFormTag(const FGameplayTag& OldValue) const
{
	if (!CurrentFormTag.IsValid())
	{
		return;
	}
	const UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(this);
	UPlayerFormPrimaryAsset* Row = GameDataSubsystem ? GetPlayerFormConfigRow(CurrentFormTag) : nullptr;
	if (!Row)
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s:%s] Failed to change form -- invalid game data or form config! %s"),
			*GetOwner()->GetName(),
			*GetName(),
			*CurrentFormTag.ToString()
		);
		return;
	}
	FPlayerFormChangeEventPayload EventPayload;
	EventPayload.OldFormTag = OldValue;
	EventPayload.NewFormTag = CurrentFormTag;
	EventPayload.FormAttributes = Row->FormAttributes;
	EventPayload.HealthChangeEffect = GameDataSubsystem->GetFormChangeHealthEffectClass();
	EventPayload.ManaChangeEffect = GameDataSubsystem->GetFormChangeManaEffectClass();
	EventPayload.FormData = Row;
	OnPlayerFormChange.Broadcast(EventPayload);
}

void UPlayerFormChangeComponent::FormChange_UpdateAbilities_Implementation(const FPlayerFormChangeEventPayload& Payload)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = Cast<UElectricCastleAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
			GetOwner()
		)
	))
	{
		if (const UPlayerFormPrimaryAsset* OldFormConfig = GetPlayerFormConfigRow(Payload.OldFormTag))
		{
			for (const FGameplayTag& AbilityTag : OldFormConfig->Abilities)
			{
				UE_LOG(
					LogElectricCastle,
					Log,
					TEXT("[%s:%s] Removing ability %s"),
					*GetOwner()->GetName(),
					*GetName(),
					*AbilityTag.ToString()
				);
				AbilitySystemComponent->RemoveAbilitiesWithTag(AbilityTag);
			}
		}
		if (const UPlayerFormPrimaryAsset* CurrentFormConfig = GetPlayerFormConfigRow(Payload.NewFormTag))
		{
			for (const FGameplayTag& AbilityTag : CurrentFormConfig->Abilities)
			{
				UE_LOG(
					LogElectricCastle,
					Log,
					TEXT("[%s:%s] Adding ability %s"),
					*GetOwner()->GetName(),
					*GetName(),
					*AbilityTag.ToString()
				);
				AbilitySystemComponent->GrantAbilitiesWithTag(AbilityTag);
			}
		}
	}
}

void UPlayerFormChangeComponent::FormChange_UpdateAttributes_Implementation(
	const FPlayerFormChangeEventPayload& Payload
)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	if (const UElectricCastleAbilitySystemComponent* AbilitySystemComponent = Cast<UElectricCastleAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())
	))
	{
		bool AttributeFound;
		const int32 CharacterLevel = IElectricCastleAbilitySystemInterface::GetCharacterLevel(GetOwner());
		const float OldHealth = AbilitySystemComponent->GetGameplayAttributeValue
			(UElectricCastleAttributeSet::GetHealthAttribute(), AttributeFound);
		const float OldMaxHealth = AbilitySystemComponent->GetGameplayAttributeValue(
			UElectricCastleAttributeSet::GetMaxHealthAttribute(),
			AttributeFound
		);
		const float OldMana = AbilitySystemComponent->GetGameplayAttributeValue(UElectricCastleAttributeSet::GetManaAttribute(), AttributeFound);
		const float OldMaxMana = AbilitySystemComponent->GetGameplayAttributeValue(
			UElectricCastleAttributeSet::GetMaxManaAttribute(),
			AttributeFound
		);
		if (CurrentFormEffectHandle.IsValid())
		{
			UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(GetOwner(), CurrentFormEffectHandle);
		}
		CurrentFormEffectHandle = UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffect(
			GetOwner(),
			Payload.FormAttributes,
			CharacterLevel
		);
		const float NewMaxHealth = AbilitySystemComponent->GetGameplayAttributeValue(
			UElectricCastleAttributeSet::GetMaxHealthAttribute(),
			AttributeFound
		);
		const float NewMaxMana = AbilitySystemComponent->GetGameplayAttributeValue(
			UElectricCastleAttributeSet::GetMaxManaAttribute(),
			AttributeFound
		);

		const float HealthModifier = NewMaxHealth * (OldHealth / OldMaxHealth);
		const float ManaModifier = NewMaxMana * (OldMana / OldMaxMana);
		if (Payload.HealthChangeEffect)
		{
			UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffectWithMagnitude(
				GetOwner(),
				Payload.HealthChangeEffect,
				1,
				FElectricCastleGameplayTags::Get().Effect_Magnitude,
				HealthModifier
			);
		}
		if (Payload.ManaChangeEffect)
		{
			UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffectWithMagnitude(
				GetOwner(),
				Payload.ManaChangeEffect,
				1,
				FElectricCastleGameplayTags::Get().Effect_Magnitude,
				ManaModifier
			);
		}
	}
}


void UPlayerFormChangeComponent::AddAvailableForm(const FGameplayTag& FormTag)
{
	if (FormTag.IsValid() && !AvailableForms.HasTagExact(FormTag))
	{
		AvailableForms.AddTag(FormTag);
		OnAvailableFormsChanged.Broadcast(FOnPlayerAvailableFormsChangedPayload(GetOwner(), AvailableForms));
	}
}

void UPlayerFormChangeComponent::RemoveAvailableForm(const FGameplayTag& FormTag)
{
	if (FormTag.IsValid() && AvailableForms.HasTagExact(FormTag))
	{
		AvailableForms.RemoveTag(FormTag);
		OnAvailableFormsChanged.Broadcast(FOnPlayerAvailableFormsChangedPayload(GetOwner(), AvailableForms));
	}
}

bool UPlayerFormChangeComponent::IsFormAvailable(const FGameplayTag& FormTag) const
{
	return FormTag.IsValid() && AvailableForms.HasTagExact(FormTag);
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

UPlayerFormPrimaryAsset* UPlayerFormChangeComponent::GetPlayerFormConfigRow(const FGameplayTag& FormTag) const
{
	if (const UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(this))
	{
		return GameDataSubsystem->GetPlayerFormConfigByTag(FormTag);
	}
	return nullptr;
}

void UPlayerFormChangeComponent::OnFormDataLoaded(const UPlayerFormPrimaryAsset* FormAsset)
{
	if (AElectricCastlePlayerCharacter* PlayerCharacter = Cast<AElectricCastlePlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->SetFormMeshes(FormAsset->MeshConfig);
		PlayerCharacter->SetAnimInstanceClass(FormAsset->AnimationBlueprint.Get());
		if (UPlayerEquipmentComponent* EquipmentComponent = IEquipmentManagerInterface::GetEquipmentComponent(
			PlayerCharacter
		))
		{
			EquipmentComponent->ChangeWeapon(FormAsset->WeaponTag);
		}
	}
}
