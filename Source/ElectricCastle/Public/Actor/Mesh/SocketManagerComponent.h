// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SocketManagerTypes.h"
#include "Components/ActorComponent.h"
#include "SocketManagerComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API USocketManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USocketManagerComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasSocket(const FGameplayTag& SocketTag) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetSocketLocation(const FGameplayTag& SocketTag) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FTransform GetSocketTransform(const FGameplayTag& SocketTag) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FName GetSocketName(const FGameplayTag& SocketTag) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	USkeletalMeshComponent* GetMeshBySocketTag(const FGameplayTag& SocketTag) const;
	UFUNCTION(BlueprintCallable)
	void RegisterSocket(USkeletalMeshComponent* InMeshComponent, const FGameplayTag SocketTag, const FName SocketName);
	UFUNCTION(BlueprintCallable)
	void RegisterSocketConfig(const FSocketMeshConfig& SocketConfig);
	UFUNCTION(BlueprintCallable)
	void AttachByTag(USkeletalMeshComponent* InSkeletalMesh, const FGameplayTag SocketTag) const;
	static USocketManagerComponent* GetSocketManagerComponent(const UObject* Actor);

private:
	UPROPERTY(Replicated)
	TArray<FSocketMeshConfig> SocketConfigs;

	FSocketMeshConfig GetSocketMeshConfig(const FGameplayTag& SocketTag) const;
};
