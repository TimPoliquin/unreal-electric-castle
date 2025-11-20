#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SocketManagerTypes.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FSocketConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Combat.Socket"))
	FGameplayTag SocketTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName = NAME_None;

	bool IsValid() const
	{
		return SocketTag.IsValid() && !SocketName.IsNone();
	}
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FSocketMeshConfig
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<USkeletalMeshComponent> MeshComponent;
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag SocketTag;
	UPROPERTY(BlueprintReadWrite)
	FName SocketName;

	bool IsValid() const
	{
		return MeshComponent.IsValid() && SocketTag.IsValid() && !SocketName.IsNone();
	}
};
