// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VoiceInterface.h"
#include "Blueprint/UserWidget.h"
#include "BaseVoiceActor.h"
#include "BaseVoiceUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class MENUSYSTEM_API UBaseVoiceUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	virtual void Init(ABaseVoiceActor* source);
	UFUNCTION(BlueprintCallable)
	virtual bool IsVoiceConnected();
	UFUNCTION(BlueprintCallable)
	virtual bool GetMuteStatus();
	UFUNCTION(BlueprintCallable)
	virtual bool ToggleMute(bool bMute);
public:
	IVoiceInterface* Voice;
	ABaseVoiceActor* BaseVoice;
};
