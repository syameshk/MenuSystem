// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerMenu.h"
#include "Components/Button.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsSubsystem.h"


void UMultiplayerMenu::Init(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath, FName NewSessionName)
{
	SessionName = NewSessionName;
	PathToLobby = FString::Printf(TEXT("%s?listen"),*LobbyPath);
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);

			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerSessionName = SessionName;
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSession);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

bool UMultiplayerMenu::Initialize()
{
	if (!Super::Initialize()) {
		return false;
	}
	if (HostButton) {
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}

	if (JoinButton) {
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMultiplayerMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTearDown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UMultiplayerMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		MultiplayerSessionsSubsystem->Log(*FString::Printf(TEXT("Session Created!")));
		UWorld* World = GetWorld();
		if (World) {
			World->ServerTravel(PathToLobby);
		}
	}
	else {
		MultiplayerSessionsSubsystem->Log(*FString::Printf(TEXT("Session Creation Failed!")));
		//Enable button if we fail
		HostButton->SetIsEnabled(true);
	}
}

void UMultiplayerMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful)
{
	if (!MultiplayerSessionsSubsystem)
		return;

	for (auto Result : SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;
		//Get the MatchType Value
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		//Log Details
		MultiplayerSessionsSubsystem->Log(*FString::Printf(TEXT("Id: %s, User: %s"), *Id, *User));
		//Check for Matchtype value
		if (SettingsValue == MatchType)
		{
			//Log Details
			MultiplayerSessionsSubsystem->Log(*FString::Printf(TEXT("Joining Match Type: %s"), *MatchType));
			//Join the session
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}

	if (!bWasSuccessful || SearchResults.Num() <= 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMultiplayerMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (!MultiplayerSessionsSubsystem)
		return;

	FString Address = MultiplayerSessionsSubsystem->GetJoinedSessionAddress();
	if (!Address.IsEmpty())
	{
		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (PlayerController)
		{
			MultiplayerSessionsSubsystem->Log(*FString::Printf(TEXT("ClientTravel. Address: %s"), *Address));
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}

	if (Address.IsEmpty() || Result != EOnJoinSessionCompleteResult::Success)
	{
		MultiplayerSessionsSubsystem->Log(*FString::Printf(TEXT("Failed. Address: %s, State %d"), *Address, Result));
		JoinButton->SetIsEnabled(true);
	}
}

void UMultiplayerMenu::OnDestroySession(bool bWasSuccessful)
{

}

void UMultiplayerMenu::OnStartSession(bool bWasSuccessful)
{

}

void UMultiplayerMenu::HostButtonClicked()
{

	if (MultiplayerSessionsSubsystem) 
	{
		HostButton->SetIsEnabled(false);
		MultiplayerSessionsSubsystem->Log(*FString::Printf(TEXT("Creating a session!")));
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMultiplayerMenu::JoinButtonClicked()
{
	if (MultiplayerSessionsSubsystem)
	{
		JoinButton->SetIsEnabled(false);
		MultiplayerSessionsSubsystem->Log(*FString::Printf(TEXT("Jonining a session!")));
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMultiplayerMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
