#include "MuxyUnrealPoll.h"
#include "LatentActions.h"

namespace
{
	UWorld* GetWorldFromContext(UObject* ctx)
	{
#if ENGINE_MINOR_VERSION >= 17
		return GEngine->GetWorldFromContextObjectChecked(ctx);
#else
		return GEngine->GetWorldFromContextObject(ctx);
#endif
	}

	class FWaitForResultsAction : public FPendingLatentAction
	{
	public:
		FString ID;
		UWorld* world;
		FName ExecutionFunction;
		int32 OutputLink;
		FWeakObjectPtr CallbackTarget;

		FWaitForResultsAction(FString pollid, UWorld * world, const FLatentActionInfo& info)
			:ID(pollid)
			,world(world)
			,ExecutionFunction(info.ExecutionFunction)
			,OutputLink(info.Linkage)
			,CallbackTarget(info.CallbackTarget)
		{}

		virtual void UpdateOperation(FLatentResponse& Response) override
		{
			UMuxyUnrealPoll * poll = UMuxyUnrealPluginBPLibrary::GetPoll(ID, world);
			Response.FinishAndTriggerIf(poll->HasResults(), ExecutionFunction, OutputLink, CallbackTarget);
		}
	private:
	};
}

void UMuxyUnrealPoll::QueueGetResults()
{
	// Reset results and wait for new ones
	winner = 0;
	winningVoteCount = 0;
	hasUpdatedResults = false;

	UMuxyUnrealPluginBPLibrary::GetConnection()->GetPollResults(ID);
}

void UMuxyUnrealPoll::DeclareWinner(int win, int count)
{
	winner = win;
	winningVoteCount = count;
	hasUpdatedResults = true;
}

void UMuxyUnrealPoll::GetResults(int& winnerOut, int& voteCountOut)
{
	winnerOut = winner;
	voteCountOut = winningVoteCount;
}

bool UMuxyUnrealPoll::HasResults()
{
	return hasUpdatedResults;
}

void UMuxyUnrealPoll::QueueGetResultsLatent(UObject * ctx, FLatentActionInfo info)
{
	UWorld * world = GetWorldFromContext(ctx);
	if (!world)
	{
		return;
	}

	FLatentActionManager& latentActionManager = world->GetLatentActionManager();
	if (latentActionManager.FindExistingAction<FWaitForResultsAction>(info.CallbackTarget, info.UUID) == nullptr)
	{
		latentActionManager.AddNewAction(info.CallbackTarget, info.UUID, new FWaitForResultsAction(ID, world, info));
	}

	QueueGetResults();
}

void UMuxyUnrealPoll::DeletePoll()
{
	UMuxyUnrealPluginBPLibrary::GetConnection()->DeletePoll(ID);
}