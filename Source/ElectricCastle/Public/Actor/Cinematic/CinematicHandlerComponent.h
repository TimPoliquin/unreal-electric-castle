// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicHandlerInterface.h"
#include "CinematicHandlerTypes.h"
#include "GameplayTagContainer.h"
#include "Cinematic/CinematicEvents.h"
#include "Components/ActorComponent.h"
#include "CinematicHandlerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UCinematicHandlerComponent : public UActorComponent, public ICinematicHandlerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCinematicHandlerComponent();
	virtual void InitializeComponent() override;

	/** Start ICinematicHandlerInterface **/
	virtual UCinematicHandlerComponent* GetCinematicHandlerComponent_Implementation() override;
	virtual bool HandleCinematicEvent_Activate_Implementation(const UCinematicContextHandle* CinematicContextHandle) override;
	virtual bool HandleCinematicEvent_Deactivate_Implementation(const UCinematicContextHandle* CinematicContextHandle) override;
	virtual bool HandleCinematicEvent_MoveTo_Implementation(const UCinematicContextHandle* CinematicContextHandle, const FVector& InMoveToLocation) override;
	virtual bool HandleCinematicEvent_OnBegin_Implementation(const UCinematicContextHandle* CinematicContextHandle) override;
	virtual bool HandleCinematicEvent_OnEnd_Implementation(const UCinematicContextHandle* CinematicContextHandle) override;
	virtual bool HandleCinematicEvent_StartTick_Implementation(const UCinematicContextHandle* CinematicContextHandle) override;
	virtual bool HandleCinematicEvent_StopTick_Implementation(const UCinematicContextHandle* CinematicContextHandle) override;
	virtual bool HandleCinematicEvent_Show_Implementation(const UCinematicContextHandle* CinematicContextHandle) override;
	virtual bool HandleCinematicEvent_Hide_Implementation(const UCinematicContextHandle* CinematicContextHandle) override;
	/** End ICinematicHandlerInterface **/

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	ECinematicFilter CinematicFilter;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties",
		meta=(
			Categories="Cinematic.Type",
			EditCondition="CinematicFilter == ECinematicFilter::WithTags || CinematicFilter == ECinematicFilter::IgnoreTags",
			ToolTip="Cinematic Tags to react to")
	)
	FGameplayTagContainer CinematicTags = FGameplayTagContainer::EmptyContainer;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	ECinematicVisibilityChange CinematicVisibilityChange = ECinematicVisibilityChange::DoNothing;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	ECinematicTickChange CinematicTickChange = ECinematicTickChange::DoNothing;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	ECinematicTransformChange CinematicTransformChange = ECinematicTransformChange::DoNothing;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties", meta=(EditCondition="CinematicTransformChange == ECinematicTransformChange::Move"))
	FVector MoveToLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	ECinematicStateChange CinematicStateChange = ECinematicStateChange::DoNothing;

private:
	UFUNCTION()
	void OnCinematicBegin(const FCinematicLifeCycleEventPayload& Payload);
	UFUNCTION()
	void OnCinematicEnd(const FCinematicLifeCycleEventPayload& Payload);

	bool ShouldHandleCinematic(const FCinematicLifeCycleEventPayload& Payload) const;
	void HandleVisibilityChange(const UCinematicContextHandle* CinematicContextHandle);
	void HandleTickChange(const UCinematicContextHandle* CinematicContextHandle);
	void HandleTransformChange(const UCinematicContextHandle* CinematicContextHandle);
	void HandleStateChange(const UCinematicContextHandle* CinematicContextHandle);
};
