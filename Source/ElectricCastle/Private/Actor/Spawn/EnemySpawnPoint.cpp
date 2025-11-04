// Copyright Alien Shores


#include "Actor/Spawn/EnemySpawnPoint.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"

AEnemySpawnPoint::AEnemySpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

AElectricCastleEnemyCharacter* AEnemySpawnPoint::SpawnEnemy() const
{
	AElectricCastleEnemyCharacter* Enemy = SpawnConfig.Spawn(this);
	if (!Enemy)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Failed to spawn enemy - check configuration!"), *GetName());
	}
	return Enemy;
}
