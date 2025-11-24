// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "EquipmentActor.h"
#include "Actor/Mesh/SocketManagerActor.h"
#include "Actor/Mesh/SocketManagerTypes.h"
#include "GameFramework/Actor.h"
#include "Player/Equipment/WeaponInterface.h"
#include "FuturemanBlaster.generated.h"

class USocketManagerComponent;
class UEquipmentComponent;

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FFuturemanBlasterConfig
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ParentAttachSocketTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSocketConfig TipSocketConfig;
};

UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API AFuturemanBlaster : public AActor, public ISocketManagerActor, public IEquipmentActor, public IWeaponInterface
{
	GENERATED_BODY()

public:
	AFuturemanBlaster();
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
	TObjectPtr<USkeletalMeshComponent> LeftBlasterMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USkeletalMeshComponent> RightBlasterMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USocketManagerComponent> SocketManagerComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UEquipmentComponent> EquipmentComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	FFuturemanBlasterConfig LeftBlasterConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	FFuturemanBlasterConfig RightBlasterConfig;
};
