// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "EquipmentActor.h"
#include "Actor/Mesh/SocketManagerActor.h"
#include "Actor/Mesh/SocketManagerTypes.h"
#include "GameFramework/Actor.h"
#include "BasicEquipmentActor.generated.h"

class UEquipmentComponent;
class USocketManagerComponent;
class UGameplayEffect;

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API ABasicEquipmentActor : public AActor, public ISocketManagerActor, public IEquipmentActor
{
	GENERATED_BODY()

public:
	ABasicEquipmentActor();

	virtual void PostInitializeComponents() override;

	USkeletalMeshComponent* GetMesh() const;

	/** Start ISocketManagerActor **/
	virtual USocketManagerComponent* GetSocketManagerComponent_Implementation() const override { return SocketManagerComponent; }
	/** End ISocketManagerActor **/

	/** Start IEquipmentActor **/
	virtual bool IsEquipped_Implementation() const override;
	virtual FGameplayTag GetItemType_Implementation() const override;
	virtual void Equip_Implementation(AActor* InOwner) override;
	virtual void Unequip_Implementation(AActor* InOwner) override;
	virtual void Attach_Implementation(AActor* InOwner) override;
	virtual void Detach_Implementation() override;
	virtual void Show_Implementation() override;
	virtual void Hide_Implementation() override;
	virtual void AddImpulse_Implementation(const FVector Impulse) override;
	/** End IEquipmentActor **/


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USkeletalMeshComponent> MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USocketManagerComponent> SocketManagerComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	FSocketConfig TipSocketConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	FGameplayTag ParentAttachSocketTag = FGameplayTag::EmptyTag;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RegisterSocketsToSocketManager(USocketManagerComponent* InSocketManagerComponent) const;
};
