#pragma once

#include "CoreMinimal.h"
#include "MagicHandEvents.generated.h"

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FMagicHandPossessPayload
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> MagicHand;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> PossessorActor;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> PossessedActor;

	bool IsValid() const
	{
		return MagicHand != nullptr && PossessorActor != nullptr && PossessedActor != nullptr;
	}
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMagicHandMissSignature);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMagicHandPossessSignature, const FMagicHandPossessPayload&, Payload);
