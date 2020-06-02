// Copyright Epic Games, Inc. All Rights Reserved.

#include "MuxyUnrealPluginBPLibrary.h"
#include "MuxyUnrealPlugin.h"

#include "ThirdParty/websocket.h"
#include <thread>
#include <vector>


std::thread* muxyThread = nullptr;
WebsocketConnection* conn = nullptr;
bool connected = false;
std::vector<nlohmann::json> queue;

UMuxyUnrealPluginEventSource * UMuxyUnrealPluginBPLibrary::eventSource = nullptr;

void SetupConnection()
{
	if (!muxyThread)
	{
		conn = new WebsocketConnection("34.222.83.186", 5050);
		muxyThread = new std::thread([]() { conn->run(); });

		conn->onMessage([](nlohmann::json obj)
			{
				std::string str = obj.dump();
				FString proxy = str.c_str();
				UE_LOG(LogTemp, Warning, TEXT("Got message: %s"), *proxy);
				auto inst = UMuxyUnrealPluginBPLibrary::GetEventSource();

				std::string type = obj["meta"]["action"];
				if (type == "authenticate")
				{
					if (obj["data"].is_null())
					{
						// Don't do anything - invisible failure
					}
					else
					{
						std::string jwt = obj["data"]["jwt"];

						inst->JWT = FString(jwt.c_str());
						inst->OnMuxyAuth.Broadcast();

						connected = true;
						if (queue.size())
						{
							for (auto it = queue.begin(); it != queue.end(); ++it)
							{
								conn->send(*it);
							}
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
	}
}

void SendMessage(nlohmann::json msg)
{
	SetupConnection();

	if (conn && !connected)
	{
		queue.push_back(msg);
	}
	else if (conn)
	{
		conn->send(msg);
	}
}


UMuxyUnrealPluginBPLibrary::UMuxyUnrealPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

UMuxyUnrealPluginEventSource* UMuxyUnrealPluginBPLibrary::GetEventSource()
{
	if (eventSource == nullptr)
	{
		eventSource = NewObject<UMuxyUnrealPluginEventSource>();
	}

	return eventSource;
}

void UMuxyUnrealPluginBPLibrary::AuthenticateWithCode(FString client, FString code)
{
	SetupConnection();

	UE_LOG(LogTemp, Warning, TEXT("Attempting to connect: %s:%s"), *client, *code);

	nlohmann::json msg;
	msg["action"] = "authenticate";

	// "dfv1aj3itxrrgx1094bld8j0ezy4p9";
	msg["data"]["client_id"] = std::string(TCHAR_TO_UTF8(*client));
	msg["data"]["pin"] = std::string(TCHAR_TO_UTF8(*code));

	conn->send(msg);
}

void UMuxyUnrealPluginBPLibrary::AuthenticateWithJWT(FString client, FString jwt)
{
	SetupConnection();

	nlohmann::json msg;
	msg["action"] = "authenticate";

	// "dfv1aj3itxrrgx1094bld8j0ezy4p9";
	msg["data"]["client_id"] = std::string(TCHAR_TO_UTF8(*client));
	msg["data"]["jwt"] = std::string(TCHAR_TO_UTF8(*jwt));

	conn->send(msg);
}

void UMuxyUnrealPluginBPLibrary::CreatePollWithTwoOptions(FString id, FString prompt, FString first, FString second)
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

	SendMessage(msg);
}

void UMuxyUnrealPluginBPLibrary::GetPollResults(FString id)
{
	nlohmann::json msg;
	msg["action"] = "get";
	msg["params"]["target"] = "poll";
	msg["data"]["poll_id"] = TCHAR_TO_ANSI(*id);

	SendMessage(msg);
}

void UMuxyUnrealPluginBPLibrary::DeletePoll(FString id)
{
	nlohmann::json msg;
	msg["action"] = "delete";
	msg["params"]["target"] = "poll";
	msg["data"]["poll_id"] = TCHAR_TO_ANSI(*id);

	SendMessage(msg);
}