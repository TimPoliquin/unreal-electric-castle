// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ElectricCastle/ElectricCastle.h"
#include "HighlightComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELECTRICCASTLE_API UHighlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHighlightComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	void AddHighlightMesh(UMeshComponent* Mesh);
	void RemoveHighlightMesh(UMeshComponent* Mesh);
	void Highlight();
	void Unhighlight();
	void SetCustomDepthStencilValue(const uint8 Value) { HighlightCustomDepthStencilValue = Value; }
	bool IsHighlighted() const { return bIsHighlighted; }
	bool IsHighlightable() const { return bHighlightable; }
	void SetHighlightable(const bool bNewHighlightable);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Highlight")
	bool bIsHighlighted = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Highlight")
	uint8 HighlightCustomDepthStencilValue = CUSTOM_DEPTH_BLUE;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Highlight", SaveGame, Replicated)
	bool bHighlightable = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Highlight")
	bool bDebug = false;

private:
	void HighlightMesh(UMeshComponent* Mesh) const;
	void UnhighlightMesh(UMeshComponent* Mesh) const;

	UPROPERTY(VisibleAnywhere, Category="Highlight")
	TArray<TWeakObjectPtr<UMeshComponent>> HighlightMeshes;
};
