// Copyright Epic Games, Inc. All Rights Reserved.

#include "MuxyUnrealPluginBPLibrary.h"
#include "MuxyUnrealPlugin.h"
#include "MuxyUnrealConnection.h"

#include "Engine/Engine.h"
#include "ThirdParty/nlohmann/json.hpp"

UMuxyUnrealPluginEventSource* UMuxyUnrealPluginBPLibrary::eventSource = nullptr;
IMuxyUnrealConnection* UMuxyUnrealPluginBPLibrary::connection = nullptr;

UMuxyUnrealPluginBPLibrary::UMuxyUnrealPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

UMuxyUnrealPluginEventSource* UMuxyUnrealPluginBPLibrary::GetEventSource(UObject* ctx)
{
	CreateEventSource(ctx);
	return eventSource;
}


UMuxyUnrealPluginEventSource* UMuxyUnrealPluginBPLibrary::GetEventSource()
{
	return eventSource;
}

void UMuxyUnrealPluginBPLibrary::CreateEventSource(UObject * ctx)
{
	if (!eventSource)
	{
		eventSource = NewObject< UMuxyUnrealPluginEventSource>(ctx);
	}
}

IMuxyUnrealConnection* UMuxyUnrealPluginBPLibrary::GetConnection()
{
	if (connection == nullptr)
	{
		connection = CreateMuxyUnrealConnection();
	}

	return connection;
}

void UMuxyUnrealPluginBPLibrary::ResetSourceAndConnection()
{
	eventSource = nullptr;

	delete connection;
	connection = nullptr;
}

void UMuxyUnrealPluginBPLibrary::AuthenticateWithCode(FString client, FString code, UObject* ctx)
{
	CreateEventSource(ctx);
	UE_LOG(LogTemp, Warning, TEXT("Attempting to connect: %s:%s"), *client, *code);

	nlohmann::json msg;
	msg["action"] = "authenticate";

	// "dfv1aj3itxrrgx1094bld8j0ezy4p9";
	msg["data"]["client_id"] = std::string(TCHAR_TO_UTF8(*client));
	msg["data"]["pin"] = std::string(TCHAR_TO_UTF8(*code));

	GetConnection()->SendMessage(msg);
}

void UMuxyUnrealPluginBPLibrary::AuthenticateWithJWT(FString client, FString jwt, UObject* ctx)
{
	CreateEventSource(ctx);

	nlohmann::json msg;
	msg["action"] = "authenticate";

	// "dfv1aj3itxrrgx1094bld8j0ezy4p9";
	msg["data"]["client_id"] = std::string(TCHAR_TO_UTF8(*client));
	msg["data"]["jwt"] = std::string(TCHAR_TO_UTF8(*jwt));

	GetConnection()->SendMessage(msg);
}

UMuxyUnrealPoll * UMuxyUnrealPluginBPLibrary::CreatePollWithTwoOptions(FString id, FString prompt, FString first, FString second, UObject* ctx)
{
	CreateEventSource(ctx);
	return GetConnection()->CreatePollWithTwoOptions(id, prompt, first, second);
}

UMuxyUnrealPoll* UMuxyUnrealPluginBPLibrary::GetPoll(FString id, UObject* ctx)
{
	CreateEventSource(ctx);
	return GetConnection()->GetPoll(id);
}