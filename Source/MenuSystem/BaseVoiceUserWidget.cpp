// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseVoiceUserWidget.h"

void UBaseVoiceUserWidget::Init(ABaseVoiceActor* source)
{
	BaseVoice = source;
	if (BaseVoice == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("UBaseVoiceUserWidget::Init BaseVoice is Null"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("UBaseVoiceUserWidget::Init BaseVoice initialized"));
	}
}

bool UBaseVoiceUserWidget::IsVoiceConnected()
{
	if (BaseVoice == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBaseVoiceUserWidget::IsVoiceConnected Voice not initilaized"));
		return false;
	}
	return BaseVoice->IsVoiceConnected();
}

bool UBaseVoiceUserWidget::GetMuteStatus()
{
	if (BaseVoice == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBaseVoiceUserWidget::GetMuteStatus Voice not initilaized"));
		return false;
	}
	return BaseVoice->GetMuteStatus();
}

bool UBaseVoiceUserWidget::ToggleMute(bool bMute)
{
	if (BaseVoice == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UBaseVoiceUserWidget::GetMuteStatus Voice not initilaized"));
		return false;
	}
	return BaseVoice->Mute(bMute);
}
