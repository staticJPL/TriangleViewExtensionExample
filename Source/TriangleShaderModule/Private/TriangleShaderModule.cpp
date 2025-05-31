#include "TriangleShaderModule.h"

void FTriangleShaderModule::StartupModule()
{
	FString PluginBaseDirectory = IPluginManager::Get().FindPlugin(TEXT("TriangleShaderModule"))->GetBaseDir();
	FString PluginShaderDirectory = FPaths::Combine(PluginBaseDirectory, TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/TriangleShaderModule"), PluginShaderDirectory);
}

void FTriangleShaderModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

IMPLEMENT_MODULE(FTriangleShaderModule,TriangleShaderModule)