// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "PlayerFormConfig.h"
#include "UObject/Object.h"
#include "FormConfigLoadRequest.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFormConfigLoadRequestSignature, const FPlayerFormConfigRow&, Row);

struct FGameplayTag;
struct FPlayerFormConfigRow;
/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UFormConfigLoadRequest : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FGameplayTag FormTag;
	UPROPERTY()
	TObjectPtr<UPlayerFormConfig> FormConfig;
	UPROPERTY()
	bool bHasNotified = false;
	FFormConfigLoadRequestSignature Callback;
	void Notify();
	bool IsValid() const;
	bool HasNotified() const;
	bool ShouldNotify() const;
	FPlayerFormConfigRow GetPlayerFormConfigRow() const;
	static UFormConfigLoadRequest* Create(const FGameplayTag& FormTag, UPlayerFormConfig* Config);
};
