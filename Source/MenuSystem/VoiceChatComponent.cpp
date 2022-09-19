// Fill out your copyright notice in the Description page of Project Settings.


#include "VoiceChatComponent.h"

// Sets default values for this component's properties
UVoiceChatComponent::UVoiceChatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UVoiceChatComponent::InitAgoraEngine()
{
	agora::rtc::RtcEngineContext RtcEngineContext;
	std::string APP_IDStr(TCHAR_TO_ANSI(*APP_ID));
	AppID = APP_IDStr;
	std::string TOKENStr(TCHAR_TO_ANSI(*TOKEN));
	Token = TOKENStr;
	std::string CHANNEL_NAMEStr(TCHAR_TO_ANSI(*CHANNEL_NAME));
	ChannelName = CHANNEL_NAMEStr;

	RtcEngineContext.appId = APP_IDStr.c_str();
	RtcEngineContext.eventHandler = this;
	RtcEngineContext.channelProfile = agora::CHANNEL_PROFILE_TYPE::CHANNEL_PROFILE_LIVE_BROADCASTING;

	RtcEngineProxy = agora::rtc::ue::createAgoraRtcEngine();
	RtcEngineProxy->initialize(RtcEngineContext);
}

void UVoiceChatComponent::ReleaseAgoraEngine()
{
	if (RtcEngineProxy != nullptr)
	{
		RtcEngineProxy->release();
		delete RtcEngineProxy;
		RtcEngineProxy = nullptr;
	}
}

void UVoiceChatComponent::JoinChannel()
{
	UE_LOG(LogTemp, Warning, TEXT("UVoiceChatComponent JoinChannel"));

	RtcEngineProxy->enableAudio();
	RtcEngineProxy->joinChannel(Token.c_str(), ChannelName.c_str(), "", 0);
	RtcEngineProxy->setClientRole(agora::rtc::CLIENT_ROLE_TYPE::CLIENT_ROLE_BROADCASTER);
	UE_LOG(LogTemp, Warning, TEXT("UVoiceChatComponent JoinChannel End"));
}

void UVoiceChatComponent::LeaveChannel()
{
	UE_LOG(LogTemp, Warning, TEXT("UVoiceChatComponent LeaveChannel"));

	RtcEngineProxy->leaveChannel();
}

void UVoiceChatComponent::ToggleMute(bool bMute)
{
	RtcEngineProxy->muteLocalAudioStream(bMute);
}

void UVoiceChatComponent::onUserJoined(agora::rtc::uid_t uid, int elapsed)
{
	UE_LOG(LogTemp, Warning, TEXT("UVoiceChatComponent::onUserJoined uid: %u"), uid);
	AsyncTask(ENamedThreads::GameThread, [=]()
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Blue, FString::Printf(TEXT("UVoiceChatComponent::onUserJoined uid: %u"), uid));
		});
}

void UVoiceChatComponent::onJoinChannelSuccess(const char* channel, agora::rtc::uid_t uid, int elapsed)
{
	UE_LOG(LogTemp, Warning, TEXT("UVoiceChatComponent::JoinChannelSuccess uid: %u"), uid);
	AsyncTask(ENamedThreads::GameThread, [=]()
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Blue, FString::Printf(TEXT("UVoiceChatComponent::JoinChannelSuccess uid: %u"), uid));
		});
}

void UVoiceChatComponent::onError(int err, const char* msg)
{
	UE_LOG(LogTemp, Warning, TEXT("UVoiceChatComponent::onError err: %d, msg: %s"), err, msg);
	AsyncTask(ENamedThreads::GameThread, [=]()
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Red, FString::Printf(TEXT("UVoiceChatComponent::onError err: %d, msg: %s"), err, msg));
		});
}

void UVoiceChatComponent::onRemoteAudioStateChanged(const RtcConnection& connection, uid_t remoteUid, REMOTE_AUDIO_STATE state, REMOTE_AUDIO_STATE_REASON reason, int elapsed)
{
	UE_LOG(LogTemp, Warning, TEXT("UVoiceChatComponent::onRemoteAudioStateChanged remoteUid: %d, msg: %s"), remoteUid, state);
	AsyncTask(ENamedThreads::GameThread, [=]()
		{
			GEngine->AddOnScreenDebugMessage(-1, 30.f, FColor::Blue, FString::Printf(TEXT("UVoiceChatComponent::onRemoteAudioStateChanged remoteUid: %d, msg: %s"), remoteUid, state));
		});
}

// Called when the game starts
void UVoiceChatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UVoiceChatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

}

