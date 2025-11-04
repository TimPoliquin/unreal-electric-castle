// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Components/ActorComponent.h"
#include "Game/Save/SaveableInterface.h"
#include "ProgressionComponent.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FAuraProgressionComponentSaveData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category="Save Data")
	int32 Level = 0;
	UPROPERTY(BlueprintReadWrite, Category="Save Data")
	int32 XP = 0;
	UPROPERTY(BlueprintReadWrite, Category="Save Data")
	int32 SpellPoints = 0;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UProgressionComponent : public UActorComponent, public ISaveableInterface
{
	GENERATED_BODY()

public:
	static UProgressionComponent* Get(const UObject* WorldContextObject);

	UProgressionComponent();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCharacterLevel() const;
	UFUNCTION(BlueprintCallable)
	void AddToLevel(const int32 AddLevel);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetXP() const;
	UFUNCTION(BlueprintCallable)
	void AddToXP(const int32 InXP);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetSpellPoints() const;
	UFUNCTION(BlueprintCallable)
	void AddSpellPoints(const int32 InSpellPoints);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 FindLevelByXP(const int32 InXP) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FLevelUpRewards GetLevelUpRewards(const int32 InLevel) const;

	FOnPlayerStatChangedSignature OnXPChangeDelegate;
	FOnPlayerStatChangedSignature OnLevelChangeDelegate;
	FOnPlayerStatChangedSignature OnLevelInitializedDelegate;
	FOnPlayerStatChangedSignature OnSpellPointsChangeDelegate;

	/** Start ISaveableInterface **/
	virtual TArray<uint8> SaveData_Implementation() override;
	virtual bool LoadData_Implementation(const TArray<uint8>& Data) override;
	virtual bool ShouldSave_Implementation() const override { return true; }
	virtual bool ShouldLoadTransform_Implementation() const override { return false; }
	virtual bool ShouldAutoSpawn_Implementation() const override { return false; }
	virtual FString GetSaveID_Implementation() const override;
	/** Start ISaveableInterface **/


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_XP)
	int32 XP = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_SpellPoints)
	int32 SpellPoints;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Debug")
	FString SaveID;

private:
	void InitializeLevel(const int32 InLevel);
	void SetLevel(const int32 NewLevel);
	void SetXP(const int32 InXP);
	void SetSpellPoints(const int32 InSpellPoints);
	UFUNCTION()
	void OnRep_Level(int32 OldLevel) const;
	UFUNCTION()
	void OnRep_XP(int32 OldXP) const;
	UFUNCTION()
	void OnRep_SpellPoints(int32 InSpellPoints) const;

	TArray<uint8> SerializeActorComponent();
	bool DeserializeActorComponent(const TArray<uint8>& Data);
};
