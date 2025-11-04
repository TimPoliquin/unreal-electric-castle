// Copyright Alien Shores


#include "Item/Component/LockComponent.h"

#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "LevelAssets/Switch/SwitchInterface.h"
#include "Player/InventoryComponent.h"


class UInventoryComponent;

ELockSFXPlaybackLocation FLockSFXConfig::GetPlaybackLocation(const ELockSFXPlaybackLocation OverridePlaybackLocation) const
{
	switch (PlaybackLocation)
	{
	case ELockSFXPlaybackLocation::Default:
		return PlaybackLocation;
	default:
		return OverridePlaybackLocation;
	}
}

void FLockSFXConfig::AutoPlay(const AActor* WorldContextObject) const
{
	if (bAutoPlay)
	{
		Play(WorldContextObject);
	}
}

void FLockSFXConfig::Play(const AActor* WorldContextObject, const ELockSFXPlaybackLocation OverridePlaybackLocation) const
{
	FLoadSoftObjectPathAsyncDelegate OnLoadCompletedDelegate;
	OnLoadCompletedDelegate.BindLambda([this, OverridePlaybackLocation, WorldContextObject](FSoftObjectPath ObjectPath, UObject* LoadedSound)
	{
		switch (GetPlaybackLocation(OverridePlaybackLocation))
		{
		case ELockSFXPlaybackLocation::Default:
		case ELockSFXPlaybackLocation::UI:
			UGameplayStatics::PlaySound2D(WorldContextObject, SoundEffect.Get());
			break;
		case ELockSFXPlaybackLocation::ActorLocation:
			UGameplayStatics::PlaySoundAtLocation(WorldContextObject, SoundEffect.Get(), WorldContextObject->GetActorLocation(), WorldContextObject->GetActorRotation());
			break;
		}
	});
	SoundEffect.LoadAsync(OnLoadCompletedDelegate);
}

ULockComponent::ULockComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CompletedSound.PlaybackLocation = ELockSFXPlaybackLocation::ActorLocation;
}

bool ULockComponent::IsPreconditionMet(const AActor* Player) const
{
	switch (UnlockMode)
	{
	case EUnlockMode::Unlocked:
		return true;
	case EUnlockMode::Key:
		return IsPreconditionMet_Key(Player);
	case EUnlockMode::Switch:
		return IsPreconditionMet_Switch();
	case EUnlockMode::Custom:
		return IsPreconditionMet_Custom();
	default:
		return false;
	}
}

bool ULockComponent::TryUnlock(AActor* Player)
{
	if (IsUnlocked())
	{
		return false;
	}
	switch (UnlockMode)
	{
	case EUnlockMode::Unlocked:
		Unlock();
		break;
	case EUnlockMode::Key:
		TryUnlock_Key(Player);
		break;
	case EUnlockMode::Switch:
		TryUnlock_Switch();
		break;
	case EUnlockMode::Custom:
		TryUnlock_Custom(Player);
		break;
	}
	return IsUnlocked();
}

bool ULockComponent::IsUnlocked() const
{
	return UnlockMode == EUnlockMode::Unlocked || bUnlocked;
}

bool ULockComponent::IsLocked() const
{
	return UnlockMode != EUnlockMode::Unlocked && !bUnlocked;
}

bool ULockComponent::IsManuallyUnlockable() const
{
	return UnlockMode == EUnlockMode::Key;
}

FString ULockComponent::GetUnlockText() const
{
	return IsManuallyUnlockable() && IsLocked() ? UnlockText : OpenText;
}

bool ULockComponent::IsUnlockedAlways() const
{
	return UnlockMode == EUnlockMode::Unlocked;
}

bool ULockComponent::IsUnlockedByKey() const
{
	return UnlockMode == EUnlockMode::Key;
}

bool ULockComponent::IsUnlockedBySwitch() const
{
	return UnlockMode == EUnlockMode::Switch;
}

bool ULockComponent::IsUnlockedByCustomLogic() const
{
	return UnlockMode == EUnlockMode::Custom;
}

void ULockComponent::SetUnlockMode(const EUnlockMode InUnlockMode)
{
	UnlockMode = InUnlockMode;
}

void ULockComponent::BeginPlay()
{
	Super::BeginPlay();
	switch (UnlockMode)
	{
	case EUnlockMode::Unlocked:
		break;
	case EUnlockMode::Key:
		InitializeUnlock_Key();
		break;
	case EUnlockMode::Switch:
		InitializeUnlock_Switch();
		break;
	case EUnlockMode::Custom:
		InitializeUnlock_Custom();
		break;
	}
}

void ULockComponent::InitializeUnlock_Key_Implementation()
{
	if (!KeyTag.IsValid())
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Configured with an invalid key"), *GetName());
		return;
	}
	if (UElectricCastleGameDataSubsystem::Get(this)->FindItemDefinitionByItemTag(KeyTag).IsValid())
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Configured key is missing an item definition: %s"), *GetName(), *KeyTag.GetTagName().ToString());
	}
}

