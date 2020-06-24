// Copyright Epic Games, Inc. All Rights Reserved.

#include "MuxyUnrealPluginBPLibrary.h"
#include "MuxyUnrealPlugin.h"
#include "MuxyUnrealConnection.h"

#include "Engine/Engine.h"
#include "ThirdParty/nlohmann/json.hpp"

UMuxyUnrealPluginEventSource* UMuxyUnrealPluginBPLibrary::EventSource = nullptr;
IMuxyUnrealConnection* UMuxyUnrealPluginBPLibrary::Connection = nullptr;

UMuxyUnrealPluginBPLibrary::UMuxyUnrealPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

UMuxyUnrealPluginEventSource* UMuxyUnrealPluginBPLibrary::GetEventSource(UObject* Context)
{
	CreateEventSource(Context);
	return EventSource;
}


UMuxyUnrealPluginEventSource* UMuxyUnrealPluginBPLibrary::GetEventSource()
{
	return EventSource;
}

void UMuxyUnrealPluginBPLibrary::CreateEventSource(UObject * Context)
{
	if (!EventSource)
	{
		EventSource = NewObject<UMuxyUnrealPluginEventSource>(Context);
		UWorld * world = GEngine->GetWorldFromContextObjectChecked(Context);

		if (world)
		{
			world->ExtraReferencedObjects.Add(EventSource);
		}
	}
}

IMuxyUnrealConnection* UMuxyUnrealPluginBPLibrary::GetConnection()
{
	if (Connection == nullptr)
	{
		Connection = CreateMuxyUnrealConnection();
	}

	return Connection;
}

void UMuxyUnrealPluginBPLibrary::ResetSourceAndConnection()
{
	EventSource = nullptr;

	delete Connection;
	Connection = nullptr;
}

void UMuxyUnrealPluginBPLibrary::AuthenticateWithCode(FString ClientID, FString Code, UObject* Context)
{
	CreateEventSource(Context);
	UE_LOG(LogTemp, Warning, TEXT("Attempting to connect: %s:%s"), *ClientID, *Code);

	nlohmann::json Msg;
	Msg["action"] = "authenticate";

	// "dfv1aj3itxrrgx1094bld8j0ezy4p9";
	Msg["data"]["client_id"] = std::string(TCHAR_TO_UTF8(*ClientID));
	Msg["data"]["pin"] = std::string(TCHAR_TO_UTF8(*Code));

	GetConnection()->SendMessage(Msg);
}

void UMuxyUnrealPluginBPLibrary::AuthenticateWithJWT(FString ClientID, FString JWT, UObject* Context)
{
	CreateEventSource(Context);

	nlohmann::json Msg;
	Msg["action"] = "authenticate";

	// "dfv1aj3itxrrgx1094bld8j0ezy4p9";
	Msg["data"]["client_id"] = std::string(TCHAR_TO_UTF8(*ClientID));
	Msg["data"]["jwt"] = std::string(TCHAR_TO_UTF8(*JWT));

	GetConnection()->SendMessage(Msg);
}

UMuxyUnrealPoll * UMuxyUnrealPluginBPLibrary::CreatePollWithTwoOptions(FString ID, FString Prompt, FString First, FString Second, UObject* Context)
{
	CreateEventSource(Context);
	return GetConnection()->CreatePollWithTwoOptions(ID, Prompt, First, Second, Context);
}

UMuxyUnrealPoll* UMuxyUnrealPluginBPLibrary::GetPoll(FString ID, UObject* Context)
{
	CreateEventSource(Context);
	return GetConnection()->GetPoll(ID);
}

void UMuxyUnrealPluginBPLibrary::BroadcastMessage(FString Topic, FString Data, UObject* Context)
{
	CreateEventSource(Context);
	return GetConnection()->Broadcast(Topic, Data);
}

void UMuxyUnrealPluginBPLibrary::SetChannelStatePath(FString Path, FString Data, UObject* Context)
{
	CreateEventSource(Context);
	return GetConnection()->SetChannelStatePath(Path, Data);
}