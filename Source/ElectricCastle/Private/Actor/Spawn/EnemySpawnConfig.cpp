#include "Actor/Spawn/EnemySpawnConfig.h"

#include "Character/ElectricCastleEnemyCharacter.h"

bool FEnemySpawnConfig::IsValid() const
{
	return EnemyClass != nullptr;
}

AElectricCastleEnemyCharacter* FEnemySpawnConfig::Spawn(const AActor* SpawnPoint) const
{
	if (AElectricCastleEnemyCharacter* Enemy = SpawnDeferred(SpawnPoint))
	{
		Enemy->FinishSpawning(SpawnPoint->GetActorTransform());
		Enemy->SpawnDefaultController();
		return Enemy;
	}
	return nullptr;
}

AElectricCastleEnemyCharacter* FEnemySpawnConfig::SpawnDeferred(const AActor* SpawnPoint) const
{
	if (!IsValid() || !SpawnPoint)
	{
		return nullptr;
	}
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AElectricCastleEnemyCharacter* Enemy = SpawnPoint->GetWorld()->SpawnActorDeferred<AElectricCastleEnemyCharacter>(EnemyClass, SpawnPoint->GetActorTransform());
	if (Enemy)
	{
		Enemy->SetLevel(EnemyLevel);
	}
	return Enemy;
}
