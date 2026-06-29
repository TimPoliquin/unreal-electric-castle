#include "AI/Perception/AIPerceptionDataTypes.h"

#include "AbilitySystem/ElectricCastleAbilitySystemComponent.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"

bool FAIPerceivedActor::IsValid() const
{
	return PerceivedActor.IsValid();
}

bool FAIPerceivedActor::IsVisible() const
{
	return bIsVisible && ActiveStimuli.Contains(EAIPerceptionStimulusType::Sight);
}

bool FAIPerceivedActor::IsPerceived() const
{
	if (IsVisible())
	{
		return true;
	}
	// if it is not visible, the actor is perceived if it has any other valid stimulus
	for (const EAIPerceptionStimulusType StimulusType : ActiveStimuli)
	{
		if (StimulusType != EAIPerceptionStimulusType::Sight && StimulusType > EAIPerceptionStimulusType::None && StimulusType < EAIPerceptionStimulusType::Count)
		{
			return true;
		}
	}
	return false;
}

bool FAIPerceivedActor::HasAnyStimuli() const
{
	return !ActiveStimuli.IsEmpty();
}

void FAIPerceivedActor::RemoveVisibilityChangedDelegate()
{
	if (!PerceivedActor.IsValid())
	{
		return;
	}
	if (!VisibilityChangedDelegateHandle.IsValid())
	{
		return;
	}
	if (UElectricCastleAbilitySystemComponent* AbilitySystemComponent = UElectricCastleAbilitySystemLibrary::GetAbilitySystemComponent(PerceivedActor.Get()))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UElectricCastleAttributeSet::GetVisibilityAttribute()).Remove(VisibilityChangedDelegateHandle);
		VisibilityChangedDelegateHandle.Reset();
	}
}
