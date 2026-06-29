// Copyright Alien Shores


#include "Actor/Lock/LockPointOfInterest.h"

#include "Actor/Lock/LockInterface.h"
#include "ElectricCastle/ElectricCastle.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Player/InventoryComponent.h"


ALockPointOfInterest::ALockPointOfInterest()
{
	PrimaryActorTick.bCanEverTick = false;
	LockMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Lock Mesh Component"));
	LockMeshComponent->SetupAttachment(GetRootComponent());
	InteractText = FString("Unlock");
	PreconditionText = FString("Key Needed");
}

void ALockPointOfInterest::PostLoad_Implementation()
{
	if (bUnlocked)
	{
		DisablePOI();
		for (AActor* Gate : Gates)
		{
			ILockInterface::Unlock(Gate);
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
				ILockInterface::Unlock(Gate);
			}
			PlayUnlockEffect(Player);
			DisablePOI();
		}
	}
}

UTexture2D* ALockPointOfInterest::GetPreconditionIcon_Implementation() const
{
	if (UElectricCastleGameDataSubsystem* GameDataSubsystem = UElectricCastleGameDataSubsystem::Get(GetWorld()))
	{
		if (const FItemDefinition& ItemDefinition = GameDataSubsystem->FindItemDefinitionByItemTag(KeyTag); ItemDefinition.IsValid())
		{
			return ItemDefinition.ItemIcon;
		}
	}
	return nullptr;
}
