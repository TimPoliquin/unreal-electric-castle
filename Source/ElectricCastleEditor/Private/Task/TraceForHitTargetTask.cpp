// Copyright Alien Shores


#include "Task/TraceForHitTargetTask.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_CallFunction.h"
#include "K2Node_IfThenElse.h"
#include "KismetCompiler.h"
#include "AbilitySystem/ElectricCastleAbilitySystemLibrary.h"
#include "AbilitySystem/Utils/TraceParams.h"

void UTraceForHitTargetTask::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	if (const UClass* ActionKey = GetClass(); ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UTraceForHitTargetTask::AllocateDefaultPins()
{
	// Input exec
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, TEXT("In"));

	// Input data: Actor
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, AActor::StaticClass(), TEXT("Player"));
	// Input data: Range
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FSphereTraceParams::StaticStruct(), TEXT("SweepParams"));

	// Output exec: Valid
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, TEXT("Hit"));

	// Output exec: Invalid
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, TEXT("NoHit"));

	// Output data: Actor (forwarded)
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object, AActor::StaticClass(), TEXT("HitActor"));
}

void UTraceForHitTargetTask::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	// Grab pins
	UEdGraphPin* ExecPin = GetExecPin();
	UEdGraphPin* PlayerPin = FindPinChecked(TEXT("Player"));
	UEdGraphPin* SweepParamsPin = FindPinChecked(TEXT("SweepParams"));
	UEdGraphPin* HitExecPin = FindPinChecked(TEXT("Hit"));
	UEdGraphPin* NoHitExecPin = FindPinChecked(TEXT("NoHit"));
	UEdGraphPin* HitActorPin = FindPinChecked(TEXT("HitActor"));

	// Spawn intermediate node: call FindHitBySphereTrace
	UK2Node_CallFunction* CallTraceNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallTraceNode->SetFromFunction(UElectricCastleAbilitySystemLibrary::StaticClass()->FindFunctionByName(TEXT("FindHitBySphereTrace")));
	CallTraceNode->AllocateDefaultPins();

	// Wire inputs
	CompilerContext.MovePinLinksToIntermediate(*PlayerPin, *CallTraceNode->FindPinChecked(TEXT("Player")));
	CompilerContext.MovePinLinksToIntermediate(*SweepParamsPin, *CallTraceNode->FindPinChecked(TEXT("SweepParams")));

	// Branch node
	UK2Node_IfThenElse* BranchNode = CompilerContext.SpawnIntermediateNode<UK2Node_IfThenElse>(this, SourceGraph);
	BranchNode->AllocateDefaultPins();

	// Wire exec flow
	CompilerContext.MovePinLinksToIntermediate(*ExecPin, *CallTraceNode->GetExecPin());
	CallTraceNode->GetThenPin()->MakeLinkTo(BranchNode->GetExecPin());

	// Condition: return value != nullptr
	UEdGraphPin* ReturnPin = CallTraceNode->GetReturnValuePin();
	CompilerContext.MovePinLinksToIntermediate(*ReturnPin, *BranchNode->GetConditionPin());

	// Route outputs
	CompilerContext.MovePinLinksToIntermediate(*HitExecPin, *BranchNode->GetThenPin());
	CompilerContext.MovePinLinksToIntermediate(*NoHitExecPin, *BranchNode->GetElsePin());
	CompilerContext.MovePinLinksToIntermediate(*HitActorPin, *ReturnPin);
}
