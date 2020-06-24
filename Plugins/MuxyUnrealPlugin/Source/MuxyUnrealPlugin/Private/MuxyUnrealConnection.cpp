#include "MuxyUnrealConnection.h"
#include "MuxyUnrealPluginBPLibrary.h"
#include "MuxyUnrealPluginBlueprintEventSource.h"

#include "ThirdParty/websocket.h"
#include <thread>
#include <vector>

IMuxyUnrealConnection::~IMuxyUnrealConnection()
{}

class MuxyUnrealConnectionImpl : public IMuxyUnrealConnection
{
public:
	MuxyUnrealConnectionImpl()
		:MuxyThread(nullptr)
		,Conn(nullptr)
		,Connected(false)
	{}

	void SetupConnection() override
	{
		if (!MuxyThread)
		{
			Conn = new WebsocketConnection("staging.gamelink.muxy.io", 80);
			MuxyThread = new std::thread([this]() { Conn->run(); });

			Conn->onMessage([this](nlohmann::json Obj)
				{
					std::string Str = Obj.dump();
					FString Proxy = Str.c_str();
					UE_LOG(LogTemp, Warning, TEXT("Got message: %s"), *Proxy);
					auto EventSource = UMuxyUnrealPluginBPLibrary::GetEventSource();

					std::string Type = Obj["meta"]["action"];
					if (Type == "authenticate")
					{
						if (Obj["data"].is_null())
						{
							// Don't do anything - invisible failure
						}
						else
						{
							std::string JWT = Obj["data"]["jwt"];

							EventSource->JWT = FString(JWT.c_str());
							EventSource->OnMuxyAuth.Broadcast();

							Connected = true;
							if (Queue.size())
							{
								for (auto it = Queue.begin(); it != Queue.end(); ++it)
								{
									Conn->send(*it);
								}
							}
						}
					}
					else if (Type == "get")
					{
						if (Obj["data"].is_null())
						{
							// Don't do anything - invisible failure
						}
						else
						{
							nlohmann::json Data = Obj["data"];
							nlohmann::json Poll = Data["poll"];
							std::string ID = Poll["poll_id"];

							nlohmann::json Results = Data["results"];

							int WinningOption = 0;
							int WinningCount = 0;

							int Index = 0;
							for (auto it = Results.begin(); it != Results.end(); ++it)
							{
								int Count = it->get<int>();
								if (Count > WinningCount)
								{
									WinningCount = Count;
									WinningOption = Index;
								}

								Index++;
							}

							FString IDFString(ID.c_str());
							EventSource->OnGetPollResultsDelegate.Broadcast(IDFString, WinningOption, WinningCount);

							// Try to find if this was a created poll
							for (size_t i = 0; i < Polls.Num(); ++i)
							{
								UMuxyUnrealPoll * Poll = Polls[i];
								if (Poll->ID == IDFString)
								{
									Poll->DeclareWinner(WinningOption, WinningCount);
									Poll->OnGetPollResultsDelegate.Broadcast(WinningOption, WinningCount);
								}
							}
						}
					}
				});
		}
	}

	void QueueMessage(nlohmann::json Msg) override
	{
		SetupConnection();

		if (Conn && !Connected)
		{
			Queue.push_back(Msg);
		}
		else if (Conn)
		{
			std::string Str = Msg.dump();
			FString Proxy = Str.c_str();
			UE_LOG(LogTemp, Warning, TEXT("Send message: %s"), *Proxy);

			Conn->send(Msg);
		}
	}

	void SendMessage(nlohmann::json Msg) override
	{
		std::string Str = Msg.dump();
		FString Proxy = Str.c_str();
		UE_LOG(LogTemp, Warning, TEXT("Send message: %s"), *Proxy);

		SetupConnection();
		Conn->send(Msg);
	}

	UMuxyUnrealPoll * CreatePollWithTwoOptions(FString ID, FString Prompt, FString First, FString Second, UObject * WorldContext) override
	{
		nlohmann::json Msg;
		Msg["action"] = "create";
		Msg["params"]["target"] = "poll";
		Msg["data"]["poll_id"] = TCHAR_TO_ANSI(*ID);
		Msg["data"]["prompt"] = TCHAR_TO_ANSI(*Prompt);

		nlohmann::json Options;
		Options.push_back(TCHAR_TO_ANSI(*First));
		Options.push_back(TCHAR_TO_ANSI(*Second));

		Msg["data"]["options"] = Options;

		QueueMessage(Msg);

		UMuxyUnrealPoll* Poll = NewObject<UMuxyUnrealPoll>(WorldContext);
		Poll->ID = ID;

		UE_LOG(LogTemp, Warning, TEXT("Size of polls: %d"), Polls.Num());

		Polls.Add(Poll);
		return Poll;
	}

	UMuxyUnrealPoll * GetPoll(FString ID)
	{
		for (size_t i = 0; i < Polls.Num(); ++i)
		{
			if (Polls[i]->ID == ID)
			{
				return Polls[i];
			}
		}

		return nullptr;
	}

	void GetPollResults(FString ID)
	{
		nlohmann::json Msg;
		Msg["action"] = "get";
		Msg["params"]["target"] = "poll";
		Msg["data"]["poll_id"] = TCHAR_TO_ANSI(*ID);

		QueueMessage(Msg);
	}

	void DeletePoll(FString ID)
	{
		nlohmann::json Msg;
		Msg["action"] = "delete";
		Msg["params"]["target"] = "poll";
		Msg["data"]["poll_id"] = TCHAR_TO_ANSI(*ID);
		
		Polls.RemoveAll([&ID](UMuxyUnrealPoll * Poll)
		{
			return Poll->ID == ID;
		});
		
		QueueMessage(Msg);
	}

	void Broadcast(FString Topic, FString Data)
	{
		nlohmann::json Msg;
		Msg["action"] = "broadcast";
		Msg["data"]["topic"] = TCHAR_TO_ANSI(*Topic);
		Msg["data"]["message"] = TCHAR_TO_ANSI(*Data);

		QueueMessage(Msg);
	}

	void SetChannelStatePath(FString path, FString data)
	{
		nlohmann::json Msg;
		Msg["action"] = "set";
		Msg["parasm"]["target"] = "channel";

		nlohmann::json operations;
		nlohmann::json replacement;

		replacement["op"] = "add_intermediates";
		replacement["path"] = TCHAR_TO_ANSI(*path);
		replacement["value"] = TCHAR_TO_ANSI(*data);

		operations.push_back(replacement);

		Msg["data"]["state"] = operations;

		QueueMessage(Msg);
	}
private:
	std::thread* MuxyThread;
	WebsocketConnection* Conn;
	bool Connected;
	std::vector<nlohmann::json> Queue;

	TArray<UMuxyUnrealPoll *> Polls;
};

IMuxyUnrealConnection * CreateMuxyUnrealConnection()
{
	return new MuxyUnrealConnectionImpl();
}