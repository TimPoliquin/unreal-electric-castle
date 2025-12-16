// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "BeamActorTypes.h"
#include "GameplayEffect.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "BeamActor.generated.h"

class UGameplayEffect;
class UNiagaraComponent;

UCLASS(Abstract)
class ELECTRICCASTLE_API ABeamActor : public AActor
{
	GENERATED_BODY()

public:
	ABeamActor();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	void SetLinearCascadeParams(const FBeamCascadeLinearParams& InParams);
	void SetNoCascadeParams();
	void SetTraceParams(const float InTraceDistance, const float InTraceRadius, const ECollisionChannel InTraceChannel, const bool bInDebug);
	void SetTraceOrigin(AActor* InTraceOrigin);
	void SetIgnoreActors(const TArray<AActor*>& InActors);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Terminate();

protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Update();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TraceDestination(FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateBeamDestination(const FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateBeamTargetEffect(const FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FActiveGameplayEffectHandle ApplyBeamTargetEffect(AActor* Target);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SpawnChildBeams(const FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SpawnChildBeams_Linear(const FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SpawnChildBeam(const FHitResult& OriginHit, const FVector& DestinationHit);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InitializeChildBeamProperties(ABeamActor* ChildBeam, const FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateChildBeams(const FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TerminateChildBeams();

	// Helper functions
	void UpdateChildBeam_Linear(ABeamActor* ChildBeam, const FHitResult& ParentHitResult);
	FVector CalculateReflectedDirection(const FHitResult& HitResult) const;
	void DrawReflectionDebug(const FHitResult& HitResult, const FVector& ReflectedDir) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UNiagaraComponent> BeamComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAudioComponent> SFXComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category="Properties")
	EBeamCascadeType CascadeType = EBeamCascadeType::None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category="Properties", meta=(ClampMin = "0", UIMin = "0", EditCondition="CascadeType == EBeamCascadeType::Linear", EditConditionHides))
	FBeamCascadeLinearParams LinearCascadeParams;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category="Properties")
	TSubclassOf<UGameplayEffect> ApplyEffectToTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Properties", meta=(EditCondition="ApplyEffectToTarget != nullptr", EditConditionHides))
	int32 EffectLevel = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Properties|Trace")
	float TraceDistance = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Properties|Trace")
	float TraceRadius = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Properties|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties|Trace")
	bool bDebug = false;

	UPROPERTY(BlueprintReadOnly, Replicated)
	TObjectPtr<AActor> TraceOrigin;
	UPROPERTY(Replicated)
	TArray<AActor*> IgnoreActors;
	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<TObjectPtr<ABeamActor>> ChildBeams;
	UPROPERTY(BlueprintReadOnly)
	FHitResult LastTraceHitResult;
	UPROPERTY(BlueprintReadOnly)
	FActiveGameplayEffectHandle TargetEffectHandle;

private:
	bool ShouldSpawnChildBeamsFromTarget(const AActor* Target) const;
};
