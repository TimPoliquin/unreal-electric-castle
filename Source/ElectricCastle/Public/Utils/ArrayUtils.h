// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ArrayUtils.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UArrayUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template <typename T>
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static T GetRandomElement(const TArray<T>& Array);
	/**
	 *Returns <code>true</code> if Source contains all elements in the Elements list
	 */
	template <typename T>
	static bool ContainsAll(const TArray<T>& Source, const TArray<T>& Elements);
	template <typename T>
	static TArray<T> RemoveAndCollapse(const TArray<T>& Source, T Element);
	template <typename T>
	static void ShuffleArray(TArray<T>& Array);
	template <typename T>
	UFUNCTION(BlueprintCallable)
	static T Pop(
		UPARAM(ref)
		TArray<T>& Array
	);
};

template <typename T>
T UArrayUtils::GetRandomElement(const TArray<T>& Array)
{
	if (Array.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Attempting to get random element from empty array!"));
		return T();
	}
	return Array[FMath::RandRange(0, Array.Num() - 1)];
}

template <typename T>
bool UArrayUtils::ContainsAll(const TArray<T>& Source, const TArray<T>& Elements)
{
	for (const T& Element : Elements)
	{
		if (!Source.Contains(Element))
		{
			return false;
		}
	}
	return true;
}

template <typename T>
TArray<T> UArrayUtils::RemoveAndCollapse(const TArray<T>& Source, T Element)
{
	TArray<T> Collapsed;
	for (T Current : Source)
	{
		if (Current != Element)
		{
			Collapsed.Add(Current);
		}
	}
	return Collapsed;
}

template <typename T>
void UArrayUtils::ShuffleArray(TArray<T>& Array)
{
	const int32 LastIndex = Array.Num() - 1;
	for (int32 i = LastIndex; i > 0; --i)
	{
		const int32 RandomIndex = FMath::RandRange(0, i);
		Array.Swap(i, RandomIndex);
	}
}

template <typename T>
T UArrayUtils::Pop(TArray<T>& Array)
{
	return Array.Pop();
}
