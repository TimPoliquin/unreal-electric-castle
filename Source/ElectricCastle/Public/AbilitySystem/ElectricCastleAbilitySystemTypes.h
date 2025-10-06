#pragma once
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "ScalableFloat.h"
#include "ElectricCastleAbilitySystemTypes.generated.h"

struct FRadialDamageParams;

USTRUCT(BlueprintType)
struct FElectricCastleGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;

	bool IsCriticalHit() const
	{
		return bIsCriticalHit;
	}

	void SetIsCriticalHit(const bool bIn)
	{
		bIsCriticalHit = bIn;
	}

	bool IsBlockedHit() const
	{
		return bIsBlockedHit;
	}

	void SetIsBlockedHit(const bool bIn)
	{
		this->bIsBlockedHit = bIn;
	}

	bool IsSuccessfullDebuff() const
	{
		return bIsSuccessfulDebuff;
	}

	int32 GetDebuffLevel() const
	{
		return DebuffLevel;
	}

	float GetDebuffDamage() const
	{
		return DebuffDamage;
	}

	float GetDebuffDuration() const
	{
		return DebuffDuration;
	}

	float GetDebuffFrequency() const
	{
		return DebuffFrequency;
	}

	FGameplayTag* GetDebuffTypeTag() const
	{
		if (DebuffType.IsValid())
		{
			return DebuffType.Get();
		}
		return new FGameplayTag();
	}

	FGameplayTag* GetDamageTypeTag() const
	{
		return DamageType.IsValid()
			       ? DamageType.Get()
			       : new FGameplayTag();
	}

	void SetIsSuccessfulDebuff(const bool InIsSuccessfulDebuff)
	{
		bIsSuccessfulDebuff = InIsSuccessfulDebuff;
	}

	void SetDebuffLevel(const int32 InDebuffLevel)
	{
		DebuffLevel = InDebuffLevel;
	}

	void SetDebuffDamage(const float InDebuffDamage)
	{
		DebuffDamage = InDebuffDamage;
	}

	void SetDebuffDuration(const float InDebuffDuration)
	{
		DebuffDuration = InDebuffDuration;
	}

	void SetDebuffFrequency(const float InDebuffFrequency)
	{
		DebuffFrequency = InDebuffFrequency;
	}

	void SetDebuffTypeTag(TSharedPtr<FGameplayTag> InDebuffType)
	{
		DebuffType = InDebuffType;
	}

	void SetDamageTypeTag(TSharedPtr<FGameplayTag> InDamageType)
	{
		DamageType = InDamageType;
	}

	FVector GetDeathImpulse() const
	{
		return DeathImpulse;
	}

	void SetDeathImpulse(const FVector& InDeathImpulse)
	{
		DeathImpulse = InDeathImpulse;
	}

	FVector GetKnockbackVector() const
	{
		return KnockbackVector;
	}

	void SetKnockbackVector(const FVector& InKnockbackVector)
	{
		KnockbackVector = InKnockbackVector;
	}

	bool IsRadialDamage() const
	{
		return bIsRadialDamage;
	}

	void SetIsRadialDamage(const bool InIsRadialDamage)
	{
		bIsRadialDamage = InIsRadialDamage;
	}

	float GetRadialDamageInnerRadius() const
	{
		return RadialDamageInnerRadius;
	}

	void SetRadialDamageInnerRadius(const float InRadialDamageInnerRadius)
	{
		this->RadialDamageInnerRadius = InRadialDamageInnerRadius;
	}

	float GetRadialDamageOuterRadius() const
	{
		return RadialDamageOuterRadius;
	}

	void SetRadialDamageOuterRadius(const float InRadialDamageOuterRadius)
	{
		this->RadialDamageOuterRadius = InRadialDamageOuterRadius;
	}

	FVector GetRadialDamageOrigin() const
	{
		return RadialDamageOrigin;
	}

	void SetRadialDamageOrigin(const FVector& InRadialDamageOrigin)
	{
		this->RadialDamageOrigin = InRadialDamageOrigin;
	}

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FElectricCastleGameplayEffectContext* Duplicate() const override
	{
		FElectricCastleGameplayEffectContext* NewContext = new FElectricCastleGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

protected:
	UPROPERTY()
	bool bIsBlockedHit = false;
	UPROPERTY()
	bool bIsCriticalHit = false;
	UPROPERTY()
	bool bIsSuccessfulDebuff = false;
	UPROPERTY()
	int32 DebuffLevel = 0;
	UPROPERTY()
	float DebuffDamage = 0.f;
	UPROPERTY()
	float DebuffDuration = 0.f;
	UPROPERTY()
	float DebuffFrequency = 0.f;
	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;
	UPROPERTY()
	FVector KnockbackVector = FVector::ZeroVector;
	UPROPERTY()
	bool bIsRadialDamage = false;


	UPROPERTY()
	float RadialDamageInnerRadius = 0.f;
	UPROPERTY()
	float RadialDamageOuterRadius = 0.f;
	UPROPERTY()
	FVector RadialDamageOrigin = FVector::ZeroVector;


	TSharedPtr<FGameplayTag> DebuffType;
	TSharedPtr<FGameplayTag> DamageType;

private:
	void GetSavingBits(uint32& RepBits) const;
	void AssignValuesToArchive(FArchive& Ar, UPackageMap* Map, const uint32& RepBits, bool& bOutSuccess);
};

template <>
struct TStructOpsTypeTraits<FElectricCastleGameplayEffectContext> : TStructOpsTypeTraitsBase2<FElectricCastleGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true,
	};
};

