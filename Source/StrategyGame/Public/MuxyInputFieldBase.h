// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MuxyInputFieldBase.generated.h"

/**
 * 
 */
UCLASS()
class STRATEGYGAME_API UMuxyInputFieldBase : public UUserWidget
{
	GENERATED_BODY()
        
    UFUNCTION(BlueprintCallable)
    void SubmitCode(FString code);
};
