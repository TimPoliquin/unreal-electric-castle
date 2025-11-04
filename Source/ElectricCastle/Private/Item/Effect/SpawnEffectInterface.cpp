// Copyright Alien Shores


#include "Item/Effect/SpawnEffectInterface.h"

void ISpawnEffectInterface::PlaySpawnEffect(AActor* Actor, const FTransform& SpawnTransform, const FTransform& TargetTransform, const float HeightMultiplier)
{
	if (Actor->Implements<USpawnEffectInterface>())
	{
		Execute_PlaySpawnEffect(Actor, FAuraSpawnEffectParams(SpawnTransform, TargetTransform, HeightMultiplier));
	}
}

AActor* ISpawnEffectInterface::CreateSpawnEffectActor(const UObject* WorldContextObject, TSubclassOf<AActor> EffectClass, AActor* Child, const bool bStartHidden)
{
	AActor* Animator = WorldContextObject->GetWorld()->SpawnActorDeferred<AActor>(EffectClass, Child->GetActorTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Animator->SetActorHiddenInGame(bStartHidden);
	Animator->FinishSpawning(Child->GetActorTransform());
	Child->SetActorHiddenInGame(bStartHidden);
	Child->SetOwner(Animator);
	Child->AttachToActor(Animator, FAttachmentTransformRules::SnapToTargetIncludingScale);
	return Animator;
}
