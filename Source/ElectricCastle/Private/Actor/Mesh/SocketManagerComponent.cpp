// Copyright Alien Shores


#include "Actor/Mesh/SocketManagerComponent.h"

#include "Actor/Mesh/SocketManagerActor.h"
#include "Actor/Mesh/SocketManagerTypes.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
USocketManagerComponent::USocketManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void USocketManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USocketManagerComponent, SocketConfigs);
}

bool USocketManagerComponent::HasSocket(const FGameplayTag& SocketTag) const
{
	return SocketConfigs.ContainsByPredicate([SocketTag](const FSocketMeshConfig& Config)
	{
		return Config.SocketTag == SocketTag;
	});
}

FVector USocketManagerComponent::GetSocketLocation(const FGameplayTag& SocketTag) const
{
	if (const FSocketMeshConfig& SocketConfig = GetSocketMeshConfig(SocketTag); SocketConfig.IsValid())
	{
		return SocketConfig.MeshComponent.Get()->GetSocketLocation(SocketConfig.SocketName);
	}
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Socket [%s] not registered"), *GetOwner()->GetName(), *GetName(), *SocketTag.ToString())
	}
	return FVector::ZeroVector;
}

FTransform USocketManagerComponent::GetSocketTransform(const FGameplayTag& SocketTag) const
{
	if (const FSocketMeshConfig& SocketConfig = GetSocketMeshConfig(SocketTag); SocketConfig.IsValid())
	{
		return SocketConfig.MeshComponent.Get()->GetSocketTransform(SocketConfig.SocketName);
	}
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Socket [%s] not registered"), *GetOwner()->GetName(), *GetName(), *SocketTag.ToString())
	}
	return FTransform();
}

FName USocketManagerComponent::GetSocketName(const FGameplayTag& SocketTag) const
{
	if (const FSocketMeshConfig& SocketConfig = GetSocketMeshConfig(SocketTag); SocketConfig.IsValid())
	{
		return SocketConfig.SocketName;
	}
	return NAME_None;
}

USkeletalMeshComponent* USocketManagerComponent::GetMeshBySocketTag(const FGameplayTag& SocketTag) const
{
	if (const FSocketMeshConfig& SocketConfig = GetSocketMeshConfig(SocketTag); SocketConfig.IsValid())
	{
		return SocketConfig.MeshComponent.Get();
	}
	if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] No socket registered with tag %s"), *GetOwner()->GetName(), *GetName(), *SocketTag.ToString())
	}
	return nullptr;
}

void USocketManagerComponent::RegisterSocket(USkeletalMeshComponent* InMeshComponent, const FGameplayTag SocketTag, const FName SocketName)
{
	RegisterSocketConfig(FSocketMeshConfig(InMeshComponent, SocketTag, SocketName));
}

void USocketManagerComponent::RegisterSocketConfig(const FSocketMeshConfig& SocketConfig)
{
	SocketConfigs.Add(SocketConfig);
}

void USocketManagerComponent::AttachByTag(USceneComponent* InSkeletalMesh, FGameplayTag SocketTag, const EAttachmentRule LocationAttachRule, const EAttachmentRule RotationAttachRule,
                                          const EAttachmentRule ScaleAttachRule) const
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	if (const FSocketMeshConfig& SocketConfig = GetSocketMeshConfig(SocketTag); SocketConfig.IsValid())
	{
		if (SocketConfig.MeshComponent.Get()->DoesSocketExist(SocketConfig.SocketName))
		{
			if (bDebug)
			{
				UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Attaching to socket [%s]"), *GetOwner()->GetName(), *GetName(), *SocketTag.ToString())
			}
			const FAttachmentTransformRules AttachRules(LocationAttachRule, RotationAttachRule, ScaleAttachRule, false);
			InSkeletalMesh->AttachToComponent(SocketConfig.MeshComponent.Get(), AttachRules, SocketConfig.SocketName);
		}
		else if (bDebug)
		{
			UE_LOG(LogElectricCastle, Error, TEXT("[%s:%s] Socket [%s] does not exist on %s!"), *GetOwner()->GetName(), *GetName(), *SocketTag.ToString(), *SocketConfig.MeshComponent->GetName());
		}
	}
	else if (bDebug)
	{
		UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Socket [%s] not registered - attachment failed!"), *GetOwner()->GetName(), *GetName(), *SocketTag.ToString())
	}
}

USocketManagerComponent* USocketManagerComponent::GetSocketManagerComponent(const UObject* Actor)
{
	return ISocketManagerActor::GetSocketManagerComponent(Actor);
}

FSocketMeshConfig USocketManagerComponent::GetSocketMeshConfig(const FGameplayTag& SocketTag) const
{
	const FSocketMeshConfig* SocketConfig = SocketConfigs.FindByPredicate([SocketTag](const FSocketMeshConfig& Config)
	{
		return Config.SocketTag.MatchesTagExact(SocketTag);
	});
	return SocketConfig ? *SocketConfig : FSocketMeshConfig();
}

const USkeletalMeshSocket* USocketManagerComponent::GetSocket(const FGameplayTag& SocketTag) const
{
	if (const FSocketMeshConfig& SocketConfig = GetSocketMeshConfig(SocketTag); SocketConfig.IsValid())
	{
		return SocketConfig.MeshComponent.Get()->GetSocketByName(SocketConfig.SocketName);
	}
	return nullptr;
}
