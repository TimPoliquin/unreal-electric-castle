#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "CinematicContextHandle.generated.h"

class UCinematicContext;

UCLASS(BlueprintType)
class ELECTRICCASTLE_API UCinematicContextHandle : public UObject
{
	GENERATED_BODY()

public:
	/** Returns true if the underlying context is still valid */
	bool IsValid() const { return Context.IsValid(); }

	/** Access cinematic tags safely */
	FGameplayTagContainer GetCinematicTags() const;

	/** Add a restore function to the underlying context */
	void AddRestoreFunction(const TFunction<void()>& Func) const;
	/** Set and restore **/
	template <typename TObject, typename TValue, typename TRestoreFunc>
	void AddRestoreLambda(TObject* Object, TValue CurrentValue, TRestoreFunc RestoreFunc) const;

	/** convenience tag check */
	bool HasTag(const FGameplayTag& Tag) const;
	/** convenience tag check */
	bool HasAnyTag(const FGameplayTagContainer& Tags) const;
	/** convenience tag check */
	bool HasNoneTag(const FGameplayTagContainer& Tags) const;

	/** Called by CinematicManager when creating the handle */
	void Initialize(UCinematicContext* InContext);

private:
	/** Weak reference so the handle never keeps the context alive */
	UPROPERTY()
	TWeakObjectPtr<UCinematicContext> Context;
};

template <typename TObject, typename TValue, typename TRestoreFunc>
void UCinematicContextHandle::AddRestoreLambda(TObject* Object, TValue CurrentValue, TRestoreFunc RestoreFunc) const
{
	TWeakObjectPtr<TObject> WeakObj = Object;
	AddRestoreFunction([WeakObj, CurrentValue, RestoreFunc]()
	{
		if (WeakObj.IsValid())
		{
			RestoreFunc(WeakObj.Get(), CurrentValue);
		}
		else
		{
			UE_LOG(LogElectricCastle, Error, TEXT("CinematicContextHandle] Unable to restore - Object is invalid!"));
		}
	});
}
