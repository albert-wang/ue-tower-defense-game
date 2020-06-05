#pragma once

#include "ThirdParty/nlohmann/json.hpp"
#include "MuxyUnrealPoll.h"

class IMuxyUnrealConnection
{
public:
	virtual ~IMuxyUnrealConnection();

	virtual void SetupConnection() = 0;
	virtual void QueueMessage(nlohmann::json msg) = 0;
	virtual void SendMessage(nlohmann::json msg) = 0;

	virtual UMuxyUnrealPoll* CreatePollWithTwoOptions(FString id, FString prompt, FString first, FString second, UObject * world) = 0;
	virtual UMuxyUnrealPoll* GetPoll(FString id) = 0;

	virtual void DeletePoll(FString id) = 0;
	virtual void GetPollResults(FString id) = 0;
};

IMuxyUnrealConnection * CreateMuxyUnrealConnection();