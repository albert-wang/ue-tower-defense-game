#include "MuxyUnrealPoll.h"
#include "LatentActions.h"

#include "MuxyUnrealConnection.h"
#include "MuxyUnrealPluginBPLibrary.h"

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
		UWorld* World;
		FName ExecutionFunction;
		int32 OutputLink;
		FWeakObjectPtr CallbackTarget;

		FWaitForResultsAction(FString PollID, UWorld * World, const FLatentActionInfo& info)
			:ID(PollID)
			,World(World)
			,ExecutionFunction(info.ExecutionFunction)
			,OutputLink(info.Linkage)
			,CallbackTarget(info.CallbackTarget)
		{}

		virtual void UpdateOperation(FLatentResponse& Response) override
		{
			UMuxyUnrealPoll * Poll = UMuxyUnrealPluginBPLibrary::GetPoll(ID, World);
			if (Poll)
			{
				Response.FinishAndTriggerIf(Poll->HasResults(), ExecutionFunction, OutputLink, CallbackTarget);
			}
		}
	private:
	};
}

void UMuxyUnrealPoll::QueueGetResults()
{
	// Reset results and wait for new ones
	Winner = 0;
	WinningVoteCount = 0;
	HasUpdatedResults = false;

	UMuxyUnrealPluginBPLibrary::GetConnection()->GetPollResults(ID);
}

void UMuxyUnrealPoll::DeclareWinner(int Win, int Count)
{
	Winner = Win;
	WinningVoteCount = Count;
	HasUpdatedResults = true;
}

void UMuxyUnrealPoll::GetResults(int& WinnerOut, int& VoteCountOut)
{
	WinnerOut = Winner;
	VoteCountOut = WinningVoteCount;
}

bool UMuxyUnrealPoll::HasResults()
{
	return HasUpdatedResults;
}

void UMuxyUnrealPoll::QueueGetResultsLatent(UObject * Context, FLatentActionInfo LatentInfo)
{
	UWorld * World = GetWorldFromContext(Context);
	if (!World)
	{
		return;
	}

	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
	if (LatentActionManager.FindExistingAction<FWaitForResultsAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
	{
		LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, new FWaitForResultsAction(ID, World, LatentInfo));
	}

	QueueGetResults();
}

void UMuxyUnrealPoll::DeletePoll()
{
	UMuxyUnrealPluginBPLibrary::GetConnection()->DeletePoll(ID);
}