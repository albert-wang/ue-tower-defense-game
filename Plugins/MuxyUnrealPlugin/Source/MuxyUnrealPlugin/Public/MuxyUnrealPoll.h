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

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPollResultDelegate, int, winner, int, voteCount);
	UPROPERTY(BlueprintAssignable)
	FPollResultDelegate OnGetPollResultsDelegate;

	UFUNCTION(BlueprintCallable)
	void GetResults(int& winner, int& voteCount);

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo="info", WorldContext="ctx"))
	void QueueGetResultsLatent(UObject * ctx, FLatentActionInfo info);

	UFUNCTION(BlueprintCallable)
	void DeletePoll();

	UFUNCTION(BlueprintCallable)
	bool HasResults();

	void DeclareWinner(int winner, int count);
private:
	bool hasUpdatedResults;
	int winner;
	int winningVoteCount;
};