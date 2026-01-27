// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MagicHandPossessableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UMagicHandPossessableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IMagicHandPossessableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanBePossessedByMagicHand() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USceneComponent* GetMagicHandAttachComponent(FName& AttachBoneName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnMagicHandPossess();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnMagicHandUnpossess();

	static USceneComponent* GetMagicHandAttachComponent(AActor* Actor, FName& AttachBoneName);

	static bool CanBePossessedByMagicHand(const UObject* Actor);
	static void OnMagicHandPossess(UObject* Actor);
	static void OnMagicHandUnpossess(UObject* Actor);
};
