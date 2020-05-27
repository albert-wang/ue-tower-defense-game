// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MuxyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class STRATEGYGAME_API UMuxyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void ConnectWithCode(FString code);

	UFUNCTION(BlueprintCallable)
	void CreatePollWithTwoOptions(FString id, FString prompt, FString option0, FString option1);

	UFUNCTION(BlueprintCallable)
	void GetPollResults(FString id);

	UFUNCTION(BlueprintCallable)
	void DeletePoll(FString id);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPollResultDelegate, FString, poll_id, int, winner, int, voteCount);
	UPROPERTY(BlueprintAssignable)
	FPollResultDelegate OnGetPollResultsDelegate;
};
