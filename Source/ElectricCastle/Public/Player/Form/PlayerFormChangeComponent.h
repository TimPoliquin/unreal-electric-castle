// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "PlayerFormDelegates.h"
#include "PlayerFormPrimaryAsset.h"
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
	TSubclassOf<UGameplayEffect> FormAttributes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> HealthChangeEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> ManaChangeEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPlayerFormPrimaryAsset> FormData;

	bool IsValid() const
	{
		return OldFormTag.IsValid() && NewFormTag.IsValid() && FormData;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FPlayerFormChangeEventSignature,
	const FPlayerFormChangeEventPayload&,
	Payload
);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UPlayerFormChangeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerFormChangeComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ChangeForm(const FGameplayTag& NewFormTag);

	UFUNCTION(BlueprintCallable)
	void FormChange_PlayEffect(const FPlayerFormChangeEventPayload& Payload);
	UFUNCTION(BlueprintCallable)
	void FormChange_UpdateCharacterMesh(const FPlayerFormChangeEventPayload& Payload);
	UFUNCTION(BlueprintCallable)
	void FormChange_UpdateInputMappingContext(const FPlayerFormChangeEventPayload& Payload);
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void FormChange_UpdateAbilities(const FPlayerFormChangeEventPayload& Payload);
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void FormChange_UpdateAttributes(const FPlayerFormChangeEventPayload& Payload);
	UFUNCTION(BlueprintCallable)
	FGameplayTag GetCurrentFormTag() const { return CurrentFormTag; }

	UFUNCTION(BlueprintCallable)
	void AddAvailableForm(const FGameplayTag& FormTag);
	UFUNCTION(BlueprintCallable)
	void RemoveAvailableForm(const FGameplayTag& FormTag);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsFormAvailable(const FGameplayTag& FormTag) const;

	UPROPERTY(BlueprintAssignable)
	FPlayerFormChangeEventSignature OnPlayerFormChange;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerAvailableFormsChangedSignature OnAvailableFormsChanged;

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	TObjectPtr<UNiagaraSystem> FormChangeEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	TObjectPtr<USoundBase> FormChangeSound;
	UPROPERTY(
		VisibleInstanceOnly,
		BlueprintReadOnly,
		Category="Form",
		Replicated,
		ReplicatedUsing=OnRep_CurrentFormTag,
		meta=(Categories="Player.Form")
	)
	FGameplayTag CurrentFormTag = FGameplayTag::EmptyTag;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Form")
	FActiveGameplayEffectHandle CurrentFormEffectHandle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Form", meta=(Categories="Player.Form"))
	FGameplayTagContainer AvailableForms;
	UFUNCTION()
	void OnRep_CurrentFormTag(const FGameplayTag& OldValue) const;

private:
	UFUNCTION()
	void OnFormDataLoaded(const UPlayerFormPrimaryAsset* FormAsset);
	UPlayerFormPrimaryAsset* GetPlayerFormConfigRow(const FGameplayTag& FormTag) const;
	USkeletalMeshComponent* GetMesh() const;

	UPROPERTY()
	TWeakObjectPtr<USkeletalMeshComponent> CharacterMeshComponent;
};
