// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "EquipmentActor.h"
#include "Actor/Mesh/SocketManagerActor.h"
#include "Actor/Mesh/SocketManagerTypes.h"
#include "GameFramework/Actor.h"
#include "Player/Equipment/WeaponInterface.h"
#include "DualWieldWeapon.generated.h"

class UEquipmentComponent;
class USocketManagerComponent;

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API ADualWieldWeapon : public AActor, public ISocketManagerActor, public IEquipmentActor, public IWeaponInterface
{
	GENERATED_BODY()

public:
	ADualWieldWeapon();
	virtual void PostInitializeComponents() override;

	/** Start IEquipmentActor **/
	virtual bool IsEquipped_Implementation() const override;
	virtual FGameplayTag GetItemType_Implementation() const override;
	virtual void Equip_Implementation(AActor* InOwner) override;
	virtual void Unequip_Implementation(AActor* InOwner) override;
	virtual void Show_Implementation() override;
	virtual void Hide_Implementation() override;
	virtual void AddImpulse_Implementation(const FVector Impulse) override;
	virtual void Attach_Implementation(AActor* InOwner) override;
	virtual void Detach_Implementation() override;
	/** End IEquipmentActor **/

	/** Start ISocketManagerActor **/
	virtual USocketManagerComponent* GetSocketManagerComponent_Implementation() const override { return SocketManagerComponent; }
	/** End ISocketManagerActor **/

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USkeletalMeshComponent> LeftHandMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USkeletalMeshComponent> RightHandMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USocketManagerComponent> SocketManagerComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UEquipmentComponent> EquipmentComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	FSocketAttachConfig LeftHandConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	FSocketAttachConfig RightHandConfig;
};
