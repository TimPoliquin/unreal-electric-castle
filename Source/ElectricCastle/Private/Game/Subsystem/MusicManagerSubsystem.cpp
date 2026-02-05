#include "Game/Subsystem/MusicManagerSubsystem.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UMusicManagerSubsystem* UMusicManagerSubsystem::Get(const UObject* WorldContextObject)
{
	return UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UMusicManagerSubsystem>();
}

void UMusicManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EnsureAudioComponents();
}

void UMusicManagerSubsystem::Deinitialize()
{
	if (MainMusicComponent)
	{
		MainMusicComponent->Stop();
		MainMusicComponent->DestroyComponent();
		MainMusicComponent = nullptr;
	}

	if (OverrideMusicComponent)
	{
		OverrideMusicComponent->Stop();
		OverrideMusicComponent->DestroyComponent();
		OverrideMusicComponent = nullptr;
	}

	Super::Deinitialize();
}

void UMusicManagerSubsystem::EnsureAudioComponents()
{
	if (!GetWorld())
	{
		return;
	}

	if (!MainMusicComponent)
	{
		MainMusicComponent = NewObject<UAudioComponent>(GetWorld());
		if (MainMusicComponent)
		{
			MainMusicComponent->bAutoActivate = false;
			MainMusicComponent->bIsUISound = false;
			MainMusicComponent->bAllowSpatialization = false;
		}
	}

	if (!OverrideMusicComponent)
	{
		OverrideMusicComponent = NewObject<UAudioComponent>(GetWorld());
		if (OverrideMusicComponent)
		{
			OverrideMusicComponent->bAutoActivate = false;
			OverrideMusicComponent->bIsUISound = false;
			OverrideMusicComponent->bAllowSpatialization = false;
		}
	}
}

float UMusicManagerSubsystem::ResolveFade(float InFade) const
{
	return (InFade >= 0.0f) ? InFade : DefaultFadeDuration;
}

float UMusicManagerSubsystem::GetApproxMainPlaybackTime() const
{
	if (!MainMusicComponent || !GetWorld())
	{
		return 0.0f;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	const float Elapsed = FMath::Max(0.0f, Now - MainMusicStartWorldTime);

	const USoundBase* Sound = MainMusicComponent->Sound;
	if (!Sound)
	{
		return Elapsed;
	}

	const float Duration = Sound->GetDuration();
	if (Duration <= 0.0f)
	{
		return Elapsed;
	}

	// If the sound is looping, wrap around duration; otherwise clamp.
	const float Wrapped = FMath::Fmod(Elapsed, Duration);
	return FMath::Clamp(Wrapped, 0.0f, Duration);
}

void UMusicManagerSubsystem::PlayLevelMusic(USoundBase* LevelMusic, float FadeInDuration, float StartTime)
{
	if (!LevelMusic || !bEnableMusic)
	{
		return;
	}

	EnsureAudioComponents();
	if (!MainMusicComponent || !GetWorld())
	{
		return;
	}

	MainMusicComponent->SetSound(LevelMusic);

	const float Fade = ResolveFade(FadeInDuration);

	// Stop any current playback to avoid overlapping
	MainMusicComponent->Stop();

	// Track when we started, so we can approximate playback time later
	MainMusicStartWorldTime = GetWorld()->GetTimeSeconds() - StartTime;

	// FadeIn has a StartTime parameter, so we can start from an offset if desired
	MainMusicComponent->FadeIn(Fade, 1.0f, StartTime);
}

void UMusicManagerSubsystem::StartOverrideMusic(USoundBase* OverrideMusic, float FadeOutMain, float FadeInOverride)
{
	if (!OverrideMusic || !bEnableMusic)
	{
		return;
	}

	EnsureAudioComponents();
	if (!MainMusicComponent || !OverrideMusicComponent)
	{
		return;
	}

	// Approximate current main playback time so we can resume later
	SavedMainPlaybackTime = GetApproxMainPlaybackTime();

	// Fade out main
	const float OutMain = ResolveFade(FadeOutMain);
	MainMusicComponent->FadeOut(OutMain, 0.0f);

	// Set and fade in override
	OverrideMusicComponent->SetSound(OverrideMusic);
	const float InOverride = ResolveFade(FadeInOverride);

	OverrideMusicComponent->Stop();
	OverrideMusicComponent->FadeIn(InOverride, 1.0f, 0.0f);

	bOverrideActive = true;
}

void UMusicManagerSubsystem::EndOverrideMusic(float FadeOutOverride, float FadeInMain)
{
	EnsureAudioComponents();
	if (!MainMusicComponent || !OverrideMusicComponent || !GetWorld() || !bEnableMusic)
	{
		return;
	}

	// Fade out override
	const float OutOverride = ResolveFade(FadeOutOverride);
	OverrideMusicComponent->FadeOut(OutOverride, 0.0f);

	// Fade main back in at saved playback time
	if (MainMusicComponent->Sound)
	{
		const float InMain = ResolveFade(FadeInMain);

		MainMusicComponent->Stop();

		// Update world-start time so our approximation stays consistent
		MainMusicStartWorldTime = GetWorld()->GetTimeSeconds() - SavedMainPlaybackTime;

		MainMusicComponent->FadeIn(InMain, 1.0f, SavedMainPlaybackTime);
	}

	bOverrideActive = false;
}

void UMusicManagerSubsystem::StopAllMusic(float FadeOutTime)
{
	EnsureAudioComponents();
	if (!MainMusicComponent && !OverrideMusicComponent)
	{
		return;
	}

	const float Fade = ResolveFade(FadeOutTime);

	if (MainMusicComponent)
	{
		MainMusicComponent->FadeOut(Fade, 0.0f);
	}

	if (OverrideMusicComponent)
	{
		OverrideMusicComponent->FadeOut(Fade, 0.0f);
	}

	bOverrideActive = false;
}
