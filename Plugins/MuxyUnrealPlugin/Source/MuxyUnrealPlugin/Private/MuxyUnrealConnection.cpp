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
		:muxyThread(nullptr)
		,conn(nullptr)
		,connected(false)
	{}

	void SetupConnection() override
	{
		if (!muxyThread)
		{
			conn = new WebsocketConnection("34.222.83.186", 5050);
			muxyThread = new std::thread([this]() { conn->run(); });

			conn->onMessage([this](nlohmann::json obj)
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

						FString idfstr(id.c_str());
						inst->OnGetPollResultsDelegate.Broadcast(idfstr, winningOption, winningCount);

						// Try to find if this was a created poll
						for (size_t i = 0; i < polls.Num(); ++i)
						{
							UMuxyUnrealPoll * poll = polls[i];
							if (poll->ID == idfstr)
							{
								poll->DeclareWinner(winningOption, winningCount);
								poll->OnGetPollResultsDelegate.Broadcast(winningOption, winningCount);
							}
						}
					}
				});
		}
	}

	void QueueMessage(nlohmann::json msg) override
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

	void SendMessage(nlohmann::json msg) override
	{
		SetupConnection();
		conn->send(msg);
	}

	UMuxyUnrealPoll * CreatePollWithTwoOptions(FString id, FString prompt, FString first, FString second) override
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

		QueueMessage(msg);

		UMuxyUnrealPoll* poll = NewObject<UMuxyUnrealPoll>();
		poll->ID = id;

		polls.Add(poll);
		return poll;
	}

	UMuxyUnrealPoll * GetPoll(FString id)
	{
		for (size_t i = 0; i < polls.Num(); ++i)
		{
			if (polls[i]->ID == id)
			{
				return polls[i];
			}
		}

		return nullptr;
	}

	void GetPollResults(FString id)
	{
		nlohmann::json msg;
		msg["action"] = "get";
		msg["params"]["target"] = "poll";
		msg["data"]["poll_id"] = TCHAR_TO_ANSI(*id);

		QueueMessage(msg);
	}

	void DeletePoll(FString id)
	{
		nlohmann::json msg;
		msg["action"] = "delete";
		msg["params"]["target"] = "poll";
		msg["data"]["poll_id"] = TCHAR_TO_ANSI(*id);
		
		QueueMessage(msg);
	}
private:
	std::thread* muxyThread = nullptr;
	WebsocketConnection* conn = nullptr;
	bool connected = false;
	std::vector<nlohmann::json> queue;

	TArray<UMuxyUnrealPoll *> polls;
};

IMuxyUnrealConnection * CreateMuxyUnrealConnection()
{
	return new MuxyUnrealConnectionImpl();
}