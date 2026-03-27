// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "CinematicHandlerInterface.h"
#include "Cinematic/CinematicEvents.h"
#include "Components/ActorComponent.h"
#include "CinematicHandlerComponent.generated.h"


class UCinematicManager;
class UCinematicEventAction;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UCinematicHandlerComponent : public UActorComponent, public ICinematicHandlerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCinematicHandlerComponent();
	virtual void Cleanup();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginDestroy() override;

	void AddCinematicAction(UCinematicEventAction* Action);

	/** Start ICinematicHandlerInterface **/
	virtual UCinematicHandlerComponent* GetCinematicHandlerComponent_Implementation() override;
	/** End ICinematicHandlerInterface **/

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Properties")
	bool bDebug = false;
	UPROPERTY(EditAnywhere, Instanced, Category="Properties")
	TArray<TObjectPtr<UCinematicEventAction>> CinematicActions;

private:
	UFUNCTION()
	void OnCinematicBegin(const FCinematicLifeCycleEventPayload& Payload);
	UPROPERTY()
	TWeakObjectPtr<UCinematicManager> CinematicManager;
};
