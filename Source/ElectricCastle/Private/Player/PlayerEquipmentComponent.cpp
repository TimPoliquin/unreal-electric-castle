// Copyright Alien Shores


#include "Player/PlayerEquipmentComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Save/SaveGameBlueprintFunctionLibrary.h"
#include "Item/ItemBlueprintLibrary.h"
#include "Item/Equipment/BasicEquipmentActor.h"
#include "Item/Equipment/FishingRodActor.h"
#include "Net/UnrealNetwork.h"
#include "Tags/ElectricCastleGameplayTags.h"

UPlayerEquipmentComponent::UPlayerEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SaveID = USaveGameBlueprintFunctionLibrary::GenerateSaveID(this);
	SetIsReplicatedByDefault(true);
}

void UPlayerEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlayerEquipmentComponent, Weapon);
	DOREPLIFETIME(UPlayerEquipmentComponent, Tool);
	DOREPLIFETIME(UPlayerEquipmentComponent, EquipmentUseMode);
}

TArray<uint8> UPlayerEquipmentComponent::SaveData_Implementation()
{
	return SerializeComponentData();
}

bool UPlayerEquipmentComponent::LoadData_Implementation(const TArray<uint8>& Data)
{
	return DeserializeComponentData(Data);
}

FString UPlayerEquipmentComponent::GetSaveID_Implementation() const
{
	return SaveID;
}

void UPlayerEquipmentComponent::InitializeEquipment()
{
	switch (EquipmentUseMode)
	{
	case EEquipmentUseMode::Tool:
		UseTool();
		break;
	case EEquipmentUseMode::Weapon:
		UseWeapon();
		break;
	default:
		UseNothing();
		break;
	}
}

bool UPlayerEquipmentComponent::HasToolEquipped(const FGameplayTag& ItemType) const
{
	return EquipmentUseMode == EEquipmentUseMode::Tool && EquipmentSlots[EEquipmentSlot::Tool] == ItemType;
}

bool UPlayerEquipmentComponent::IsUsingTool() const
{
	return EquipmentUseMode == EEquipmentUseMode::Tool;
}

bool UPlayerEquipmentComponent::IsUsingWeapon() const
{
	return EquipmentUseMode == EEquipmentUseMode::Weapon;
}

void UPlayerEquipmentComponent::UseEquipment_Implementation(const EEquipmentSlot Slot)
{
	switch (Slot)
	{
	case EEquipmentSlot::Tool:
		UseTool();
		break;
	case EEquipmentSlot::Weapon:
		UseWeapon();
		break;
	default:
		UseNothing();
		break;
	}
}

void UPlayerEquipmentComponent::UseTool()
{
	if (IsValid(Weapon) && Weapon->Implements<UEquipmentActor>())
	{
		IEquipmentActor::Execute_Hide(Weapon);
		IEquipmentActor::Execute_Unequip(Weapon, GetOwner());
	}
	EquipmentUseMode = EEquipmentUseMode::Tool;
	if (!IsValid(Tool))
	{
		Tool = SpawnEquipment(EEquipmentSlot::Tool);
	}
	if (Tool && Tool->Implements<UEquipmentActor>())
	{
		IEquipmentActor::Execute_Show(Tool);
		IEquipmentActor::Execute_Attach(Tool, GetOwner());
		IEquipmentActor::Execute_Equip(Tool, GetOwner());
	}
	OnUseTool.Broadcast();
}

void UPlayerEquipmentComponent::UseWeapon()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	if (IsValid(Tool) && Tool->Implements<UEquipmentActor>())
	{
		IEquipmentActor::Execute_Hide(Tool);
		IEquipmentActor::Execute_Unequip(Tool, GetOwner());
	}
	EquipmentUseMode = EEquipmentUseMode::Weapon;
	if (!IsValid(Weapon))
	{
		Weapon = SpawnEquipment(EEquipmentSlot::Weapon);
	}
	if (Weapon && Weapon->Implements<UEquipmentActor>())
	{
		IEquipmentActor::Execute_Show(Weapon);
		IEquipmentActor::Execute_Attach(Weapon, GetOwner());
		IEquipmentActor::Execute_Equip(Weapon, GetOwner());
	}
	else
	{
		UE_LOG(
			LogElectricCastle,
			Error,
			TEXT("[%s][%s] Failed to spawn equipment for slot [%s]. Check item configuration."),
			*GetOwner()->GetName(),
			*GetName(),
			*UEnum::GetValueAsString(EEquipmentSlot::Weapon)
		);
	}
	OnUseWeapon.Broadcast();
}

