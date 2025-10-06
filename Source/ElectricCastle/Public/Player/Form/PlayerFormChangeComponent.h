// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "PlayerFormChangeComponent.generated.h"

class UPlayerFormConfig;
class UNiagaraSystem;
struct FPlayerFormConfigRow;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerFormDataLoadedSignature);

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FPlayerFormChangeEventPayload
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag OldFormTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag NewFormTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> CharacterMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimationBlueprintClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> PortraitImage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> FormAttributes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> HealthChangeEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> ManaChangeEffect;
	UPROPERTY(BlueprintAssignable)
	FPlayerFormDataLoadedSignature OnPlayerFormDataLoaded;

	bool IsValid() const
	{
		return OldFormTag.IsValid() && NewFormTag.IsValid();
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerFormChangeEventSignature, const FPlayerFormChangeEventPayload&, Payload);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UPlayerFormChangeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerFormChangeComponent();

	UFUNCTION(BlueprintCallable)
	void ChangeForm_Async(const FGameplayTag& FormTag);

	UFUNCTION(BlueprintCallable)
	void FormChange_PlayEffect(const FPlayerFormChangeEventPayload& Payload);
	UFUNCTION(BlueprintCallable)
	void FormChange_UpdateCharacterMesh(const FPlayerFormChangeEventPayload& Payload);
	UFUNCTION(BlueprintCallable)
	void FormChange_UpdateAbilities(const FPlayerFormChangeEventPayload& Payload);
	UFUNCTION(BlueprintCallable)
	void FormChange_UpdateAttributes(const FPlayerFormChangeEventPayload& Payload);
	UFUNCTION(BlueprintCallable)
	FGameplayTag GetCurrentFormTag() const { return CurrentFormTag; }

	UPROPERTY(BlueprintAssignable)
	FPlayerFormChangeEventSignature OnPlayerFormChange;

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	TObjectPtr<UNiagaraSystem> FormChangeEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	TObjectPtr<USoundBase> FormChangeSound;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Form", meta=(Categories="Player.Form"))
	FGameplayTag CurrentFormTag = FGameplayTag::EmptyTag;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Form")
	FActiveGameplayEffectHandle CurrentFormEffectHandle;

private:
	UPlayerFormConfig* GetPlayerFormConfig() const;
	FPlayerFormConfigRow GetPlayerFormConfigRow(const FGameplayTag& FormTag) const;
	USkeletalMeshComponent* GetMesh() const;

	UPROPERTY()
	TWeakObjectPtr<USkeletalMeshComponent> CharacterMeshComponent;
};
