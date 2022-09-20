// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <iostream>
#include <string.h>
#include "VoiceInterface.h"
#include "AgoraPluginInterface.h"
#include "BaseVoiceActor.h"
#if PLATFORM_ANDROID
#include "AndroidPermission/Classes/AndroidPermissionFunctionLibrary.h"
#endif
#include "AgoraVoiceActor.generated.h"
using namespace agora::rtc;

UCLASS()
class MENUSYSTEM_API AAgoraVoiceActor : public ABaseVoiceActor, public agora::rtc::IRtcEngineEventHandler
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAgoraVoiceActor();

	UPROPERTY(EditAnywhere, Category = "Agora|Context")
	FString APP_ID = "YOUR_APPID";

	UPROPERTY(EditAnywhere, Category = "Agora|Context")
	FString TOKEN = "";

	UPROPERTY(EditAnywhere, Category = "Agora|Context")
	FString CHANNEL_NAME = "YOUR_CHANNEL_NAME";

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void JoinChannel();

	UFUNCTION(BlueprintCallable)
	void LeaveChannel();

	UFUNCTION(BlueprintCallable)
	void ToggleMute(bool bMute);

	void onUserJoined(agora::rtc::uid_t uid, int elapsed) override;

	void onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed);

	void onLeaveChannel(const RtcStats& stats);

	void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason);

	void onUserMuteAudio(uid_t uid, bool muted);

	void onConnectionLost();

	void onError(int err, const char* msg);

	void onRemoteAudioStateChanged( uid_t remoteUid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed) override;
	void onAudioVolumeIndication(const AudioVolumeInfo* speakers, unsigned int speakerNumber,int totalVolume);
	void onRtcStats(const RtcStats& stats);

	void onRejoinChannelSuccess(const char* channel, uid_t uid, int elapsed);

public:
	bool IsVoiceConnected() override;
	bool Mute(bool bMute) override;
	bool GetMuteStatus() override;

private:
	agora::rtc::IRtcEngine* RtcEngineProxy;

	FString AppId;

	FString Token;

	FString ChannelName;

	void InitAgoraEngine();

	void ReleaseAgoraEngine();

	bool bIsVoiceConnected = false;
	bool bIsLocalPlayerMuted = false;
};
