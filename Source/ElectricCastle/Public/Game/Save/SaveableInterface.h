// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USaveableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELECTRICCASTLE_API ISaveableInterface
{
	GENERATED_BODY()

public:
	// Called when the object should save its data
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save System")
	TArray<uint8> SaveData();
	virtual TArray<uint8> SaveData_Implementation();

	// Called when the object should load its data
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save System")
	bool LoadData(const TArray<uint8>& Data);
	virtual bool LoadData_Implementation(const TArray<uint8>& Data);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save System")
	void PostLoad();

	// Called to check if this object should be saved
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save System")
	bool ShouldSave() const;
	virtual bool ShouldSave_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save System")
	bool ShouldAutoSpawn() const;
	virtual bool ShouldAutoSpawn_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save System")
	bool ShouldLoadTransform() const;
	virtual bool ShouldLoadTransform_Implementation() const;

	// Get unique identifier for this saveable object
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save System")
	FString GetSaveID() const;
};
