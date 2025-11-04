// Copyright Alien Shores


#include "Actor/EffectAreaActor.h"

#include "Item/Component/ApplyGameplayEffectComponent.h"
#include "Kismet/GameplayStatics.h"

AEffectAreaActor::AEffectAreaActor()
{
	PrimaryActorTick.bCanEverTick = true;
	EffectComponent = CreateDefaultSubobject<UApplyGameplayEffectComponent>(TEXT("Effect Component"));
}

void AEffectAreaActor::BeginPlay()
{
	Super::BeginPlay();
	if (bPlaySpawnEffect)
	{
		PlaySpawnEffect();
		if (SpawnSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SpawnSound, GetActorLocation(), GetActorRotation());
		}
	}
}
