// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MuxyUnrealPluginBlueprintEventSource.h"
#include "MuxyUnrealPoll.h"
#include "MuxyUnrealSubscription.h"

#include "MuxyUnrealPluginBPLibrary.generated.h"

class IMuxyUnrealConnection;

/* 
*	Function library class.
*	Each function in it is expected to be static and represents blueprint node that can be called in any blueprint.
*
*	When declaring function you can define metadata for the node. Key function specifiers will be BlueprintPure and BlueprintCallable.
*	BlueprintPure - means the function does not affect the owning object in any way and thus creates a node without Exec pins.
*	BlueprintCallable - makes a function which can be executed in Blueprints - Thus it has Exec pins.
*	DisplayName - full name of the node, shown when you mouse over the node and in the blueprint drop down menu.
*				Its lets you name the node using characters not allowed in C++ function names.
*	CompactNodeTitle - the word(s) that appear on the node.
*	Keywords -	the list of keywords that helps you to find node when you search for it using Blueprint drop-down menu. 
*				Good example is "Print String" node which you can find also by using keyword "log".
*	Category -	the category your node will be under in the Blueprint drop-down menu.
*
*	For more info on custom blueprint nodes visit documentation:
*	https://wiki.unrealengine.com/Custom_Blueprint_Node_Creation
*/
UCLASS()
class UMuxyUnrealPluginBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	/*(
	UFUNCTION(BlueprintCallable, 
		meta = (DisplayName = "Execute Sample function", 
				Keywords = "MuxyUnrealPlugin sample test testing"), 
		Category = "MuxyUnrealPluginTesting")
	static float MuxyUnrealPluginSampleFunction(float Param);
	*/

	UFUNCTION(BlueprintCallable, meta = (WorldContext="Context"))
	static void AuthenticateWithCode(FString ClientID, FString code, UObject* Context);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "Context"))
	static void AuthenticateWithJWT(FString ClientID, FString JWT, UObject* Context);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "Context"))
	static UMuxyUnrealPoll * CreatePollWithTwoOptions(FString ID, FString prompt, FString first, FString second, UObject* Context);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "Context"))
	static UMuxyUnrealPoll* GetPoll(FString ID, UObject* Context);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "Context"))
	static UMuxyUnrealPluginEventSource * GetEventSource(UObject * Context);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "Context"))
	static void BroadcastMessage(FString Topic, FString Data, UObject* Context);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "Context"))
	static void SetChannelStatePath(FString Path, FString info, UObject* Context);
	
	static UMuxyUnrealPluginEventSource * GetEventSource();
	static IMuxyUnrealConnection * GetConnection();
	static void ResetSourceAndConnection();
private:
	static void CreateEventSource(UObject * Context);

	static UMuxyUnrealPluginEventSource * EventSource;
	static IMuxyUnrealConnection * Connection;
};