void ULockComponent::InitializeUnlock_Switch_Implementation()
{
	if (Switches.Num() <= 0)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s] No switches configured!"), *GetName());
		return;
	}
	for (AActor* SwitchActor : Switches)
	{
		if (!IsValid(SwitchActor))
		{
			continue;
		}
		if (ISwitchInterface* SwitchInterface = Cast<ISwitchInterface>(SwitchActor))
		{
			SwitchInterface->GetOnSwitchActivatedDelegate().AddUniqueDynamic(this, &ULockComponent::OnSwitchActivated);
		}
		else
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s] Configured switch does not implement ISwitchInterface: %s"), *GetName(), *SwitchActor->GetName());
		}
	}
}

void ULockComponent::InitializeUnlock_Custom_Implementation()
{
	// Do nothing by default here
}

bool ULockComponent::IsPreconditionMet_Custom_Implementation() const
{
	return false;
}

bool ULockComponent::IsPreconditionMet_Key_Implementation(const AActor* Player) const
{
	if (const UInventoryComponent* InventoryComponent = UInventoryComponent::Get(Player))
	{
		return InventoryComponent->HasItemInInventory(KeyTag);
	}
	return false;
}

bool ULockComponent::IsPreconditionMet_Switch_Implementation() const
{
	for (const AActor* SwitchActor : Switches)
	{
		if (!ISwitchInterface::IsSwitchActive(SwitchActor))
		{
			return false;
		}
	}
	return true;
}

void ULockComponent::Unlock_Implementation(bool bBroadcast)
{
	bUnlocked = true;
	if (bBroadcast)
	{
		OnUnlockDelegate.Broadcast(FOnLockComponentUnlockPayload(GetOwner(), this, UnlockMode));
	}
}

void ULockComponent::TryUnlock_Key_Implementation(AActor* Player)
{
	bool bShouldUnlock = false;
	if (UInventoryComponent* InventoryComponent = UInventoryComponent::Get(Player))
	{
		if (bConsumesKey)
		{
			bShouldUnlock = InventoryComponent->UseKey(KeyTag);
		}
		else
		{
			bShouldUnlock = InventoryComponent->HasItemInInventory(KeyTag);
		}
	}
	if (bShouldUnlock)
	{
		Unlock();
	}
}

void ULockComponent::TryUnlock_Switch_Implementation()
{
	bool bAllActive = true;
	for (const AActor* SwitchActor : Switches)
	{
		if (!ISwitchInterface::IsSwitchActive(SwitchActor))
		{
			bAllActive = false;
			break;
		}
	}
	if (bAllActive)
	{
		Unlock();
	}
}

void ULockComponent::TryUnlock_Custom_Implementation(AActor* Player)
{
	// by default, do nothing
}

void ULockComponent::PlaySuccessSound(const ELockSFXPlaybackLocation PlaybackLocation)
{
	SuccessSound.Play(GetOwner(), PlaybackLocation);
}

void ULockComponent::PlayResetSound(const ELockSFXPlaybackLocation PlaybackLocation)
{
	ResetSound.Play(GetOwner(), PlaybackLocation);
}

void ULockComponent::PlayCompletedSound(const ELockSFXPlaybackLocation PlaybackLocation)
{
	CompletedSound.Play(GetOwner(), PlaybackLocation);
}

void ULockComponent::OnSwitchActivated(const FOnSwitchStatusChangedPayload& Payload)
{
	bool bAllActivated = true;
	bool PayloadSwitchFound = false;
	bool bTriggerReset = false;
	int32 UnlockedCount = 0;
	for (AActor* Switch : Switches)
	{
		if (Payload.Switch == Switch)
		{
			PayloadSwitchFound = true;
		}
		if (!ISwitchInterface::IsSwitchActive(Switch))
		{
			bAllActivated = false;
			if (bOrdered && !PayloadSwitchFound)
			{
				bTriggerReset = true;
				break;
			}
		}
		else
		{
			UnlockedCount++;
		}
	}
	if (bTriggerReset)
	{
		ResetSound.AutoPlay(Payload.Switch);
		OnResetDelegate.Broadcast();
	}
	else
	{
		SuccessSound.AutoPlay(Payload.Switch);
		OnUnlockProgressDelegate.Broadcast(FOnLockComponentUnlockProgressPayload(GetOwner(), this, UnlockedCount, Switches.Num()));
	}
	if (bAllActivated)
	{
		CompletedSound.AutoPlay(GetOwner());
		TryUnlock_Switch();
	}
	else if (bTriggerReset)
	{
		for (AActor* Switch : Switches)
		{
			ISwitchInterface::ResetSwitch(Switch);
		}
	}
}
