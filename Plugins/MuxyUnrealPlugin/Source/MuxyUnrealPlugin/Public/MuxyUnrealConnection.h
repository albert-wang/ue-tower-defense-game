#pragma once

#include "ThirdParty/nlohmann/json.hpp"
#include "MuxyUnrealPoll.h"
#include "MuxyUnrealConnection.h"

class IMuxyUnrealConnection
{
public:
	virtual ~IMuxyUnrealConnection();

	virtual void SetupConnection() = 0;
	virtual void QueueMessage(nlohmann::json Msg) = 0;
	virtual void SendMessage(nlohmann::json Msg) = 0;

	// Polls
	virtual UMuxyUnrealPoll* CreatePollWithTwoOptions(FString ID, FString Prompt, FString First, FString Second, UObject * WorldContext) = 0;
	virtual UMuxyUnrealPoll* GetPoll(FString ID) = 0;
	virtual void DeletePoll(FString ID) = 0;
	virtual void GetPollResults(FString ID) = 0;

	// Broadcast
	virtual void Broadcast(FString Topic, FString Data) = 0;

	// State management
	virtual void SetChannelStatePath(FString path, FString data) = 0;
};

IMuxyUnrealConnection * CreateMuxyUnrealConnection();