#include "MuxyUnrealPluginBlueprintEventSource.h"
#include "MuxyUnrealPluginBPLibrary.h"

void UMuxyUnrealPluginEventSource::BeginDestroy()
{
	UMuxyUnrealPluginBPLibrary::ResetSourceAndConnection();
	Super::BeginDestroy();
}