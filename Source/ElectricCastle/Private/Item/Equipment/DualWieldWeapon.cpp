// Copyright Alien Shores


#include "Item/Equipment/DualWieldWeapon.h"

#include "Actor/Mesh/SocketManagerComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Item/Equipment/EquipmentComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"


// Sets default values
ADualWieldWeapon::ADualWieldWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	LeftHandMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Left Hand Mesh"));
	LeftHandMeshComponent->SetIsReplicated(true);
	LeftHandMeshComponent->SetupAttachment(GetRootComponent());
	RightHandMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Right Hand Mesh"));
	RightHandMeshComponent->SetIsReplicated(true);
	RightHandMeshComponent->SetupAttachment(GetRootComponent());
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment Component"));
	EquipmentComponent->SetItemType(FElectricCastleGameplayTags::Get().Item_Type_Equipment_Weapon);
	SocketManagerComponent = CreateDefaultSubobject<USocketManagerComponent>(TEXT("Socket Manager Component"));
	LeftHandConfig.ParentAttachSocketTag = FElectricCastleGameplayTags::Get().Combat_Socket_LeftHand;
	LeftHandConfig.TipSocketConfig = FSocketConfig(
		FElectricCastleGameplayTags::Get().Combat_Socket_Weapon_Tip,
		FName("TipSocket")
	);
	RightHandConfig.ParentAttachSocketTag = FElectricCastleGameplayTags::Get().Combat_Socket_RightHand;
	RightHandConfig.TipSocketConfig = FSocketConfig(
		FElectricCastleGameplayTags::Get().Combat_Socket_Weapon_Tip,
		FName("TipSocket")
	);
}

void ADualWieldWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SocketManagerComponent->RegisterSocket(
		LeftHandMeshComponent,
		LeftHandConfig.TipSocketConfig.SocketTag,
		LeftHandConfig.TipSocketConfig.SocketName
	);
	SocketManagerComponent->RegisterSocket(
		RightHandMeshComponent,
		RightHandConfig.TipSocketConfig.SocketTag,
		RightHandConfig.TipSocketConfig.SocketName
	);
}

bool ADualWieldWeapon::IsEquipped_Implementation() const
{
	return EquipmentComponent->IsEquipped();
}

FGameplayTag ADualWieldWeapon::GetItemType_Implementation() const
{
	return EquipmentComponent->GetItemType();
}

void ADualWieldWeapon::Equip_Implementation(AActor* InOwner)
{
	EquipmentComponent->Equip(InOwner);
}

void ADualWieldWeapon::Unequip_Implementation(AActor* InOwner)
{
	EquipmentComponent->Unequip(InOwner);
}

void ADualWieldWeapon::Show_Implementation()
{
	SetHidden(false);
	LeftHandMeshComponent->SetVisibility(true);
	RightHandMeshComponent->SetVisibility(true);
}

void ADualWieldWeapon::Hide_Implementation()
{
	SetHidden(true);
	LeftHandMeshComponent->SetVisibility(false);
	RightHandMeshComponent->SetVisibility(false);
}

void ADualWieldWeapon::AddImpulse_Implementation(const FVector Impulse)
{
	LeftHandMeshComponent->AddImpulse(Impulse, NAME_None, true);
	RightHandMeshComponent->AddImpulse(Impulse, NAME_None, true);
}

void ADualWieldWeapon::Attach_Implementation(AActor* InOwner)
{
	if (const USocketManagerComponent* OwnerSocketManager = GetSocketManagerComponent(InOwner))
	{
		OwnerSocketManager->AttachByTag(LeftHandMeshComponent, LeftHandConfig.ParentAttachSocketTag);
		OwnerSocketManager->AttachByTag(RightHandMeshComponent, RightHandConfig.ParentAttachSocketTag);
	}
	else
	{
		UE_LOG(
			LogElectricCastle,
			Error,
			TEXT("[%s] Cannot equip to owner %s - it must implement ISocketManagerActor!"),
			*GetName(),
			*InOwner->GetName()
		);
	}
}

void ADualWieldWeapon::Detach_Implementation()
{
	LeftHandMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	LeftHandMeshComponent->SetSimulatePhysics(true);
	LeftHandMeshComponent->SetEnableGravity(true);
	LeftHandMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	RightHandMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	RightHandMeshComponent->SetSimulatePhysics(true);
	RightHandMeshComponent->SetEnableGravity(true);
	RightHandMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
}
