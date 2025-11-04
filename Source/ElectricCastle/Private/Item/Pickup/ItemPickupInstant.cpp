// Copyright Alien Shores


#include "Item/Pickup/ItemPickupInstant.h"

#include "ElectricCastle/ElectricCastle.h"
#include "Item/Component/ApplyGameplayEffectComponent.h"


// Sets default values
AItemPickupInstant::AItemPickupInstant()
{
	PrimaryActorTick.bCanEverTick = false;
	TArray<FName> PlayerTag;
	PlayerTag.Add(TAG_PLAYER);
	EffectComponent = CreateDefaultSubobject<UApplyGameplayEffectComponent>(TEXT("Effect Component"));
	EffectComponent->SetDestroyOnEffectApplication(true);
	EffectComponent->SetApplyToTags(PlayerTag);
}

int32 AItemPickupInstant::GetItemLevel_Implementation() const
{
	return EffectComponent->GetActorLevel();
}

void AItemPickupInstant::SetItemLevel_Implementation(const int32 Level)
{
	EffectComponent->SetActorLevel(Level);
}

void AItemPickupInstant::OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!EffectComponent->CheckPreRequisites(OtherActor))
	{
		return;
	}
	PlayPickupEffect(OtherActor, true);
	EffectComponent->OnOverlap(OtherActor);
}

void AItemPickupInstant::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	EffectComponent->OnEndOverlap(OtherActor);
}
