#include "TriangleShaderModule.h"

void FTriangleShaderModule::StartupModule(){
	
	FString BaseDir = FPaths::Combine(FPaths::GameSourceDir(), TEXT("TriangleShaderModule"));
	FString ModuleShaderDir = FPaths::Combine(BaseDir, TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/TriangleShaderModule"), ModuleShaderDir);
}

void FTriangleShaderModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

IMPLEMENT_MODULE(FTriangleShaderModule,TriangleShaderModule)