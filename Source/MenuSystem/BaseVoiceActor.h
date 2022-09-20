// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseVoiceActor.generated.h"

UCLASS()
class MENUSYSTEM_API ABaseVoiceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseVoiceActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual bool IsVoiceConnected();
	virtual bool GetMuteStatus();
	virtual bool Mute(bool bMute);

};
