// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HighlightActorInterface.generated.h"

class UHighlightComponent;
// This class does not need to be modified.
UINTERFACE()
class UHighlightActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API IHighlightActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UHighlightComponent* GetHighlightComponent() const;
	UFUNCTION(BlueprintNativeEvent)
	void GetHighlightMeshes(TArray<UMeshComponent*>& OutHighlightMeshes);

	static bool IsHighlightActor(const UObject* Object);
	static UHighlightComponent* GetHighlightComponent(const UObject* Actor);
	static void Highlight(const UObject* Actor);
	static void Unhighlight(const UObject* Actor);
};
