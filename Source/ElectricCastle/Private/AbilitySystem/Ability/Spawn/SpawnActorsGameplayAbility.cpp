// Copyright Alien Shores


#include "AbilitySystem/Ability/Spawn/SpawnActorsGameplayAbility.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Utils/ArrayUtils.h"

TArray<FVector> USpawnActorsGameplayAbility::GetSpawnLocations() const
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector LeftExtent = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	const float DeltaSpread = SpawnSpread / NumToSpawn;
	TArray<FVector> SpawnLocations;
	for (int32 i = 0; i < NumToSpawn; i++)
	{
		const FVector Direction = LeftExtent.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		FVector SpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnRadius, MaxSpawnRadius);
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(
			Hit,
			SpawnLocation + FVector::UpVector * 400,
			SpawnLocation - FVector::UpVector * 400,
			ECC_Visibility
		);
		if (Hit.bBlockingHit)
		{
			SpawnLocation = Hit.ImpactPoint;
		}
		SpawnLocations.Add(SpawnLocation);
		if (bShowDebug)
		{
			DrawDebugRange(Location, Direction, SpawnLocation);
		}
	}
	return SpawnLocations;
}

TSubclassOf<APawn> USpawnActorsGameplayAbility::GetRandomSpawnClass() const
{
	if (SpawnClasses.Num() > 0)
	{
		return UArrayUtils::GetRandomElement(SpawnClasses);
	}
	UE_LOG(LogTemp, Error, TEXT("AuraSummonAbility SpawnClasses is empty!"));
	return nullptr;
}

void USpawnActorsGameplayAbility::DrawDebugRange(
	const FVector& Location,
	const FVector& Direction,
	const FVector& SpawnLocation
) const
{
	UKismetSystemLibrary::DrawDebugArrow(
		GetAvatarActorFromActorInfo(),
		Location,
		Location + Direction * MaxSpawnRadius,
		4.f,
		FLinearColor::Green,
		3.f
	);
	DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnRadius, 1.f, 12, FColor::Yellow, false, 3.f);
	DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnRadius, 1.f, 12, FColor::Yellow, false, 3.f);
	DrawDebugSphere(GetWorld(), SpawnLocation, 5.f, 12, FColor::Red, false, 3.f);
}
