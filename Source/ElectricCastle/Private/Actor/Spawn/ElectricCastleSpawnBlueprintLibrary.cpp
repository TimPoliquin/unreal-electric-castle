// Copyright Alien Shores


#include "Actor/Spawn/ElectricCastleSpawnBlueprintLibrary.h"

#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Kismet/KismetMathLibrary.h"

TArray<FTransform> UElectricCastleSpawnBlueprintLibrary::GenerateSpawnLocations(
	const FVector& Center,
	const float Radius,
	const int32 NumSpawns,
	const float Spread,
	const FVector ForwardVector,
	const FVector UpVector
)
{
	TArray<FTransform> SpawnTransforms;
	TArray<FRotator> Rotators = UElectricCastleAbilitySystemLibrary::EvenlySpacedRotators(ForwardVector, UpVector, Spread, NumSpawns);
	for (FRotator Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(UKismetMathLibrary::GetForwardVector(Rotator) * Radius + Center);
		SpawnTransform.SetRotation(Rotator.Quaternion());
		SpawnTransforms.Add(SpawnTransform);
	}
	return SpawnTransforms;
}

FAuraSpawnParams UElectricCastleSpawnBlueprintLibrary::SafePop(TArray<FAuraSpawnParams>& SpawnParams)
{
	if (SpawnParams.Num() > 0)
	{
		return SpawnParams.Pop();
	}
	UE_LOG(LogElectricCastle, Error, TEXT("[UElectricCastleSpawnBlueprintLibrary.SafePop] Attempted to pop off an empty array!"))
	return FAuraSpawnParams();
}

bool UElectricCastleSpawnBlueprintLibrary::IsValid(const FAuraSpawnParams& SpawnParams)
{
	return SpawnParams.IsValid();
}

FVector UElectricCastleSpawnBlueprintLibrary::GetGroundLocation(const AActor* Actor)
{
	if (!Actor)
	{
		return FVector::ZeroVector;
	}
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Actor);
	if (Actor->GetWorld()->LineTraceSingleByChannel(Hit, Actor->GetActorLocation(), Actor->GetActorLocation() + (FVector::DownVector * 1000), ECC_WorldStatic, Params))
	{
		return Hit.ImpactPoint;
	}
	return Actor->GetActorLocation();
}
