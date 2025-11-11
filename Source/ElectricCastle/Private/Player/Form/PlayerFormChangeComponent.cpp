// Copyright Alien Shores


#include "Player/Form/PlayerFormChangeComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemInterface.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "Character/ElectricCastlePlayerCharacter.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerEquipmentComponent.h"
#include "Player/Form/FormConfigLoadRequest.h"
#include "Player/Form/PlayerFormConfig.h"
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
		const FGameplayTag& OldValue = CurrentFormTag;
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
	if (UPlayerFormConfig* FormConfig = GetPlayerFormConfig())
	{
		UFormConfigLoadRequest* LoadRequest = FormConfig->GetOrCreateLoadRequest(Payload.NewFormTag);
		LoadRequest->OnLoadComplete.AddUniqueDynamic(this, &UPlayerFormChangeComponent::OnFormDataLoaded);
		FormConfig->LoadAsync(LoadRequest);
	}
}

void UPlayerFormChangeComponent::OnRep_CurrentFormTag(const FGameplayTag& OldValue) const
{
	if (!CurrentFormTag.IsValid())
	{
		return;
	}
	const FPlayerFormConfigRow& Row = GetPlayerFormConfigRow(CurrentFormTag);
	if (!Row.IsValid())
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
	EventPayload.FormAttributes = Row.FormAttributes;
	if (UPlayerFormConfig* FormConfig = GetPlayerFormConfig())
	{
		EventPayload.HealthChangeEffect = FormConfig->GetHealthChangeEffect();
		EventPayload.ManaChangeEffect = FormConfig->GetManaChangeEffect();
		UFormConfigLoadRequest* LoadRequest = FormConfig->GetOrCreateLoadRequest(EventPayload.NewFormTag);
		LoadRequest->OnLoadComplete.AddUniqueDynamic(
			this,
			&UPlayerFormChangeComponent::OnFormDataLoaded_Broadcast
		);
		FormConfig->LoadAsync(LoadRequest);
	}
	OnPlayerFormChange.Broadcast(EventPayload);
}

void UPlayerFormChangeComponent::FormChange_UpdateAbilities(const FPlayerFormChangeEventPayload& Payload)
{
}

void UPlayerFormChangeComponent::FormChange_UpdateAttributes(const FPlayerFormChangeEventPayload& Payload)
{
	if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = Cast<UElectricCastleAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())
	))
	{
		bool AttributeFound;
		const int32 CharacterLevel = IElectricCastleAbilitySystemInterface::GetCharacterLevel(GetOwner());
		const float OldMaxHealth = AbilitySystemComponent->GetGameplayAttributeValue(
			UElectricCastleAttributeSet::GetMaxHealthAttribute(),
			AttributeFound
		);
		const float OldMaxMana = AbilitySystemComponent->GetGameplayAttributeValue(
			UElectricCastleAttributeSet::GetMaxManaAttribute(),
			AttributeFound
		);
		if (CurrentFormEffectHandle.IsValid())
		{
			UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(GetOwner(), CurrentFormEffectHandle);
		}
		const float StandardMaxHealth = AbilitySystemComponent->GetGameplayAttributeValue(
			UElectricCastleAttributeSet::GetMaxHealthAttribute(),
			AttributeFound
		);
		const float StandardMaxMana = AbilitySystemComponent->GetGameplayAttributeValue(
			UElectricCastleAttributeSet::GetMaxManaAttribute(),
			AttributeFound
		);

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
		const float HealthModifier = (StandardMaxHealth / OldMaxHealth) * NewMaxHealth;
		const float ManaModifier = (StandardMaxMana / OldMaxMana) * NewMaxMana;
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

bool UPlayerFormChangeComponent::IsFormLoaded(const FGameplayTag& FormTag) const
{
	if (const FPlayerFormConfigRow& FormConfig = GetPlayerFormConfigRow(FormTag); FormConfig.IsValid())
	{
		return FormConfig.IsLoaded();
	}
	return false;
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


void UPlayerFormChangeComponent::OnFormDataLoaded_Broadcast(const FPlayerFormConfigRow& FormConfigRow)
{
	OnPlayerFormDataLoaded.Broadcast(FormConfigRow);
}

void UPlayerFormChangeComponent::OnFormDataLoaded(const FPlayerFormConfigRow& FormConfigRow)
{
	if (AElectricCastlePlayerCharacter* PlayerCharacter = Cast<AElectricCastlePlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->SetFormMeshes(FormConfigRow.MeshConfig);
		PlayerCharacter->SetAnimInstanceClass(FormConfigRow.AnimationBlueprint.Get());
		if (UPlayerEquipmentComponent* EquipmentComponent = IEquipmentManagerInterface::GetEquipmentComponent(
			PlayerCharacter
		))
		{
			EquipmentComponent->ChangeWeapon(FormConfigRow.WeaponTag, FormConfigRow.WeaponHandSocketName);
		}
	}
}
