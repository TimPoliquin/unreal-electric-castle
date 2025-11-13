// Copyright Alien Shores


#include "Player/Progression/ProgressionComponent.h"

#include "Game/ElectricCastleGameState.h"
#include "Game/Save/SaveGameBlueprintFunctionLibrary.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Tags/ElectricCastleGameplayTags.h"


UProgressionComponent* UProgressionComponent::Get(const UObject* WorldContextObject)
{
	if (const AElectricCastleGameState* GameState = AElectricCastleGameState::Get(WorldContextObject))
	{
		return GameState->GetProgressionComponent();
	}
	return nullptr;
}

UProgressionComponent::UProgressionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SpellPoints = 0;
	SaveID = USaveGameBlueprintFunctionLibrary::GenerateSaveID(this);
	SetIsReplicatedByDefault(true);
}

void UProgressionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UProgressionComponent, Level);
	DOREPLIFETIME(UProgressionComponent, XP);
	DOREPLIFETIME(UProgressionComponent, SpellPoints);
}

int32 UProgressionComponent::GetCharacterLevel() const
{
	return Level;
}

TArray<uint8> UProgressionComponent::SaveData_Implementation()
{
	return SerializeActorComponent();
}

bool UProgressionComponent::LoadData_Implementation(const TArray<uint8>& Data)
{
	return DeserializeActorComponent(Data);
}

FString UProgressionComponent::GetSaveID_Implementation() const
{
	return SaveID;
}

void UProgressionComponent::InitializeLevel(const int32 InLevel)
{
	Level = InLevel;
	OnLevelInitializedDelegate.Broadcast(
		FIntAttributeChangedPayload::CreateBroadcastPayload(
			FElectricCastleGameplayTags::Get().Attributes_Progression_Level,
			InLevel
		)
	);
}

void UProgressionComponent::SetLevel(const int32 NewLevel)
{
	const int PreviousLevel = Level;
	Level = NewLevel;
	OnLevelInitializedDelegate.Broadcast(
		FIntAttributeChangedPayload(
			FElectricCastleGameplayTags::Get().Attributes_Progression_Level,
			PreviousLevel,
			NewLevel
		)
	);
}

void UProgressionComponent::AddToLevel(const int32 AddLevel)
{
	SetLevel(Level + AddLevel);
}

int32 UProgressionComponent::GetXP() const
{
	return XP;
}

void UProgressionComponent::AddToXP(const int32 InXP)
{
	SetXP(InXP + XP);
}

int32 UProgressionComponent::GetSpellPoints() const
{
	return SpellPoints;
}

void UProgressionComponent::AddSpellPoints(const int32 InSpellPoints)
{
	SetSpellPoints(SpellPoints + InSpellPoints);
}

int32 UProgressionComponent::FindLevelByXP(const int32 InXP) const
{
	if (const UElectricCastleGameDataSubsystem* GameData = UElectricCastleGameDataSubsystem::Get(this))
	{
		return GameData->FindLevelByXP(InXP);
	}
	return -1;
}

FLevelUpRewards UProgressionComponent::GetLevelUpRewards(const int32 InLevel) const
{
	if (const UElectricCastleGameDataSubsystem* GameData = UElectricCastleGameDataSubsystem::Get(this))
	{
		return GameData->GetLevelUpRewards(InLevel);
	}
	return FLevelUpRewards();
}

void UProgressionComponent::SetSpellPoints(const int32 InSpellPoints)
{
	const int32 PreviousSpellPoints = SpellPoints;
	SpellPoints = InSpellPoints;
	OnSpellPointsChangeDelegate.Broadcast(
		FIntAttributeChangedPayload(
			FElectricCastleGameplayTags::Get().Attributes_Progression_SpellPoints,
			PreviousSpellPoints,
			InSpellPoints
		)
	);
}

void UProgressionComponent::SetXP(const int32 InXP)
{
	const int32 PreviousXP = XP;
	XP = InXP;
	OnXPChangeDelegate.Broadcast(
		FIntAttributeChangedPayload(FElectricCastleGameplayTags::Get().Attributes_Progression_XP, PreviousXP, InXP)
	);
}

void UProgressionComponent::OnRep_Level(int32 OldLevel) const
{
	OnLevelChangeDelegate.Broadcast(
		FIntAttributeChangedPayload(FElectricCastleGameplayTags::Get().Attributes_Progression_Level, OldLevel, Level)
	);
}

void UProgressionComponent::OnRep_XP(int32 OldXP) const
{
	OnXPChangeDelegate.Broadcast(
		FIntAttributeChangedPayload(FElectricCastleGameplayTags::Get().Attributes_Progression_XP, OldXP, XP)
	);
}

void UProgressionComponent::OnRep_SpellPoints(int32 InSpellPoints) const
{
	OnSpellPointsChangeDelegate.Broadcast(
		FIntAttributeChangedPayload(
			FElectricCastleGameplayTags::Get().Attributes_Progression_SpellPoints,
			InSpellPoints,
			SpellPoints
		)
	);
}

TArray<uint8> UProgressionComponent::SerializeActorComponent()
{
	TArray<uint8> Data;
	FMemoryWriter Writer(Data);

	// Create save data struct
	FAuraProgressionComponentSaveData SaveData;
	SaveData.Level = Level;
	SaveData.XP = XP;
	SaveData.SpellPoints = SpellPoints;

	// Serialize the struct
	Writer << SaveData.Level;
	Writer << SaveData.XP;
	Writer << SaveData.SpellPoints;

	return Data;
}

bool UProgressionComponent::DeserializeActorComponent(const TArray<uint8>& Data)
{
	if (Data.Num() == 0)
	{
		return false;
	}

	FMemoryReader Reader(Data);

	try
	{
		Reader << Level;
		Reader << XP;
		Reader << SpellPoints;

		return true;
	}
	catch (...)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[%s:%s] Failed to deserialize equipment data"),
			*GetOwner()->GetName(),
			*GetName()
		);
		return false;
	}
}
