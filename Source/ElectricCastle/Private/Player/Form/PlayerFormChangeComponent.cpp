// Copyright Alien Shores


#include "Player/Form/PlayerFormChangeComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Game/Subsystem/ElectricCastleGameDataSubsystem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Form/PlayerFormConfig.h"

UPlayerFormChangeComponent::UPlayerFormChangeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerFormChangeComponent::ChangeForm_Async(const FGameplayTag& FormTag)
{
	if (FormTag.MatchesTagExact(CurrentForm))
	{
		return;
	}
	const FPlayerFormConfigRow& Row = GetPlayerFormConfigRow(FormTag);
	if (!Row.IsValid())
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Failed to change form -- invalid game data or form config! %s"), *GetOwner()->GetName(), *GetName(), *FormTag.ToString());
		return;
	}
	const FGameplayTag OldFormTag = CurrentForm;
	FLoadSoftObjectPathAsyncDelegate FormLoad;
	FormLoad.BindLambda([this, FormTag, OldFormTag](FSoftObjectPath ObjectPath, UObject* Loaded)
	{
		const FPlayerFormConfigRow& Row = GetPlayerFormConfigRow(FormTag);
		if (Row.IsLoaded())
		{
			FPlayerFormChangeEventPayload EventPayload;
			EventPayload.OldFormTag = OldFormTag;
			EventPayload.NewFormTag = FormTag;
			EventPayload.CharacterMesh = Row.CharacterMesh.Get();
			EventPayload.AnimationBlueprintClass = Row.AnimationBlueprint.Get();
			EventPayload.PortraitImage = Row.PortraitImage.Get();
			CurrentForm = FormTag;
			OnPlayerFormChange.Broadcast(EventPayload);
		}
	});
	if (!Row.CharacterMesh.IsNull())
	{
		Row.CharacterMesh.LoadAsync(FormLoad);
	}
	if (!Row.AnimationBlueprint.IsNull())
	{
		Row.AnimationBlueprint.LoadAsync(FormLoad);
	}
	if (!Row.PortraitImage.IsNull())
	{
		Row.PortraitImage.LoadAsync(FormLoad);
	}
}

void UPlayerFormChangeComponent::FormChange_PlayEffect(const FPlayerFormChangeEventPayload& Payload)
{
	if (FormChangeEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			FormChangeEffect,
			GetOwner()->GetRootComponent(),
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::Type::SnapToTarget,
			true,
			true
		);
	}
	if (FormChangeSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			FormChangeSound,
			GetOwner()->GetActorLocation(),
			GetOwner()->GetActorRotation()
		);
	}
}

void UPlayerFormChangeComponent::FormChange_UpdateCharacterMesh(const FPlayerFormChangeEventPayload& Payload)
{
	GetMesh()->SetSkeletalMesh(Payload.CharacterMesh);
	GetMesh()->SetAnimInstanceClass(Payload.AnimationBlueprintClass);
}


// Called when the game starts
void UPlayerFormChangeComponent::BeginPlay()
{
	Super::BeginPlay();
	CharacterMeshComponent = Cast<ACharacter>(GetOwner())->GetMesh();
}

USkeletalMeshComponent* UPlayerFormChangeComponent::GetMesh() const
{
	if (!CharacterMeshComponent.IsExplicitlyNull())
	{
		return CharacterMeshComponent.Get();
	}
	return nullptr;
}

FPlayerFormConfigRow UPlayerFormChangeComponent::GetPlayerFormConfigRow(const FGameplayTag& FormTag) const
{
	if (const UElectricCastleGameDataSubsystem* GameData = UElectricCastleGameDataSubsystem::Get(this))
	{
		if (const UPlayerFormConfig* FormConfig = GameData->GetPlayerFormConfig())
		{
			return FormConfig->GetPlayerFormConfigRowByTag(FormTag);
		}
	}
	return FPlayerFormConfigRow(EPlayerForm::None);
}
