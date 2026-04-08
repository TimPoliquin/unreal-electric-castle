// Copyright Alien Shores


#include "Actor/Block/AnimNotify/ParryBlurConfig.h"

void UParryBlurConfig::Start(const float InDuration)
{
	Duration = InDuration;
	CurrentTime = 0.f;
	FadeOutStartTime = Duration - FadeOutTime;
}

void UParryBlurConfig::Tick(const float InDeltaTime)
{
	CurrentTime += InDeltaTime;
}

float UParryBlurConfig::GetBlurValue() const
{
	if (CurrentTime >= Duration)
	{
		return 0.f;
	}
	if (CurrentTime < FadeInTime && FadeInTime > 0.f)
	{
		return FMath::Lerp(0.f, BlurIntensity, CurrentTime / FadeInTime);
	}
	if (CurrentTime >= Duration - FadeOutTime && FadeOutTime > 0.f)
	{
		return FMath::Lerp(BlurIntensity, 0.f, (CurrentTime - FadeOutStartTime) / FadeOutTime);
	}
	return BlurIntensity;
}
