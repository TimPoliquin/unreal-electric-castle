// Copyright Alien Shores

using UnrealBuildTool;

public class ElectricCastleEditorTarget : TargetRules
{
	public ElectricCastleEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new[] { "ElectricCastle" });
	}
}