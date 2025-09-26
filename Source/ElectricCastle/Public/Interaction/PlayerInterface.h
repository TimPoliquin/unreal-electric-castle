// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "PlayerInterface.generated.h"


DECLARE_DELEGATE(FOnCameraMoveFinishedSignature);

// This class does not need to be modified.
UINTERFACE()
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 InXP);
	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP();
	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(const int32 CurrentXP) const;


	/** Abilities and levels **/
	UFUNCTION(BlueprintNativeEvent)
	FLevelUpRewards GetLevelUpRewards(const int32 CurrentLevel) const;
	UFUNCTION(BlueprintNativeEvent)
	void ApplyLevelUpRewards(const int32 LevelIncrement, const FLevelUpRewards& InLevelUpRewards);
	UFUNCTION(BlueprintNativeEvent)
	void SpendAttributePoints(int32 SpentPoints);
	UFUNCTION(BlueprintNativeEvent)
	void SpendSpellPoints(const int32 SpentPoints);
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial = nullptr);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HideMagicCircle();

	/** Camera **/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MoveCameraToPoint(
		const FVector& Destination,
		const FVector& Direction,
		UCurveFloat* AnimationCurve
	);
	virtual void MoveCameraToPointWithCallback(
		const FVector& Destination,
		const FVector& Direction,
		UCurveFloat* AnimationCurve,
		FOnCameraMoveFinishedSignature& OnCameraMoveFinishedSignature
	) = 0;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ReturnCamera(
		UCurveFloat* AnimationCurve
	);

	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SaveProgress(const FName& CheckpointTag);

	static void AddToXP(AActor* Actor, int32 InXP);
	static bool ImplementsPlayerInterface(const UObject* Object);
	static int32 GetAttributePoints(const UObject* Object);
	static int32 GetSpellPoints(const UObject* Object);
	static void SpendAttributePoints(UObject* Object, int32 SpentPoints);
	static void SpendSpellPoints(UObject* Object, const int32 SpentPoints);
	static void SaveProgress(UObject* Object, const FName& CheckpointTag);
};
