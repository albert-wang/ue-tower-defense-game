// Some copyright should be here...

using System.IO;
using UnrealBuildTool;

public class MuxyUnrealPlugin : ModuleRules
{
	public MuxyUnrealPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
				Path.Combine(ModuleDirectory, "Private/ThirdParty")
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
				Path.Combine(ModuleDirectory, "Private/ThirdParty")
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

		System.Console.WriteLine(Path.Combine(ModuleDirectory, "Private/ThirdParty"));

		PublicSystemLibraryPaths.Add(Path.Combine(ModuleDirectory, "Private/ThirdParty"));
		PublicAdditionalLibraries.Add("Experimental.lib");
		PublicAdditionalLibraries.Add("websockets.lib");
		PublicAdditionalLibraries.Add("zlib.lib");
		PublicAdditionalLibraries.Add("libcrypto.lib");
		PublicAdditionalLibraries.Add("libssl.lib");
		PublicAdditionalLibraries.Add("libuv.lib");
		PublicAdditionalLibraries.Add("crypt32.lib");
		PublicAdditionalLibraries.Add("Psapi.lib");
		PublicAdditionalLibraries.Add("Userenv.lib");

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
