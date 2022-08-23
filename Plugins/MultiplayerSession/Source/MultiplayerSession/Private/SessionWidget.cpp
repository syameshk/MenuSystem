// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsSubsystem.h"

void USessionWidget::Init()
{
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

void USessionWidget::Setup() {
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
}

void USessionWidget::Login(int32 LoginMethod)
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->Log(*FString::Printf(TEXT("Login requested %d"), LoginMethod));
		MultiplayerSessionsSubsystem->Login(LoginMethod);
	}
}

void USessionWidget::HostSession(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath, FName NewSessionName) {
	SessionName = NewSessionName;
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->Log(*FString::Printf(TEXT("Creating a session: %s"), *MatchType));
		MultiplayerSessionsSubsystem->MultiplayerSessionName = SessionName;
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void USessionWidget::JoinSession(FString TypeOfMatch, FName NewSessionName) {
	SessionName = NewSessionName;
	MatchType = TypeOfMatch;

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->Log(*FString::Printf(TEXT("Jonining a session %s"), *MatchType));
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void USessionWidget::LeaveSession()
{
	bIsSwitchSession = false;
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->Log(*FString::Printf(TEXT("Leaving a session %s"), *MatchType));
		//MultiplayerSessionsSubsystem->FindSessions(10000);
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void USessionWidget::SwitchSession(FString TypeOfMatch, FName NewSessionName)
{
	bIsSwitchSession = true;
	SessionName = NewSessionName;
	MatchType = TypeOfMatch;

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->Log(*FString::Printf(TEXT("Leaving a session %s"), *MatchType));
		//MultiplayerSessionsSubsystem->FindSessions(10000);
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

bool USessionWidget::Initialize()
{
	if (!Super::Initialize()) {
		return false;
	}
	return true;
}

void USessionWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTearDown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void USessionWidget::OnCreateSession(bool bWasSuccessful)
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
	}
}

void USessionWidget::OnFindSession(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful)
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
		//JoinButton->SetIsEnabled(true);
	}
}

void USessionWidget::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
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
		//JoinButton->SetIsEnabled(true);
	}
}

void USessionWidget::OnDestroySession(bool bWasSuccessful)
{
	MultiplayerSessionsSubsystem->Log(*FString::Printf(TEXT("Session Destroyed!")));

	if(bIsSwitchSession)
	{
		JoinSession(MatchType, SessionName);
	}
	else
	{
		//Load menu level
		UWorld* World = GetWorld();
		if (World) {
			World->ServerTravel(PathToHome);
		}
	}
}

void USessionWidget::OnStartSession(bool bWasSuccessful)
{

}

void USessionWidget::MenuTearDown()
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
