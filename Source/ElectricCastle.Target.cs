// Copyright Alien Shores

using UnrealBuildTool;

public class ElectricCastleTarget : TargetRules
{
	public ElectricCastleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange(new[] { "ElectricCastle" });
	}
}