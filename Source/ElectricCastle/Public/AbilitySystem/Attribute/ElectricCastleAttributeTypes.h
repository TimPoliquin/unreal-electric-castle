#pragma once

#include "ElectricCastleAttributeTypes.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

template <class T>
using TStaticFuncPtr = TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

USTRUCT()
struct FEffectPropertiesVal
{
	GENERATED_BODY()

	FEffectPropertiesVal()
	{
	}

	FEffectPropertiesVal(
		AActor* InAvatarActor,
		AController* InController,
		ACharacter* InCharacter,
		UAbilitySystemComponent* InAbilitySystemComponent
	) :
		AvatarActor(InAvatarActor),
		Controller(InController),
		Character(InCharacter),
		AbilitySystemComponent(InAbilitySystemComponent)
	{
	}

	UPROPERTY()
	AActor* AvatarActor = nullptr;
	UPROPERTY()
	AController* Controller = nullptr;
	UPROPERTY()
	ACharacter* Character = nullptr;
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;
};

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties()
	{
	}

	FEffectPropertiesVal Source;
	FEffectPropertiesVal Target;
	FGameplayEffectContextHandle EffectContextHandle;
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FAttributeSetSaveData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category="Save Data")
	int32 Strength = 0;
	UPROPERTY(BlueprintReadWrite, Category="Save Data")
	int32 Intelligence = 0;
	UPROPERTY(BlueprintReadWrite, Category="Save Data")
	int32 Resilience = 0;
	UPROPERTY(BlueprintReadWrite, Category="Save Data")
	int32 Vigor = 0;
	UPROPERTY(BlueprintReadWrite, Category="Save Data")
	float Health = 0;
	UPROPERTY(BlueprintReadWrite, Category="Save Data")
	float Mana = 0;

	// Serialization function
	friend FArchive& operator<<(FArchive& Ar, FAttributeSetSaveData& Struct)
	{
		Ar << Struct.Strength;
		Ar << Struct.Intelligence;
		Ar << Struct.Resilience;
		Ar << Struct.Vigor;
		Ar << Struct.Health;
		Ar << Struct.Mana;
		return Ar;
	}
};
