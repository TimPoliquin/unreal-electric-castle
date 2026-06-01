// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Actor/Significance/SignificanceCalculation.h"
#include "Components/ActorComponent.h"
#include "ActorSignificanceComponent.generated.h"


class USignificanceCalculation;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UActorSignificanceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UActorSignificanceComponent();

	virtual void BeginPlay() override;
	void SetSignificanceTag(const FGameplayTag& InSignificanceTag) { SignificanceTag = InSignificanceTag; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Significance", meta=(Categories="Significance"))
	FGameplayTag SignificanceTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Export, Category="Significance")
	TObjectPtr<USignificanceCalculation> SignificanceCalculation;
};