void UPlayerEquipmentComponent::UseNothing()
{
	if (Weapon)
	{
		Weapon->SetHidden(true);
	}
	if (Tool)
	{
		Tool->SetHidden(true);
	}
	EquipmentUseMode = EEquipmentUseMode::None;
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		GetOwner()
	))
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(
			FElectricCastleGameplayTags::Get().Player_Equipped_Tool
		);
		AbilitySystemComponent->RemoveLooseGameplayTag(
			FElectricCastleGameplayTags::Get().Player_Equipped_Weapon
		);
	}
}

void UPlayerEquipmentComponent::Equip(const EEquipmentSlot& Slot, const FGameplayTag& ItemType)
{
	if (EquipmentSlots.Contains(Slot))
	{
		EquipmentSlots[Slot] = ItemType;
	}
	else
	{
		EquipmentSlots.Add(Slot, ItemType);
	}
}

AActor* UPlayerEquipmentComponent::GetWeapon() const
{
	return Weapon;
}

FGameplayTag UPlayerEquipmentComponent::GetToolType() const
{
	if (EquipmentSlots.Contains(EEquipmentSlot::Tool))
	{
		return EquipmentSlots[EEquipmentSlot::Tool];
	}
	return FGameplayTag::EmptyTag;
}

FGameplayTag UPlayerEquipmentComponent::GetWeaponType() const
{
	if (EquipmentSlots.Contains(EEquipmentSlot::Weapon))
	{
		return EquipmentSlots[EEquipmentSlot::Weapon];
	}
	return FGameplayTag::EmptyTag;
}

AFishingRodActor* UPlayerEquipmentComponent::GetFishingRod() const
{
	if (IEquipmentActor::GetItemType(Tool).MatchesTagExact(FElectricCastleGameplayTags::Get().Item_Type_Equipment_FishingRod))
	{
		return Cast<AFishingRodActor>(Tool);
	}
	return nullptr;
}

FGameplayTag UPlayerEquipmentComponent::GetEquippedItem(const EEquipmentSlot Slot) const
{
	if (EquipmentSlots.Contains(Slot))
	{
		return EquipmentSlots[Slot];
	}
	return FGameplayTag::EmptyTag;
}

EEquipmentUseMode UPlayerEquipmentComponent::GetEquipmentUseMode() const
{
	return EquipmentUseMode;
}

void UPlayerEquipmentComponent::PlayEquipAnimation(const EEquipmentSlot Slot) const
{
	const FGameplayTag& ItemType = GetEquippedItem(Slot);
	OnEquipmentAnimationRequest.Broadcast(FEquipmentDelegatePayload(Slot, ItemType));
}

void UPlayerEquipmentComponent::EquipAnimationCompleted_Implementation(const EEquipmentSlot Slot)
{
	OnEquipmentAnimationCompleteDelegate.Broadcast(FEquipmentDelegatePayload(Slot, EquipmentSlots[Slot]));
}


void UPlayerEquipmentComponent::ChangeWeapon(const FGameplayTag& WeaponTag)
{
	if (Weapon && Weapon->Implements<UEquipmentActor>())
	{
		IEquipmentActor::Execute_Unequip(Weapon, GetOwner());
		GetWorld()->DestroyActor(Weapon);
		Weapon = nullptr;
	}
	if (!WeaponTag.IsValid())
	{
		return;
	}
	Equip(EEquipmentSlot::Weapon, WeaponTag);
	if (IsUsingWeapon())
	{
		UseWeapon();
	}
}

AActor* UPlayerEquipmentComponent::SpawnEquipment(const EEquipmentSlot& Slot)
{
	if (!EquipmentSlots.Contains(Slot))
	{
		return nullptr;
	}
	const FItemDefinition ItemDefinition = UItemBlueprintLibrary::GetItemDefinitionByItemType(
		this,
		EquipmentSlots[Slot]
	);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* Equipment = GetWorld()->SpawnActor<AActor>(
		ItemDefinition.EquipmentClass,
		SpawnParameters
	);
	return Equipment;
}

TArray<uint8> UPlayerEquipmentComponent::SerializeComponentData() const
{
	TArray<uint8> Data;
	FMemoryWriter Writer(Data);

	// Create save data struct
	FAuraPlayerEquipmentComponentSaveData SaveData;
	SaveData.EquipmentUseMode = EquipmentUseMode;
	SaveData.EquipmentSlots = EquipmentSlots;

	// Serialize the struct
	Writer << SaveData.EquipmentUseMode;
	Writer << SaveData.EquipmentSlots;

	return Data;
}

bool UPlayerEquipmentComponent::DeserializeComponentData(const TArray<uint8>& Data)
{
	if (Data.Num() == 0)
	{
		return false;
	}

	FMemoryReader Reader(Data);

	try
	{
		Reader << EquipmentUseMode;
		Reader << EquipmentSlots;

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
