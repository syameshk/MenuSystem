// Fill out your copyright notice in the Description page of Project Settings.


#include "AgoraVoiceActor.h"

// Sets default values
AAgoraVoiceActor::AAgoraVoiceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAgoraVoiceActor::BeginPlay()
{
	Super::BeginPlay();
	InitAgoraEngine();
}

// Called every frame
void AAgoraVoiceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAgoraVoiceActor::JoinChannel()
{
	UE_LOG(LogTemp, Warning, TEXT("AAgoraVoiceActor JoinChannel"));

	RtcEngineProxy->enableAudio();
	RtcEngineProxy->joinChannel(TCHAR_TO_ANSI(*Token), TCHAR_TO_ANSI(*ChannelName), "", 0);
	RtcEngineProxy->setClientRole(agora::rtc::CLIENT_ROLE_TYPE::CLIENT_ROLE_BROADCASTER);
}

void AAgoraVoiceActor::LeaveChannel()
{
	UE_LOG(LogTemp, Warning, TEXT("AAgoraVoiceActor LeaveChannel"));
	RtcEngineProxy->leaveChannel();
}

void AAgoraVoiceActor::ToggleMute(bool bMute)
{
	UE_LOG(LogTemp, Warning, TEXT("AAgoraVoiceActor ToggleMute %d"), bMute);
	RtcEngineProxy->muteLocalAudioStream(bMute);
}

void AAgoraVoiceActor::onUserJoined(agora::rtc::uid_t uid, int elapsed)
{
	UE_LOG(LogTemp, Warning, TEXT("AAgoraVoiceActor::onUserJoined uid: %u"), uid);
	AsyncTask(ENamedThreads::GameThread, [=]()
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Blue, FString::Printf(TEXT("UVoiceChatComponent::onUserJoined uid: %u"), uid));
		});
}

void AAgoraVoiceActor::onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed)
{
	UE_LOG(LogTemp, Warning, TEXT("AAgoraVoiceActor::JoinChannelSuccess uid: %u"), uid);
	AsyncTask(ENamedThreads::GameThread, [=]()
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Blue, FString::Printf(TEXT("UVoiceChatComponent::JoinChannelSuccess uid: %u"), uid));
		});
}

void AAgoraVoiceActor::onLeaveChannel(const RtcStats& stats)
{
	UE_LOG(LogTemp, Warning, TEXT("AAgoraVoiceActor::onLeaveChannel duration: %d"), stats.duration);
}

void AAgoraVoiceActor::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
{
	UE_LOG(LogTemp, Warning, TEXT("AAgoraVoiceActor::onUserOffline uid: %u, reason: %s"), uid, reason);
	AsyncTask(ENamedThreads::GameThread, [=]()
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Blue, FString::Printf(TEXT("AAgoraVoiceActor::onUserOffline uid: %u, muted: %d"), uid, reason));
		});
}

void AAgoraVoiceActor::onUserMuteAudio(uid_t uid, bool muted)
{
	UE_LOG(LogTemp, Warning, TEXT("AAgoraVoiceActor::onUserMuteAudio uid: %u, muted: %b"), uid, muted);
	AsyncTask(ENamedThreads::GameThread, [=]()
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Blue, FString::Printf(TEXT("AAgoraVoiceActor::onUserMuteAudio uid: %u, muted: %d"), uid, muted));
		});
}

void AAgoraVoiceActor::onConnectionLost()
{
	UE_LOG(LogTemp, Warning, TEXT("AAgoraVoiceActor::onConnectionLost"));
}

void AAgoraVoiceActor::onError(int err, const char* msg)
{
	UE_LOG(LogTemp, Warning, TEXT("AAgoraVoiceActor::onError err: %d, msg: %s"), err, msg);
	AsyncTask(ENamedThreads::GameThread, [=]()
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Red, FString::Printf(TEXT("UVoiceChatComponent::onError err: %d, msg: %s"), err, msg));
		});
}

void AAgoraVoiceActor::onRemoteAudioStateChanged(uid_t remoteUid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed)
{
	UE_LOG(LogTemp, Warning, TEXT("UVoiceChatComponent::onRemoteAudioStateChanged remoteUid: %d, msg: %s"), remoteUid, state);
	AsyncTask(ENamedThreads::GameThread, [=]()
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Blue, FString::Printf(TEXT("UVoiceChatComponent::onRemoteAudioStateChanged remoteUid: %d, msg: %s"), remoteUid, state));
		});
}

void AAgoraVoiceActor::onAudioVolumeIndication(const AudioVolumeInfo* speakers, unsigned int speakerNumber, int totalVolume)
{
	UE_LOG(LogTemp, Warning, TEXT("AAgoraVoiceActor::onAudioVolumeIndication spaker: %u, msg: %d"), speakers->uid, speakerNumber);
}

void AAgoraVoiceActor::onRtcStats(const RtcStats& stats)
{
	UE_LOG(LogTemp, Warning, TEXT("AAgoraVoiceActor::onRtcStats spaker: %u, msg: %u"), stats.userCount, stats.duration);
}

void AAgoraVoiceActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ReleaseAgoraEngine();
}

void AAgoraVoiceActor::InitAgoraEngine()
{
	UE_LOG(LogTemp, Warning, TEXT("AAgoraVoiceActor InitAgoraEngine"));

	agora::rtc::RtcEngineContext RtcEngineContext;

	RtcEngineContext.appId = TCHAR_TO_ANSI(*APP_ID);
	RtcEngineContext.eventHandler = this;
	RtcEngineContext.channelProfile = agora::CHANNEL_PROFILE_TYPE::CHANNEL_PROFILE_LIVE_BROADCASTING;


	AppId = APP_ID;
	Token = TOKEN;
	ChannelName = CHANNEL_NAME;

	RtcEngineProxy = agora::rtc::ue::createAgoraRtcEngine();

	int sucess = RtcEngineProxy->initialize(RtcEngineContext);

	UE_LOG(LogTemp, Warning, TEXT("AAgoraVoiceActor InitAgoraEngine Sucess %d"), sucess);
}

void AAgoraVoiceActor::ReleaseAgoraEngine()
{
	UE_LOG(LogTemp, Warning, TEXT("AAgoraVoiceActor ReleaseAgoraEngine"));
	if (RtcEngineProxy != nullptr)
	{
		RtcEngineProxy->release();
		delete RtcEngineProxy;
		RtcEngineProxy = nullptr;
	}
}

