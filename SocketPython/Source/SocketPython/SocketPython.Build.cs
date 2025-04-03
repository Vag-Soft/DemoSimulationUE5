// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SocketPython : ModuleRules
{
	public SocketPython(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Networking", "Sockets", "ImageWrapper", "RenderCore", "Renderer", "RHI", "Slate", "SlateCore" });

        PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd" // if you're using editor-only symbols like GEditor
		});	
    }
}
