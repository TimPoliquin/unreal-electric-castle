using UnrealBuildTool;

public class ElectricCastleEditor : ModuleRules
{
	public ElectricCastleEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Core dependencies
		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"UnrealEd", // editor utilities
				"AssetRegistry",
				"HairStrandsCore", // groom support
				"SkeletalMeshUtilitiesCommon",
				"ToolMenus"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"Slate",
				"SlateCore",
				"EditorFramework",
				"UnrealEd",
				"BlueprintGraph",
				"KismetCompiler", "ElectricCastle"
			}
		);
	}
}