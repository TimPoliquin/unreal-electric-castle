// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CinematicHandlerInterface.generated.h"

class UCinematicContextHandle;
class UCinematicHandlerComponent;
// This class does not need to be modified.
UINTERFACE()
class UCinematicHandlerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ICinematicHandlerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UCinematicHandlerComponent* GetCinematicHandlerComponent();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool HandleCinematicEvent_Show(const UCinematicContextHandle* CinematicContextHandle);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool HandleCinematicEvent_Hide(const UCinematicContextHandle* CinematicContextHandle);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool HandleCinematicEvent_StopTick(const UCinematicContextHandle* CinematicContextHandle);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool HandleCinematicEvent_StartTick(const UCinematicContextHandle* CinematicContextHandle);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool HandleCinematicEvent_MoveTo(const UCinematicContextHandle* CinematicContextHandle, const FVector& MoveToLocation);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool HandleCinematicEvent_Activate(const UCinematicContextHandle* CinematicContextHandle);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool HandleCinematicEvent_Deactivate(const UCinematicContextHandle* CinematicContextHandle);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool HandleCinematicEvent_OnBegin(const UCinematicContextHandle* CinematicContextHandle);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool HandleCinematicEvent_OnEnd(const UCinematicContextHandle* CinematicContextHandle);

	static bool IsCinematicHandler(const UObject* Object);
	static UCinematicHandlerComponent* GetCinematicHandlerComponent(UObject* Object);
	static bool Show(UObject* Object, const UCinematicContextHandle* CinematicContextHandle);
	static bool Hide(UObject* Object, const UCinematicContextHandle* CinematicContextHandle);
	static bool StopTick(UObject* Object, const UCinematicContextHandle* CinematicContextHandle);
	static bool StartTick(UObject* Object, const UCinematicContextHandle* CinematicContextHandle);
	static bool MoveTo(UObject* Object, const UCinematicContextHandle* CinematicContextHandle, const FVector& MoveToLocation);
	static bool Activate(UObject* Object, const UCinematicContextHandle* CinematicContextHandle);
	static bool Deactivate(UObject* Object, const UCinematicContextHandle* CinematicContextHandle);
	static bool OnBegin(UObject* Object, const UCinematicContextHandle* CinematicContextHandle);
	static bool OnEnd(UObject* Object, const UCinematicContextHandle* CinematicContextHandle);
};
