// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "BeamActor.generated.h"

class UGameplayEffect;
class UNiagaraComponent;

UENUM(BlueprintType)
enum class EBeamCascadeType : uint8
{
	None,
	Linear,
	Web
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FBeamCascadeLinearParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistance = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCascades = 0;
};

USTRUCT(BlueprintType)
struct ELECTRICCASTLE_API FBeamCascadeWebParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistance = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCascades = 0;
};

UCLASS(Abstract)
class ELECTRICCASTLE_API ABeamActor : public AActor
{
	GENERATED_BODY()

public:
	ABeamActor();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	void SetLinearCascadeParams(const FBeamCascadeLinearParams& InParams);
	void SetWebCascadeParams(const FBeamCascadeWebParams& InParams);
	void SetNoCascadeParams();
	void SetTraceParams(const float InTraceDistance, const float InTraceRadius, const ECollisionChannel InTraceChannel, const bool bInDebug);
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
	void SpawnChildBeams(const FHitResult& HitResult);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TerminateChildBeams();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UNiagaraComponent> BeamComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAudioComponent> SFXComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category="Properties")
	EBeamCascadeType CascadeType = EBeamCascadeType::None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category="Properties", meta=(ClampMin = "0", UIMin = "0", EditCondition="CascadeType == EBeamCascadeType::Linear", EditConditionHides))
	FBeamCascadeLinearParams LinearCascadeParams;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category="Properties", meta=(ClampMin = "0", UIMin = "0", EditCondition="CascadeType == EBeamCascadeType::Web", EditConditionHides))
	FBeamCascadeWebParams WebCascadeParams;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Properties")
	TSubclassOf<UGameplayEffect> ApplyEffectToTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Properties", meta=(EditCondition="ApplyEffectToTarget != nullptr", EditConditionHides))
	int32 EffectLevel = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties|Trace")
	float TraceDistance = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties|Trace")
	float TraceRadius = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Properties|Trace")
	bool bDebug = false;

	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<ABeamActor>> ChildBeams;
	UPROPERTY(BlueprintReadOnly)
	FHitResult LastTraceHitResult;
	UPROPERTY(BlueprintReadOnly)
	FActiveGameplayEffectHandle TargetEffectHandle;
};
