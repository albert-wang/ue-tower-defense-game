// Copyright Epic Games, Inc. All Rights Reserved.

#include "MuxyUnrealPlugin.h"
#include "MuxyUnrealPluginBPLibrary.h"

#define LOCTEXT_NAMESPACE "FMuxyUnrealPluginModule"

void FMuxyUnrealPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UMuxyUnrealPluginBPLibrary::ResetSourceAndConnection();
}

void FMuxyUnrealPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UMuxyUnrealPluginBPLibrary::ResetSourceAndConnection();
}

void FMuxyUnrealPluginModule::PreUnloadCallback()
{
	UMuxyUnrealPluginBPLibrary::ResetSourceAndConnection();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMuxyUnrealPluginModule, MuxyUnrealPlugin)