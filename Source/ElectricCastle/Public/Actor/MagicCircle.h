// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicCircle.generated.h"

class UDecalComponent;

UCLASS()
class ELECTRICCASTLE_API AMagicCircle : public AActor
{
	GENERATED_BODY()

public:
	AMagicCircle();

	void SetDecalMaterial(UMaterialInterface* DecalMaterial) const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDecalComponent> DecalComponent;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TickRotation = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector RotateAxis = FVector::UpVector;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bRotateOnTick = false;
};
