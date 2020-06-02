#pragma once

#include "UObject/Object.h"
#include "Engine/DynamicBlueprintBinding.h"

#include "MuxyUnrealPluginBlueprintEventSource.generated.h"

UCLASS()
class UMuxyUnrealPluginEventSource : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void DoSomething(float Param);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMuxyAuthenticateDelegate);
	UPROPERTY(BlueprintAssignable)
	FMuxyAuthenticateDelegate OnMuxyAuth;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPollResultDelegate, FString, poll_id, int, winner, int, voteCount);
	UPROPERTY(BlueprintAssignable)
	FPollResultDelegate OnGetPollResultsDelegate;

	UPROPERTY(BlueprintReadOnly)
	FString JWT;
};