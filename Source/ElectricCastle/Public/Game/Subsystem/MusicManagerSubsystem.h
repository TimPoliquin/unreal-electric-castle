#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MusicManagerSubsystem.generated.h"

class UAudioComponent;
class USoundBase;

/**
 * Simple music manager:
 * - Persistent across levels (GameInstanceSubsystem)
 * - Main (level) music + override (battle) music
 * - Fade in/out
 * - Resume main music at previous playback time after override
 */
UCLASS(Abstract, Blueprintable)
class ELECTRICCASTLE_API UMusicManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UMusicManagerSubsystem* Get(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Controls whether music is played at all **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	bool bEnableMusic = true;

	/** Default fade duration if per-call value is negative */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	float DefaultFadeDuration = 1.0f;


	/** Last saved playback time of the main music (seconds) */
	UPROPERTY(BlueprintReadOnly, Category = "Music")
	float SavedMainPlaybackTime = 0.0f;

	/** World time when main music last started (used to approximate playback time) */
	UPROPERTY(BlueprintReadOnly, Category = "Music")
	float MainMusicStartWorldTime = 0.0f;

	/** Is an override (battle) track currently active? */
	UPROPERTY(BlueprintReadOnly, Category = "Music")
	bool bOverrideActive = false;

	/** Play or restart the level/main music. */
	UFUNCTION(BlueprintCallable, Category = "Music")
	void PlayLevelMusic(USoundBase* LevelMusic, float FadeInDuration = -1.0f, float StartTime = 0.0f);

	/**
	 * Start override music (e.g., battle).
	 * Saves main playback time, fades main out, fades override in.
	 */
	UFUNCTION(BlueprintCallable, Category = "Music")
	void StartOverrideMusic(USoundBase* OverrideMusic, float FadeOutMain = -1.0f, float FadeInOverride = -1.0f);

	/**
	 * End override music.
	 * Fades override out, fades main back in at SavedMainPlaybackTime.
	 */
	UFUNCTION(BlueprintCallable, Category = "Music")
	void EndOverrideMusic(float FadeOutOverride = -1.0f, float FadeInMain = -1.0f);

	/** Stop all music (both tracks), with optional fade. */
	UFUNCTION(BlueprintCallable, Category = "Music")
	void StopAllMusic(float FadeOutTime = -1.0f);

protected:
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> MainMusicComponent;

	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> OverrideMusicComponent;

	void EnsureAudioComponents();
	float ResolveFade(float InFade) const;

	/** Approximate current playback time of main music, based on world time and sound duration. */
	float GetApproxMainPlaybackTime() const;
};
