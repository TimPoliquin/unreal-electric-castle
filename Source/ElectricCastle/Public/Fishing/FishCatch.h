// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "FishCatch.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UFishCatch : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FishCatch")
	FGameplayTag FishType = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	FString FishName = FString("");
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FishCatch")
	float Size = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	FString Description = FString("");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	TObjectPtr<const UTexture2D> Icon = nullptr;

	bool IsValid() const
	{
		return FishType.IsValid();
	}

	UFUNCTION()
	void AddToPlayerInventory(AActor* Player);
};