USTRUCT()
struct FAbilityTagStatus
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag AbilityTag = FGameplayTag::EmptyTag;
	UPROPERTY()
	FGameplayTag StatusTag = FGameplayTag::EmptyTag;
	UPROPERTY()
	int32 AbilityLevel = 0;

	static FAbilityTagStatus Create(
		const FGameplayTag AbilityTag,
		const FGameplayTag StatusTag,
		const int32 AbilityLevel
	)
	{
		FAbilityTagStatus AbilityStatus;
		AbilityStatus.AbilityTag = AbilityTag;
		AbilityStatus.StatusTag = StatusTag;
		AbilityStatus.AbilityLevel = AbilityLevel;
		return AbilityStatus;
	}

	static TArray<FAbilityTagStatus> CreateArray(
		const FGameplayTag AbilityTag,
		const FGameplayTag StatusTag,
		const int32 AbilityLevel
	)
	{
		TArray<FAbilityTagStatus> AbilityStatus;
		AbilityStatus.Add(Create(AbilityTag, StatusTag, AbilityLevel));
		return AbilityStatus;
	}
};

USTRUCT(BlueprintType)
struct FElectricCastleAbilityDescription
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	FString Description = FString();
	UPROPERTY(BlueprintReadOnly)
	FString NextLevelDescription = FString();
};

USTRUCT(BlueprintType)
struct FElectricCastleEquipAbilityPayload
{
	GENERATED_BODY()
	UPROPERTY()
	FGameplayTag AbilityTag;
	UPROPERTY()
	FGameplayTag StatusTag;
	UPROPERTY()
	FGameplayTag SlotTag;
	UPROPERTY()
	FGameplayTag PreviousSlotTag;

	static FElectricCastleEquipAbilityPayload Create(
		const FGameplayTag& AbilityTag,
		const FGameplayTag& StatusTag,
		const FGameplayTag& SlotTag,
		const FGameplayTag& PreviousSlotTag
	)
	{
		FElectricCastleEquipAbilityPayload EquipPayload;
		EquipPayload.AbilityTag = AbilityTag;
		EquipPayload.StatusTag = StatusTag;
		EquipPayload.SlotTag = SlotTag;
		EquipPayload.PreviousSlotTag = PreviousSlotTag;
		return EquipPayload;
	}
};

USTRUCT(BlueprintType)
struct FElectricCastleRadialDamageParams
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsRadialDamage = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RadialDamageInnerRadius = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float RadialDamageOuterRadius = 0.f;
	UPROPERTY(BlueprintReadWrite)
	FVector RadialDamageOrigin = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FElectricCastleDamageConfig
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Categories = "Damage"))
	FGameplayTag DamageTypeTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FScalableFloat Amount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DebuffChance = 20.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DebuffDamage = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DebuffFrequency = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DebuffDuration = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DeathImpulseMagnitude = 6000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float KnockbackForceMagnitude = 500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float KnockbackChance = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsRadialDamage = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RadialDamageInnerRadius = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RadialDamageOuterRadius = 0.f;

	FElectricCastleRadialDamageParams GetRadialDamageParams() const
	{
		FElectricCastleRadialDamageParams DamageParams;
		if (bIsRadialDamage)
		{
			DamageParams.bIsRadialDamage = bIsRadialDamage;
			DamageParams.RadialDamageInnerRadius = RadialDamageInnerRadius;
			DamageParams.RadialDamageOuterRadius = RadialDamageOuterRadius;
		}
		return DamageParams;
	}

	int32 GetDamageAtLevel(const int32 AbilityLevel) const
	{
		return Amount.GetValueAtLevel(AbilityLevel);
	}
};

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()
	FDamageEffectParams()
	{
	}

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent = nullptr;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent = nullptr;
	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f;
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType = FGameplayTag::EmptyTag;
	UPROPERTY(BlueprintReadWrite)
	float DebuffChance = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float DebuffDamage = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float DebuffDuration = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float DebuffFrequency = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 0.f;
	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulse = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite)
	float KnockbackChance = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float KnockbackForceMagnitude = 0.f;
	UPROPERTY(BlueprintReadWrite)
	FVector KnockbackForce = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite)
	bool bIsRadialDamage = false;
	UPROPERTY(BlueprintReadWrite)
	float RadialDamageInnerRadius = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float RadialDamageOuterRadius = 0.f;
	UPROPERTY(BluePrintReadWrite)
	FVector RadialDamageOrigin = FVector::ZeroVector;

	void FillFromDamageConfig(const FElectricCastleDamageConfig& DamageConfig)
	{
		DamageType = DamageConfig.DamageTypeTag;
		DebuffChance = DamageConfig.DebuffChance;
		DebuffDamage = DamageConfig.DebuffDamage;
		DebuffDuration = DamageConfig.DebuffDuration;
		DebuffFrequency = DamageConfig.DebuffFrequency;
		DeathImpulseMagnitude = DamageConfig.DeathImpulseMagnitude;
		KnockbackChance = DamageConfig.KnockbackChance;
		KnockbackForceMagnitude = DamageConfig.KnockbackForceMagnitude;
		const FElectricCastleRadialDamageParams& RadialDamageParams = DamageConfig.GetRadialDamageParams();
		FillFromRadialDamageEffectParams(RadialDamageParams);
	}

	void FillFromRadialDamageEffectParams(const FElectricCastleRadialDamageParams& RadialDamageParams)
	{
		if (RadialDamageParams.bIsRadialDamage)
		{
			bIsRadialDamage = RadialDamageParams.bIsRadialDamage;
			RadialDamageInnerRadius = RadialDamageParams.RadialDamageInnerRadius;
			RadialDamageOuterRadius = RadialDamageParams.RadialDamageOuterRadius;
			RadialDamageOrigin = RadialDamageParams.RadialDamageOrigin;
		}
	}

	bool RollForKnockbackChance() const
	{
		return FMath::RandRange(1, 100) < KnockbackChance;
	}
};
