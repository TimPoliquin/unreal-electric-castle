// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "StateShiftInterface.h"
#include "GameFramework/Actor.h"
#include "StateShiftActor.generated.h"

class USphereMaskComponent;

UCLASS()
class ELECTRICCASTLE_API AStateShiftActor : public AActor, public IStateShiftInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AStateShiftActor();

	/** Start IStateShiftInterface **/
	virtual UStateShiftComponent* GetStateShiftComponent() const override;
	UFUNCTION(BlueprintCallable)
	virtual UShapeComponent* GetStateShiftCollisionComponent() const override;
	virtual void StateShiftReaction_Implementation(const FStateShiftStateChangedPayload& Payload) override;
	/** End IStateShiftInterface **/

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStateShiftComponent> StateShiftComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereMaskComponent> SphereMaskComponent;
	UFUNCTION(BlueprintNativeEvent)
	UShapeComponent* FindStateShiftCollisionComponent() const;
	UFUNCTION(NetMulticast, Reliable)
	void StateShiftReaction_NetMulticast(const FStateShiftStateChangedPayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void StateShiftReaction_Visibility_Show();
	UFUNCTION(BlueprintNativeEvent)
	void StateShiftReaction_Visibility_Hide();
	UFUNCTION(BlueprintNativeEvent)
	void StateShiftReaction_Visibility_Mask(const FStateShiftStateChangedPayload& Payload);
	UFUNCTION(BlueprintNativeEvent)
	void StateShiftReaction_Visibility_FadeIn();
	UFUNCTION(BlueprintNativeEvent)
	void StateShiftReaction_Visibility_FadeOut();
	UFUNCTION(BlueprintNativeEvent)
	void StateShiftReaction_Collision_Enable();
	UFUNCTION(BlueprintNativeEvent)
	void StateShiftReaction_Collision_Disable();
	UFUNCTION(BlueprintNativeEvent)
	void StateShiftReaction_Custom();
};
