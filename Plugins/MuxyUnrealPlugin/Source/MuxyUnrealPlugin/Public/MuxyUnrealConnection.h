#pragma once

#include "ThirdParty/nlohmann/json.hpp"
#include "MuxyUnrealPoll.h"

class IMuxyUnrealConnection
{
public:
	virtual ~IMuxyUnrealConnection();

	virtual void SetupConnection() = 0;
	virtual void QueueMessage(nlohmann::json Msg) = 0;
	virtual void SendMessage(nlohmann::json Msg) = 0;

	virtual UMuxyUnrealPoll* CreatePollWithTwoOptions(FString ID, FString Prompt, FString First, FString Second, UObject * WorldContext) = 0;
	virtual UMuxyUnrealPoll* GetPoll(FString ID) = 0;

	virtual void DeletePoll(FString ID) = 0;
	virtual void GetPollResults(FString ID) = 0;
};

IMuxyUnrealConnection * CreateMuxyUnrealConnection();