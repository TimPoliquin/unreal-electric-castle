// MyCustomTaskNode.h
#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "TraceForHitTargetTask.generated.h"

UCLASS()
class UTraceForHitTargetTask : public UK2Node
{
	GENERATED_BODY()

public:
	// Node title
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override
	{
		return FText::FromString(TEXT("Trace For Hit Target"));
	}

	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

	// Allocate pins (inputs/outputs)
	virtual void AllocateDefaultPins() override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

	// Node category
	virtual FText GetMenuCategory() const override
	{
		return FText::FromString(TEXT("Custom|Trace"));
	}
};
