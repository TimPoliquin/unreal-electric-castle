// Copyright Alien Shores


#include "Item/Equipment/BasicEquipmentActor.h"

#include "Actor/Mesh/SocketManagerComponent.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Item/Equipment/EquipmentComponent.h"
#include "Tags/ElectricCastleGameplayTags.h"


// Sets default values
ABasicEquipmentActor::ABasicEquipmentActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetIsReplicated(true);
	MeshComponent->SetupAttachment(GetRootComponent());
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment Component"));
	SocketManagerComponent = CreateDefaultSubobject<USocketManagerComponent>(TEXT("Socket Manager Component"));
	TipSocketConfig.SocketTag = FElectricCastleGameplayTags::Get().Combat_Socket_Weapon_Tip;
	TipSocketConfig.SocketName = TEXT("TipSocket");
	ParentAttachSocketTag = FElectricCastleGameplayTags::Get().Combat_Socket_RightHand;
}

USkeletalMeshComponent* ABasicEquipmentActor::GetMesh() const
{
	return MeshComponent;
}

bool ABasicEquipmentActor::IsEquipped_Implementation() const
{
	return EquipmentComponent->IsEquipped();
}

FGameplayTag ABasicEquipmentActor::GetItemType_Implementation() const
{
	return EquipmentComponent->GetItemType();
}

void ABasicEquipmentActor::Equip_Implementation(AActor* InOwner)
{
	EquipmentComponent->Equip(InOwner);
}

void ABasicEquipmentActor::Unequip_Implementation(AActor* InOwner)
{
	EquipmentComponent->Unequip(InOwner);
}

void ABasicEquipmentActor::Attach_Implementation(AActor* InOwner)
{
	if (const USocketManagerComponent* OwnerSocketManagerComponent = GetSocketManagerComponent(InOwner))
	{
		OwnerSocketManagerComponent->AttachByTag(MeshComponent, ParentAttachSocketTag);
	}
	else
	{
		UE_LOG(LogElectricCastle, Error, TEXT("[%s] Cannot attach to owner %s - it must implement ISocketManagerActor!"), *GetName(), *InOwner->GetName());
	}
}

void ABasicEquipmentActor::Detach_Implementation()
{
	MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetEnableGravity(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
}

void ABasicEquipmentActor::Show_Implementation()
{
	SetHidden(false);
	GetMesh()->SetVisibility(true);
}

void ABasicEquipmentActor::Hide_Implementation()
{
	SetHidden(true);
	GetMesh()->SetVisibility(false);
}

void ABasicEquipmentActor::AddImpulse_Implementation(const FVector Impulse)
{
	GetMesh()->AddImpulse(Impulse, NAME_None, true);
}

void ABasicEquipmentActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	RegisterSocketsToSocketManager(SocketManagerComponent);
}

void ABasicEquipmentActor::RegisterSocketsToSocketManager_Implementation(USocketManagerComponent* InSocketManagerComponent) const
{
	InSocketManagerComponent->RegisterSocket(MeshComponent, FElectricCastleGameplayTags::Get().Combat_Socket_Weapon_Tip, FName("TipSocket"));
}
