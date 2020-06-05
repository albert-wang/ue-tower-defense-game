#pragma once
#include "MuxyUnrealPoll.generated.h"

UCLASS()
class UMuxyUnrealPoll : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FString ID;

	UFUNCTION(BlueprintCallable)
	void QueueGetResults();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPollResultDelegate, int, Winner, int, VoteCount);
	UPROPERTY(BlueprintAssignable)
	FPollResultDelegate OnGetPollResultsDelegate;

	UFUNCTION(BlueprintCallable)
	void GetResults(int& Winner, int& VoteCount);

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo="LatentInfo", WorldContext="Context"))
	void QueueGetResultsLatent(UObject * Context, FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintCallable)
	void DeletePoll();

	UFUNCTION(BlueprintCallable)
	bool HasResults();

	void DeclareWinner(int Winner, int Count);
private:
	bool HasUpdatedResults;
	int Winner;
	int WinningVoteCount;
};