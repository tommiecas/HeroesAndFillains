// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HeroesAndFillains : ModuleRules
{
	public HeroesAndFillains(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "MultiplayerSessions", "OnlineSubsystem", "OnlineSubsystemSteam", "SlateCore", "UMG", "Landscape", "GeometryCollectionEngine", "AIModule", "GameplayAbilities", "CommonInput", "CommonUI", });

		PrivateDependencyModuleNames.AddRange(new string[] { "MotionWarping", "GameplayTags", "GameplayTasks", "NavigationSystem", "AIModule"});

		PublicIncludePaths.AddRange(new string[] { "HeroesAndFillains/Public" });
		PrivateIncludePaths.AddRange(new string[] { "HeroesAndFillains/Private"  });
		


		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}