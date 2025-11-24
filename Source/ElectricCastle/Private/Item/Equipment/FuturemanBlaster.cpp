// Copyright Alien Shores


#include "Item/Equipment/FuturemanBlaster.h"

#include "Actor/Mesh/SocketManagerComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Item/Equipment/EquipmentComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"


// Sets default values
AFuturemanBlaster::AFuturemanBlaster()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	LeftBlasterMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Left Blaster Mesh"));
	LeftBlasterMeshComponent->SetIsReplicated(true);
	LeftBlasterMeshComponent->SetupAttachment(GetRootComponent());
	RightBlasterMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Right Blaster Mesh"));
	RightBlasterMeshComponent->SetIsReplicated(true);
	RightBlasterMeshComponent->SetupAttachment(GetRootComponent());
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment Component"));
	EquipmentComponent->SetItemType(FElectricCastleGameplayTags::Get().Item_Type_Equipment_Futureman_Blaster);
	EquipmentComponent->SetEquippedTag(FElectricCastleGameplayTags::Get().Player_Equipped_Weapon_Futureman_Blaster);
	SocketManagerComponent = CreateDefaultSubobject<USocketManagerComponent>(TEXT("Socket Manager Component"));
	LeftBlasterConfig.ParentAttachSocketTag = FElectricCastleGameplayTags::Get().Combat_Socket_LeftHand;
	LeftBlasterConfig.TipSocketConfig = FSocketConfig(FElectricCastleGameplayTags::Get().Combat_Socket_Weapon_Tip, FName("TipSocket"));
	RightBlasterConfig.ParentAttachSocketTag = FElectricCastleGameplayTags::Get().Combat_Socket_RightHand;
	RightBlasterConfig.TipSocketConfig = FSocketConfig(FElectricCastleGameplayTags::Get().Combat_Socket_Weapon_Tip, FName("TipSocket"));
}

void AFuturemanBlaster::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SocketManagerComponent->RegisterSocket(LeftBlasterMeshComponent, LeftBlasterConfig.TipSocketConfig.SocketTag, LeftBlasterConfig.TipSocketConfig.SocketName);
	SocketManagerComponent->RegisterSocket(RightBlasterMeshComponent, RightBlasterConfig.TipSocketConfig.SocketTag, RightBlasterConfig.TipSocketConfig.SocketName);
}

bool AFuturemanBlaster::IsEquipped_Implementation() const
{
	return EquipmentComponent->IsEquipped();
}

FGameplayTag AFuturemanBlaster::GetItemType_Implementation() const
{
	return EquipmentComponent->GetItemType();
}

void AFuturemanBlaster::Equip_Implementation(AActor* InOwner)
{
	EquipmentComponent->Equip(InOwner);
}

void AFuturemanBlaster::Unequip_Implementation(AActor* InOwner)
{
	EquipmentComponent->Unequip(InOwner);
}

void AFuturemanBlaster::Show_Implementation()
{
	SetHidden(false);
	LeftBlasterMeshComponent->SetVisibility(true);
	RightBlasterMeshComponent->SetVisibility(true);
}

void AFuturemanBlaster::Hide_Implementation()
{
	SetHidden(true);
	LeftBlasterMeshComponent->SetVisibility(false);
	RightBlasterMeshComponent->SetVisibility(false);
}

void AFuturemanBlaster::AddImpulse_Implementation(const FVector Impulse)
{
	LeftBlasterMeshComponent->AddImpulse(Impulse, NAME_None, true);
	RightBlasterMeshComponent->AddImpulse(Impulse, NAME_None, true);
}

void AFuturemanBlaster::Attach_Implementation(AActor* InOwner)
{
	if (const USocketManagerComponent* OwnerSocketManager = GetSocketManagerComponent(InOwner))
	{
		OwnerSocketManager->AttachByTag(LeftBlasterMeshComponent, LeftBlasterConfig.ParentAttachSocketTag);
		OwnerSocketManager->AttachByTag(RightBlasterMeshComponent, RightBlasterConfig.ParentAttachSocketTag);
	}
	else
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] Cannot equip to owner %s - it must implement ISocketManagerActor!"), *GetName(), *InOwner->GetName());
	}
}

void AFuturemanBlaster::Detach_Implementation()
{
	LeftBlasterMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	LeftBlasterMeshComponent->SetSimulatePhysics(true);
	LeftBlasterMeshComponent->SetEnableGravity(true);
	LeftBlasterMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	RightBlasterMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	RightBlasterMeshComponent->SetSimulatePhysics(true);
	RightBlasterMeshComponent->SetEnableGravity(true);
	RightBlasterMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
}
