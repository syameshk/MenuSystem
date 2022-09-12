// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "SessionWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSION_API USessionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Init(FString HomePath = FString(TEXT("/Game/Maps/StartUp")));

	UFUNCTION(BlueprintCallable)
	void Setup();

	UFUNCTION(BlueprintCallable)
	void Login(int32 LoginMethod = 0);

	UFUNCTION(BlueprintCallable)
	void HostSession(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")), FName NewSessionName = FName(TEXT("GameSession")));

	UFUNCTION(BlueprintCallable)
	void JoinSession(FString TypeOfMatch = FString(TEXT("FreeForAll")), FName NewSessionName = FName(TEXT("GameSession")));

	UFUNCTION(BlueprintCallable)
	void LeaveSession();

	UFUNCTION(BlueprintCallable)
	void SwitchSession(FString TypeOfMatch = FString(TEXT("FreeForAll")), FName NewSessionName = FName(TEXT("GameSession")));

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	//Callbacks for multiplayer session
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);


private:
	void MenuTearDown();

	//The session subsystem
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 NumPublicConnections{ 16 };
	FString MatchType{ TEXT("FreeForAll") };
	FString PathToLobby{ TEXT("") };
	FString PathToHome{ TEXT("/Game/Maps/StartUp") };
	FName SessionName{ NAME_GameSession };
	bool bIsSwitchSession{ false };
};
