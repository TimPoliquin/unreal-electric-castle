// Copyright Alien Shores


#include "Item/Pickup/ItemPickupInventory.h"

#include "Interaction/PlayerInterface.h"
#include "Player/InventoryComponent.h"


AItemPickupInventory::AItemPickupInventory()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AItemPickupInventory::BeginPlay()
{
	Super::BeginPlay();
}

void AItemPickupInventory::ActivatePickup(AActor* PickupActor)
{
	if (UInventoryComponent* InventoryComponent = UInventoryComponent::Get(PickupActor))
	{
		const int32 AmountAdded = InventoryComponent->AddToInventory(ItemType, Count);
		if (AmountAdded > 0)
		{
			PlayPickupEffect(PickupActor, AmountAdded == Count);
		}
	}
}

void AItemPickupInventory::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                          const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && OtherActor->Implements<UPlayerInterface>())
	{
		ActivatePickup(OtherActor);
	}
}
