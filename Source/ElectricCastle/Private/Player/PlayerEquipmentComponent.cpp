// Copyright Alien Shores


#include "Player/PlayerEquipmentComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Save/SaveGameBlueprintFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "Item/ItemBlueprintLibrary.h"
#include "Item/Equipment/EquipmentActor.h"
#include "Item/Equipment/FishingRodActor.h"
#include "Tags/ElectricCastleGameplayTags.h"

UPlayerEquipmentComponent::UPlayerEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SaveID = USaveGameBlueprintFunctionLibrary::GenerateSaveID(this);
	SetIsReplicatedByDefault(true);
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
	if (IsValid(Weapon))
	{
		Weapon->SetHidden(true);
		Weapon->GetMesh()->SetVisibility(false);
		Weapon->UnEquip(GetOwner());
	}
	EquipmentUseMode = EEquipmentUseMode::Tool;
	if (!IsValid(Tool))
	{
		Tool = SpawnEquipment(EEquipmentSlot::Tool);
	}
	if (Tool)
	{
		Tool->SetHidden(false);
		Tool->GetMesh()->SetVisibility(true);
		Tool->Equip(GetOwner());
	}
	OnUseTool.Broadcast();
}

void UPlayerEquipmentComponent::UseWeapon()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	if (IsValid(Tool))
	{
		Tool->SetHidden(true);
		Tool->GetMesh()->SetVisibility(false);
		Tool->UnEquip(GetOwner());
	}
	EquipmentUseMode = EEquipmentUseMode::Weapon;
	if (!IsValid(Weapon))
	{
		Weapon = SpawnEquipment(EEquipmentSlot::Weapon);
	}
	if (Weapon)
	{
		Weapon->SetHidden(false);
		Weapon->GetMesh()->SetVisibility(true);
		Weapon->Equip(GetOwner());
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
		AbilitySystemComponent->RemoveReplicatedLooseGameplayTag(
			FElectricCastleGameplayTags::Get().Player_Equipped_Tool
		);
		AbilitySystemComponent->RemoveReplicatedLooseGameplayTag(
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

AEquipmentActor* UPlayerEquipmentComponent::GetWeapon() const
{
	return Weapon;
}

USkeletalMeshComponent* UPlayerEquipmentComponent::GetWeaponMesh() const
{
	if (EquipmentUseMode == EEquipmentUseMode::Weapon && EquipmentSlots.Contains(EEquipmentSlot::Weapon) &&
		IsValid(Weapon))
	{
		return Weapon->GetMesh();
	}
	return nullptr;
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
	if (IsValid(Tool) && Tool->GetItemType() == FElectricCastleGameplayTags::Get().Item_Type_Equipment_FishingRod)
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


void UPlayerEquipmentComponent::ChangeWeapon(const FGameplayTag& WeaponTag, const FName HandSocket)
{
	if (Weapon)
	{
		Weapon->UnEquip(GetOwner());
		GetWorld()->DestroyActor(Weapon);
		Weapon = nullptr;
	}
	if (!WeaponTag.IsValid())
	{
		return;
	}
	if (!HandSocket.IsNone())
	{
		EquipmentSocketNames[EEquipmentSlot::Weapon] = HandSocket;
	}
	Equip(EEquipmentSlot::Weapon, WeaponTag);
	if (IsUsingWeapon())
	{
		UseWeapon();
	}
}

void UPlayerEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
}

USkeletalMeshComponent* UPlayerEquipmentComponent::GetCharacterMesh() const
{
	return Cast<ACharacter>(GetOwner())->GetMesh();
}

AEquipmentActor* UPlayerEquipmentComponent::SpawnEquipment(const EEquipmentSlot& Slot)
{
	if (!EquipmentSlots.Contains(Slot))
	{
		return nullptr;
	}
	USkeletalMeshComponent* CharacterMesh = GetCharacterMesh();
	if (!IsValid(CharacterMesh))
	{
		UE_LOG(
			LogElectricCastle,
			Warning,
			TEXT("[%s] No character mesh set for player %s"),
			*GetName(),
			*GetOwner()->GetName()
		)
		return nullptr;
	}
	const FName SocketName = EquipmentSocketNames[Slot];
	const FVector SocketLocation = CharacterMesh->GetSocketLocation(SocketName);
	const FRotator SocketRotation = CharacterMesh->GetSocketRotation(SocketName);

	const FItemDefinition ItemDefinition = UItemBlueprintLibrary::GetItemDefinitionByItemType(
		this,
		EquipmentSlots[Slot]
	);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = CharacterMesh->GetOwner();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AEquipmentActor* Equipment = GetWorld()->SpawnActor<AEquipmentActor>(
		ItemDefinition.EquipmentClass,
		SocketLocation,
		SocketRotation,
		SpawnParameters
	);
	if (!Equipment)
	{
		UE_LOG(
			LogElectricCastle,
			Error,
			TEXT("[%s][%s] Failed to spawn equipment for slot [%s]. Check item configuration."),
			*GetOwner()->GetName(),
			*GetName(),
			*ItemDefinition.ItemType.ToString()
		);
		return nullptr;
	}
	Equipment->AttachToComponent(
		CharacterMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		SocketName
	);
	UE_LOG(
		LogElectricCastle,
		Warning,
		TEXT("[%s] Spawned equipment [%s] at socket %s"),
		GetOwner()->HasAuthority() ? *FString("Server") : *FString("Client"),
		*Equipment->GetName(),
		*SocketName.ToString()
	)
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
