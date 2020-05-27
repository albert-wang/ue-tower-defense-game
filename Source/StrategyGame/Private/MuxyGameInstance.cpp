// Fill out your copyright notice in the Description page of Project Settings.

#include "StrategyGame.h"
#include "MuxyGameInstance.h"

#include "ThirdParty/websocket.h"
#include <thread>
#include <vector>

std::thread * muxyThread = nullptr;
WebsocketConnection * conn = nullptr;
bool connected = false;
std::vector<nlohmann::json> queue;

void SendMessage(nlohmann::json msg)
{
	if (conn && !connected)
	{
		queue.push_back(msg);
	}
	else if (conn)
	{
		conn->send(msg);
	}
}

void SetupConnection(UMuxyGameInstance * inst)
{
	if (!muxyThread)
	{
		conn = new WebsocketConnection("34.222.83.186", 5050);
		muxyThread = new std::thread([]() { conn->run(); });

		conn->onMessage([inst](nlohmann::json obj)
			{
				std::string str = obj.dump();
				FString proxy = str.c_str();
				UE_LOG(LogGame, Warning, TEXT("Got message: %s"), *proxy);

				std::string type = obj["meta"]["action"];
				if (type == "authenticate")
				{
					connected = true;
					if (queue.size())
					{
						for (auto it = queue.begin(); it != queue.end(); ++it)
						{
							conn->send(*it);
						}
					}
				}
				else if (type == "get")
				{
					nlohmann::json data = obj["data"];
					std::string id = data["poll_id"];

					nlohmann::json results = data["results"];

					int winningOption = 0;
					int winningCount = 0;

					int index = 0;
					for (auto it = results.begin(); it != results.end(); ++it)
					{
						int count = it->get<int>();
						if (count > winningCount)
						{
							winningCount = count;
							winningOption = index;
						}

						index++;
					}

					inst->OnGetPollResultsDelegate.Broadcast(FString(id.c_str()), winningOption, winningCount);
				}
			});

		/*
		{ "action": "authenticate", "data": { "client_id": "1f1wyw7py7nhcsrdv8l8jggar42b7d", "pin": "pPVqSe" }}s
		*/

		nlohmann::json msg;
		msg["action"] = "authenticate";

		msg["data"]["client_id"] = "dfv1aj3itxrrgx1094bld8j0ezy4p9";
		msg["data"]["jwt"] = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJhbGxvd2VkX3N0YWdlIjoic2FuZGJveCIsImNoYW5uZWxfaWQiOiIyMzE2MTM1NyIsImV4cCI6MTU5MzE5MjE1OSwib3BhcXVlX3VzZXJfaWQiOiJVMTIzNDU2NzgiLCJwdWJzdWJfcGVybXMiOnsic2VuZCI6WyIqIl19LCJyb2xlIjoidmlld2VyIiwidXNlcl9pZCI6IjEyMzQ1Njc4In0.m5HZN1rLaE4hmBuZgB2b-ItJmg1B6ix1dG3VMXeC_u8";

		conn->send(msg);
	}
}

void UMuxyGameInstance::ConnectWithCode(FString code)
{
	UE_LOG(LogGame, Warning, TEXT("Connection code: %s"), *code);

	SetupConnection(this);
}

void UMuxyGameInstance::CreatePollWithTwoOptions(FString id, FString prompt, FString first, FString second)
{
	nlohmann::json msg;
	msg["action"] = "create";
	msg["params"]["target"] = "poll";
	msg["data"]["poll_id"] = TCHAR_TO_ANSI(*id);
	msg["data"]["prompt"] = TCHAR_TO_ANSI(*prompt);
	
	nlohmann::json options;
	options.push_back(TCHAR_TO_ANSI(*first));
	options.push_back(TCHAR_TO_ANSI(*second));

	msg["data"]["options"] = options;
	
	SetupConnection(this);
	SendMessage(msg);
}

void UMuxyGameInstance::GetPollResults(FString id)
{
	nlohmann::json msg;
	msg["action"] = "get";
	msg["params"]["target"] = "poll";
	msg["data"]["poll_id"] = TCHAR_TO_ANSI(*id);

	SetupConnection(this);
	SendMessage(msg);
}

void UMuxyGameInstance::DeletePoll(FString id)
{
	nlohmann::json msg;
	msg["action"] = "delete";
	msg["params"]["target"] = "poll";
	msg["data"]["poll_id"] = TCHAR_TO_ANSI(*id);

	SetupConnection(this);
	SendMessage(msg);
}