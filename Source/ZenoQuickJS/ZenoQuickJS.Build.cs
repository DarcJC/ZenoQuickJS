// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ZenoQuickJS : ModuleRules
{
	public ZenoQuickJS(ReadOnlyTargetRules Target) : base(Target)
	{
		CStandard = CStandardVersion.Latest;
		CppStandard = CppStandardVersion.Latest;
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		var platformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "windows-x86_64" : null;
		if (platformString == null)
		{
			 throw new System.Exception("Unsupported platform");
		}
		var quickJsBasePath = Path.Combine(PluginDirectory, "ThirdParty/quickjs/library", platformString);
		var includePath = Path.Combine(PluginDirectory, "ThirdParty/quickjs/include"); 
		PublicIncludePaths.Add(includePath);
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			 var libPath = Path.Combine(quickJsBasePath, "libquickjs-dyn.a");
			 PublicAdditionalLibraries.Add(libPath);

			 var dllFiles = Directory.GetFiles(quickJsBasePath, "*.dll");
			 foreach (var dllFile in dllFiles)
			 {
				  RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", Path.GetFileName(dllFile)), dllFile);
			 }
		}
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
