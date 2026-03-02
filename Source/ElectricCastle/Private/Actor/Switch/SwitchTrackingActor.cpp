// Copyright Alien Shores


#include "Actor/Switch/SwitchTrackingActor.h"

#include "Actor/Lock/LockInterface.h"
#include "Actor/Lock/LockComponent.h"


// Sets default values
ASwitchTrackingActor::ASwitchTrackingActor()
{
	PrimaryActorTick.bCanEverTick = false;
	LockComponent = CreateDefaultSubobject<ULockComponent>(TEXT("LockComponent"));
	LockComponent->SetUnlockMode(EUnlockMode::Switch);
}

// Called when the game starts or when spawned
void ASwitchTrackingActor::BeginPlay()
{
	Super::BeginPlay();
	LockComponent->OnUnlockDelegate.AddDynamic(this, &ASwitchTrackingActor::OnUnlock);
	LockComponent->OnUnlockProgressDelegate.AddDynamic(this, &ASwitchTrackingActor::OnUnlockProgress);
	LockComponent->OnResetDelegate.AddDynamic(this, &ASwitchTrackingActor::OnReset);
}

void ASwitchTrackingActor::PlaySuccessSound(const ELockSFXPlaybackLocation PlaybackLocation)
{
	LockComponent->PlaySuccessSound(PlaybackLocation);
}

void ASwitchTrackingActor::PlayResetSound(const ELockSFXPlaybackLocation PlaybackLocation)
{
	LockComponent->PlayResetSound(PlaybackLocation);
}

void ASwitchTrackingActor::PlayCompletedSound(const ELockSFXPlaybackLocation PlaybackLocation)
{
	LockComponent->PlayCompletedSound(PlaybackLocation);
}


void ASwitchTrackingActor::OnUnlock_Implementation(const FOnLockComponentUnlockPayload& Payload)
{
	for (AActor* Actor : LockedActors)
	{
		ILockInterface::Unlock(Actor);
	}
}

void ASwitchTrackingActor::OnUnlockProgress_Implementation(const FOnLockComponentUnlockProgressPayload& Payload)
{
	PlaySuccessSound();
}

void ASwitchTrackingActor::OnReset_Implementation()
{
	PlayResetSound();
}
