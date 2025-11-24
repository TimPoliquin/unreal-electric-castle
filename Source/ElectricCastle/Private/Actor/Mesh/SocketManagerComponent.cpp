// Copyright Alien Shores


#include "Actor/Mesh/SocketManagerComponent.h"

#include "Actor/Mesh/SocketManagerActor.h"
#include "Actor/Mesh/SocketManagerTypes.h"
#include "ElectricCastle/ElectricCastleLogChannels.h"
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
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Socket [%s] not registered"), *GetOwner()->GetName(), *GetName(), *SocketTag.ToString())
	return FVector::ZeroVector;
}

FTransform USocketManagerComponent::GetSocketTransform(const FGameplayTag& SocketTag) const
{
	if (const FSocketMeshConfig& SocketConfig = GetSocketMeshConfig(SocketTag); SocketConfig.IsValid())
	{
		return SocketConfig.MeshComponent.Get()->GetSocketTransform(SocketConfig.SocketName);
	}
	UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Socket [%s] not registered"), *GetOwner()->GetName(), *GetName(), *SocketTag.ToString())
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

void USocketManagerComponent::AttachByTag(USkeletalMeshComponent* InSkeletalMesh, const FGameplayTag SocketTag) const
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	if (const FSocketMeshConfig& SocketConfig = GetSocketMeshConfig(SocketTag); SocketConfig.IsValid())
	{
		if (SocketConfig.MeshComponent.Get()->DoesSocketExist(SocketConfig.SocketName))
		{
			UE_LOG(LogElectricCastle, Warning, TEXT("[%s:%s] Attaching to socket [%s]"), *GetOwner()->GetName(), *GetName(), *SocketTag.ToString())
			InSkeletalMesh->AttachToComponent(SocketConfig.MeshComponent.Get(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketConfig.SocketName);
		}
		else
		{
			UE_LOG(LogElectricCastle, Error, TEXT("[%s:%s] Socket [%s] does not exist on %s!"), *GetOwner()->GetName(), *GetName(), *SocketTag.ToString(), *SocketConfig.MeshComponent->GetName());
		}
	}
	else
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
		return Config.SocketTag == SocketTag;
	});
	return SocketConfig ? *SocketConfig : FSocketMeshConfig();
}
