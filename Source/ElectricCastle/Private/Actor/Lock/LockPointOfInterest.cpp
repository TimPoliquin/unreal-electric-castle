// Copyright Alien Shores


#include "Actor/Lock/LockPointOfInterest.h"

#include "Actor/Lock/LockedInterface.h"
#include "ElectricCastle/ElectricCastle.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/InventoryComponent.h"


ALockPointOfInterest::ALockPointOfInterest()
{
	PrimaryActorTick.bCanEverTick = false;
	LockMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lock Mesh Component"));
	LockMeshComponent->SetupAttachment(GetRootComponent());
	LockMeshComponent->SetRenderCustomDepth(false);
}

void ALockPointOfInterest::HighlightActor_Implementation()
{
	if (!bUnlocked)
	{
		LockMeshComponent->SetRenderCustomDepth(true);
	}
}

void ALockPointOfInterest::UnHighlightActor_Implementation()
{
	LockMeshComponent->SetRenderCustomDepth(false);
}

void ALockPointOfInterest::PostLoad_Implementation()
{
	if (bUnlocked)
	{
		DisablePOI();
		for (AActor* Gate : Gates)
		{
			ILockedInterface::Unlock(Gate);
		}
	}
}

void ALockPointOfInterest::BeginPlay()
{
	Super::BeginPlay();
	if (bUnlocked)
	{
		DisablePOI();
		return;
	}
	if (InteractionWidget && InteractionWidget
		->
		GetWidget()
	)
	{
		InitializeInteractionWidgetSettings(InteractionWidget->GetWidget(), InteractText);
	}
	if (PreconditionWidget && PreconditionWidget
		->
		GetWidget()
	)
	{
		InitializePreconditionWidgetSettings(PreconditionWidget->GetWidget(), RuneIcon);
	}
	LockMeshComponent->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
}

bool ALockPointOfInterest::IsPreconditionMet_Implementation(AActor* Player) const
{
	if (bUnlocked)
	{
		return false;
	}
	if (const UInventoryComponent* InventoryComponent = UInventoryComponent::Get(Player))
	{
		return InventoryComponent->HasItemInInventory(KeyTag);
	}
	return false;
}

void ALockPointOfInterest::HandleInteract_Implementation(AActor* Player)
{
	Unlock(Player);
}

void ALockPointOfInterest::Unlock(AActor* Player)
{
	if (bUnlocked)
	{
		return;
	}
	if (UInventoryComponent* InventoryComponent = UInventoryComponent::Get(Player))
	{
		if (InventoryComponent->UseKey(KeyTag))
		{
			bUnlocked = true;
			if (UnlockSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, UnlockSound, GetActorLocation());
			}
			for (AActor* Gate : Gates)
			{
				ILockedInterface::Unlock(Gate);
			}
			PlayUnlockEffect(Player);
			DisablePOI();
			UnHighlightActor(this);
		}
	}
}
