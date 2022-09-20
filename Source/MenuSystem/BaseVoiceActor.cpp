// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseVoiceActor.h"

// Sets default values
ABaseVoiceActor::ABaseVoiceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseVoiceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseVoiceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ABaseVoiceActor::IsVoiceConnected()
{
	return false;
}

bool ABaseVoiceActor::GetMuteStatus()
{
	return false;
}

bool ABaseVoiceActor::Mute(bool bMute)
{
	return false;
}

