// Copyright Alien Shores

using UnrealBuildTool;

public class ElectricCastle : ModuleRules
{
	public ElectricCastle(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		bEnableExceptions = true;
		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "CommonInput", "GameplayAbilities", "UMG", "SlateCore",
			"ModelViewViewModel", "CableComponent", "AIModule", "MotionWarping"
		});

		PrivateDependencyModuleNames.AddRange(new[]
			{ "GameplayTags", "GameplayTasks", "NavigationSystem", "Niagara", "AIModule" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}