// Copyright Alien Shores


#include "Item/Pickup/TreasurePickup.h"

#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Item/Data/TreasureConfig.h"
#include "Tags/ElectricCastleGameplayTags.h"


class UTreasureConfig;

ATreasurePickup::ATreasurePickup()
{
	PrimaryActorTick.bCanEverTick = false;
	ItemType = FElectricCastleGameplayTags::Get().Item_Type_Treasure;
}

ATreasurePickup* ATreasurePickup::SpawnTreasure(const AActor* WorldContextObject, const FVector& Location, const float Value, const bool bRandomize)
{
	if (Value >= 1.f)
	{
		if (const UTreasureConfig* TreasureConfig = UElectricCastleGameDataSubsystem::Get(WorldContextObject)->GetTreasureConfig())
		{
			if (const FTreasureConfigRow TreasureConfigVal = TreasureConfig->GetTreasureConfig(Value, bRandomize); TreasureConfigVal.IsValid())
			{
				FTransform TreasureTransform;
				TreasureTransform.SetLocation(Location);
				TreasureTransform.SetRotation(WorldContextObject->GetActorRotation().Quaternion());
				ATreasurePickup* TreasurePickup = WorldContextObject->GetWorld()->SpawnActorDeferred<ATreasurePickup>(
					TreasureConfig->GetTreasureClass(),
					TreasureTransform,
					nullptr,
					nullptr,
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);
				TreasurePickup->Count = TreasureConfigVal.Value;
				TreasurePickup->MeshComponent->SetStaticMesh(TreasureConfigVal.Mesh);
				TreasurePickup->FinishSpawning(TreasureTransform);
				return TreasurePickup;
			}
		}
	}
	return nullptr;
}
