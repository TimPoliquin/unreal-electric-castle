// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "LevelAssets/Switch/SwitchDelegates.h"
#include "LockComponent.generated.h"

class ULockComponent;

UENUM(BlueprintType)
enum class EUnlockMode : uint8
{
	Unlocked,
	Key,
	Switch,
	Custom,
};

USTRUCT(BlueprintType)
struct FOnLockComponentUnlockPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> LockedActor;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULockComponent> LockComponent;
	UPROPERTY(BlueprintReadOnly)
	EUnlockMode UnlockType = EUnlockMode::Unlocked;
};

USTRUCT(BlueprintType)
struct FOnLockComponentUnlockProgressPayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> LockedActor;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULockComponent> LockComponent;
	UPROPERTY(BlueprintReadOnly)
	int32 UnlockedSwitches = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 TotalSwitches = 0;
};

UENUM(BlueprintType)
enum class ELockSFXPlaybackLocation : uint8
{
	Default,
	UI,
	ActorLocation
};

USTRUCT(BlueprintType)
struct FLockSFXConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<USoundBase> SoundEffect;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAutoPlay = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ELockSFXPlaybackLocation PlaybackLocation = ELockSFXPlaybackLocation::Default;

	bool IsValid() const
	{
		return !SoundEffect.IsNull();
	}

	ELockSFXPlaybackLocation GetPlaybackLocation(const ELockSFXPlaybackLocation OverridePlaybackLocation) const;
	void AutoPlay(const AActor* WorldContextObject) const;
	void Play(const AActor* WorldContextObject, ELockSFXPlaybackLocation OverridePlaybackLocation = ELockSFXPlaybackLocation::Default) const;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLockComponentUnlockSignature, const FOnLockComponentUnlockPayload&, Payload);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLockComponentUnlockProgressSignature, const FOnLockComponentUnlockProgressPayload&, Payload);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLockComponentResetSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API ULockComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULockComponent();
	FOnLockComponentUnlockSignature OnUnlockDelegate;
	FOnLockComponentUnlockProgressSignature OnUnlockProgressDelegate;
	FOnLockComponentResetSignature OnResetDelegate;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPreconditionMet(const AActor* Player) const;
	UFUNCTION(BlueprintCallable)
	bool TryUnlock(AActor* Player);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsUnlocked() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsLocked() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsManuallyUnlockable() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetUnlockText() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsUnlockedAlways() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsUnlockedByKey() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsUnlockedBySwitch() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsUnlockedByCustomLogic() const;
	UFUNCTION(BlueprintCallable)
	void PlaySuccessSound(const ELockSFXPlaybackLocation PlaybackLocation = ELockSFXPlaybackLocation::Default);
	UFUNCTION(BlueprintCallable)
	void PlayResetSound(const ELockSFXPlaybackLocation PlaybackLocation = ELockSFXPlaybackLocation::Default);
	UFUNCTION(BlueprintCallable)
	void PlayCompletedSound(const ELockSFXPlaybackLocation PlaybackLocation = ELockSFXPlaybackLocation::Default);

	void SetUnlockMode(EUnlockMode InUnlockMode);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Lock")
	EUnlockMode UnlockMode = EUnlockMode::Unlocked;
	/** Key Properties **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Lock|Key", meta=(Categories="Item.Type.Key", EditCondition="UnlockMode == EUnlockMode::Key", EditConditionHides))
	FGameplayTag KeyTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Lock|Key", meta=(EditCondition="UnlockMode == EUnlockMode::Key", EditConditionHides))
	bool bConsumesKey = true;
	/** Switch Properties **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Lock|Switch",
		meta=(EditCondition="UnlockMode == EUnlockMode::Switch", EditConditionHides, ObjectMustImplement="/Script/ElectricCastle.SwitchInterface"))
	TArray<TObjectPtr<AActor>> Switches;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Lock|Switch", meta=(EditCondition="UnlockMode == EUnlockMode::Switch", EditConditionHides))
	bool bOrdered = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Lock|Switch|SFX", meta=(EditCondition="UnlockMode == EUnlockMode::Switch", EditConditionHides))
	FLockSFXConfig SuccessSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Lock|Switch|SFX", meta=(EditCondition="UnlockMode == EUnlockMode::Switch", EditConditionHides))
	FLockSFXConfig ResetSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Lock|Switch|SFX", meta=(EditCondition="UnlockMode == EUnlockMode::Switch", EditConditionHides))
	FLockSFXConfig CompletedSound;
	/** Properties **/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame, Category="Properties|Lock|Debug")
	bool bUnlocked = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Lock|UI")
	FString UnlockText = FString("Unlock");
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties|Lock|UI")
	FString OpenText = FString("Open");

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	bool IsPreconditionMet_Key(const AActor* Player) const;
	UFUNCTION(BlueprintNativeEvent)
	bool IsPreconditionMet_Switch() const;
	UFUNCTION(BlueprintNativeEvent)
	bool IsPreconditionMet_Custom() const;
	UFUNCTION(BlueprintNativeEvent)
	void InitializeUnlock_Key();
	UFUNCTION(BlueprintNativeEvent)
	void InitializeUnlock_Switch();
	UFUNCTION(BlueprintNativeEvent)
	void InitializeUnlock_Custom();
	UFUNCTION(BlueprintNativeEvent)
	void TryUnlock_Key(AActor* Player);
	UFUNCTION(BlueprintNativeEvent)
	void TryUnlock_Switch();
	UFUNCTION(BlueprintNativeEvent)
	void TryUnlock_Custom(AActor* Player);
	UFUNCTION(BlueprintNativeEvent)
	void Unlock(bool bBroadcast = true);

private:
	UFUNCTION()
	void OnSwitchActivated(const FOnSwitchStatusChangedPayload& Payload);
};
