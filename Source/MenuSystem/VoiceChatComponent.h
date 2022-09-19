// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <iostream>
#include <string.h>
#include "AgoraPluginInterface.h"
#include "VoiceChatComponent.generated.h"
using namespace agora::rtc;

//UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
UCLASS(Blueprintable)
class MENUSYSTEM_API UVoiceChatComponent : public UActorComponent, public IRtcEngineEventHandlerEx
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVoiceChatComponent();

	UPROPERTY(EditAnywhere, Category = "Agora|Context")
	FString APP_ID = "YOUR_APPID";

	UPROPERTY(EditAnywhere, Category = "Agora|Context")
	FString TOKEN = "";

	UPROPERTY(EditAnywhere, Category = "Agora|Context")
	FString CHANNEL_NAME = "YOUR_CHANNEL_NAME";


	UFUNCTION(BlueprintCallable)
	void InitAgoraEngine();

	UFUNCTION(BlueprintCallable)
	void ReleaseAgoraEngine();

	UFUNCTION(BlueprintCallable)
	void JoinChannel();

	UFUNCTION(BlueprintCallable)
	void LeaveChannel();

	UFUNCTION(BlueprintCallable)
	void ToggleMute(bool bMute);

	void onUserJoined(agora::rtc::uid_t uid, int elapsed) override;

	void onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed);

	void onError(int err, const char* msg);

	void onRemoteAudioStateChanged(const RtcConnection& connection, uid_t remoteUid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	agora::rtc::IRtcEngine* RtcEngineProxy;

	std::string AppID;

	std::string Token;

	std::string ChannelName;
		
};
