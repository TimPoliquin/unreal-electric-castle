// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "FormConfig.h"
#include "GameplayTagContainer.h"
#include "FormLockOnConfig.generated.h"

/**
 * 
 */
UCLASS()
class ELECTRICCASTLE_API UFormLockOnConfig : public UFormConfig
{
	GENERATED_BODY()

public:
	UFormLockOnConfig();
	virtual void OnFormActivated_Implementation(AActor* Actor, UPlayerFormChangeComponent* FormChangeComponent) override;
	virtual void OnFormDeactivated_Implementation(AActor* Actor, UPlayerFormChangeComponent* FormChangeComponent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Abilities"))
	FGameplayTag LockOnAbilityTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Abilities"))
	bool bAutoGrantAndRemove = false;
};
