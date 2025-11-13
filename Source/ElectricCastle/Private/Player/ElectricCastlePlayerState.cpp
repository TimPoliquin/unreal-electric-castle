// Copyright Alien Shores


#include "Player/ElectricCastlePlayerState.h"

#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAttributeSet.h"
#include "Character/ElectricCastleCharacter.h"
#include "Player/Progression/ProgressionComponent.h"

AElectricCastlePlayerState::AElectricCastlePlayerState()
{
	SetNetUpdateFrequency(100.f);
	AbilitySystemComponent = CreateDefaultSubobject<UElectricCastleAbilitySystemComponent>(
		TEXT("AbilitySystemComponent")
	);
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AbilitySystemComponent->bShouldSave = true;
	AttributeSet = CreateDefaultSubobject<UElectricCastleAttributeSet>(TEXT("Player Attributes"));
}

UAbilitySystemComponent* AElectricCastlePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AElectricCastlePlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

UElectricCastleAbilitySystemComponent* AElectricCastlePlayerState::GetElectricCastleAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UElectricCastleAttributeSet* AElectricCastlePlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

int32 AElectricCastlePlayerState::GetCharacterLevel_Implementation() const
{
	if (const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this))
	{
		return ProgressionComponent->GetCharacterLevel();
	}
	return -1;
}

TArray<uint8> AElectricCastlePlayerState::SaveData_Implementation()
{
	TArray<uint8> SaveData;
	FMemoryWriter Writer(SaveData);
	FAttributeSetSaveData AttributeSetSaveData;
	AttributeSet->ToSaveData(AttributeSetSaveData);
	Writer << AttributeSetSaveData;
	return SaveData;
}

bool AElectricCastlePlayerState::LoadData_Implementation(const TArray<uint8>& InData)
{
	try
	{
		if (InData.Num() <= 0)
		{
			return false;
		}
		FMemoryReader Reader(InData);
		FAttributeSetSaveData AttributeSetSaveData;
		Reader << AttributeSetSaveData;
		AttributeSet->FromSaveData(AttributeSetSaveData);
		return true;
	}
	catch (...)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Failed to deserialize attribute data"), *GetName());
		return false;
	}
}

UPlayerFormChangeComponent* AElectricCastlePlayerState::GetFormChangeComponent_Implementation() const
{
	return GetFormChangeComponent(GetPawn());
}

void AElectricCastlePlayerState::InitializeAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
}

void AElectricCastlePlayerState::BeginPlay()
{
	Super::BeginPlay();
	if (const UProgressionComponent* ProgressionComponent = UProgressionComponent::Get(this))
	{
		AttributeSet->InitializeDefaultAttributes(ProgressionComponent->GetCharacterLevel());
	}
}
