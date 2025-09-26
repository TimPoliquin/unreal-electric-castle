// Copyright Alien Shores


#include "Item/Equipment/EquipmentActor.h"

#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"


// Sets default values
AEquipmentActor::AEquipmentActor()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
}

USkeletalMeshComponent* AEquipmentActor::GetMesh() const
{
	return MeshComponent;
}

bool AEquipmentActor::IsEquipped() const
{
	return bIsEquipped;
}

void AEquipmentActor::Equip(AActor* InOwner)
{
	if (IsEquipped())
	{
		return;
	}
	bIsEquipped = true;
	if (EquipGameplayEffect)
	{
		EquippedHandle = UElectricCastleAbilitySystemLibrary::ApplyBasicGameplayEffect(InOwner, EquipGameplayEffect);
	}
}

void AEquipmentActor::UnEquip(AActor* InOwner)
{
	if (!IsEquipped())
	{
		return;
	}
	bIsEquipped = false;
	if (EquippedHandle.IsValid())
	{
		UElectricCastleAbilitySystemLibrary::RemoveGameplayEffect(InOwner, EquippedHandle);
		EquippedHandle.Invalidate();
	}
}
