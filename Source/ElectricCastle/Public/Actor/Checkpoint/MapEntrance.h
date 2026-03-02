// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Checkpoint.h"
#include "MapEntrance.generated.h"

UCLASS()
class ELECTRICCASTLE_API AMapEntrance : public ACheckpoint
{
	GENERATED_BODY()

public:
	AMapEntrance(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;
	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag;

	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;
};
