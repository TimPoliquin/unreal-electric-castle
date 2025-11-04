// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item/Component/LockComponent.h"
#include "SwitchTrackingActor.generated.h"

class ULockComponent;

UCLASS()
class ELECTRICCASTLE_API ASwitchTrackingActor : public AActor
{
	GENERATED_BODY()

public:
	ASwitchTrackingActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnUnlock(
		UPARAM(ref)
		const FOnLockComponentUnlockPayload& Payload
	);
	UFUNCTION(BlueprintNativeEvent)
	void OnReset();
	UFUNCTION(BlueprintNativeEvent)
	void OnUnlockProgress(const FOnLockComponentUnlockProgressPayload& Payload);

	UFUNCTION(BlueprintCallable)
	void PlaySuccessSound(ELockSFXPlaybackLocation PlaybackLocation = ELockSFXPlaybackLocation::Default);
	UFUNCTION(BlueprintCallable)
	void PlayResetSound(const ELockSFXPlaybackLocation PlaybackLocation = ELockSFXPlaybackLocation::Default);
	UFUNCTION(BlueprintCallable)
	void PlayCompletedSound(const ELockSFXPlaybackLocation PlaybackLocation = ELockSFXPlaybackLocation::Default);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<ULockComponent> LockComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties", meta=(ObjectMustImplement="Script/Aura.AuraLockedInterface"))
	TArray<AActor*> LockedActors;
};
