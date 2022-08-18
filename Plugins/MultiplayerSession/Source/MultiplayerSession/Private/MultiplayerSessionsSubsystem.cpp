// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineIdentityInterface.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem():
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this,&ThisClass::OnStartSessionComplete))
	//,LoginCompleteDelegate(FOnLoginCompleteDelegate::CreateUObject(this, &ThisClass::OnLoginCompleteCallback))
{
	bIsLoggedIn = false;

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
		Log(*FString::Printf(TEXT("Found Subsystem %s"), *Subsystem->GetSubsystemName().ToString()));

		int num = SessionInterface->GetNumSessions();
		Log(*FString::Printf(TEXT("Number of sessions %d"), num));

		//Login();
			
	}
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!SessionInterface.IsValid())
	{
		LogWarning(*FString::Printf(TEXT("Session Interface Is Not Valid!")));
		return;
	}

	//Check if the user logged in before doing anything
	if (!bIsLoggedIn) {
		LogError(*FString::Printf(TEXT("User not logged in!")));
		MultiplayerOnCreateSessionComplete.Broadcast(false);
		return;
	}
	
	//Check if any sessions exists
	auto ExistingSession = SessionInterface->GetNamedSession(MultiplayerSessionName);
	//If exists, destroy the session
	if (ExistingSession != nullptr)
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;

		DestroySession();
		return;
	}

	//Add delegate and store the delegate so we can later remove from the list
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->BuildUniqueId = 1;

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	//TEST
	int players = GetWorld()->GetNumPlayerControllers();
	Log(*FString::Printf(TEXT("Players %d"), players));
	if (!LocalPlayer)
		Log(*FString::Printf(TEXT("Local Player Not Valid")));

	FUniqueNetIdRepl id = LocalPlayer->GetPreferredUniqueNetId();
	if (!id.IsValid()) {
		Log(*FString::Printf(TEXT("Local Player ID Not Valid")));
	}

	FUniqueNetIdPtr Ptr = LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId();
	if (!Ptr.IsValid()) {
		Log(*FString::Printf(TEXT("Local Player ID Not Valid")));
	}
	else {
		Log(*FString::Printf(TEXT("Local Player ID %s"), *Ptr.Get()->ToString()));
	}
	//END TEST
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), MultiplayerSessionName, *LastSessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
	
}

void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid())
	{
		LogWarning(*FString::Printf(TEXT("Session Interface Is Not Valid!")));
		return;
	}

	//Check if the user logged in before doing anything
	if (!bIsLoggedIn) {
		LogError(*FString::Printf(TEXT("User not logged in!")));
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	//Redister to the event
	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	//Search options
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	//Search start
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!SessionInterface.IsValid())
	{
		LogError(*FString::Printf(TEXT("Session Interface Is Not Valid!")));
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	//Join the session
	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), MultiplayerSessionName, SessionResult))
	{
		LogError(*FString::Printf(TEXT("Could not join session!")));
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
	if (!SessionInterface.IsValid())
	{
		LogError(*FString::Printf(TEXT("Session Interface Is Not Valid!")));
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}

	//Register events
	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!SessionInterface->DestroySession(MultiplayerSessionName))
	{
		LogError(*FString::Printf(TEXT("Destroy Session Failed!")));
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::StartSession()
{
}

void UMultiplayerSessionsSubsystem::Login()
{
	Log(*FString::Printf(TEXT("Trying to login!")));
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem) 
	{
		if (IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface())
		{
			FOnlineAccountCredentials Credentials;
			Credentials.Id = FString();
			Credentials.Token = FString();
			Credentials.Type = FString(TEXT("accountportal"));
			Identity->OnLoginCompleteDelegates->AddUObject(this, &UMultiplayerSessionsSubsystem::OnLoginCompleteCallback);
			Identity->Login(0, Credentials);
		}
	}
}

FString UMultiplayerSessionsSubsystem::GetJoinedSessionAddress()
{
	FString Address;
	if (!SessionInterface.IsValid())
	{
		LogError(*FString::Printf(TEXT("Session Interface Is Not Valid!")));
		return Address;
	}
	
	if (SessionInterface->GetResolvedConnectString(MultiplayerSessionName, Address))
	{
		Log(*FString::Printf(TEXT("Connect String: %s"), *Address));
	}
	return Address;
}

void UMultiplayerSessionsSubsystem::OnLoginCompleteCallback(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	UE_LOG(LogTemp, Warning, TEXT("LoggedIn: %d"), bWasSuccessful);
	Log(*FString::Printf(TEXT("LoggedIn: %d"), bWasSuccessful));
	if (!bWasSuccessful) {
		LogError(*FString::Printf(TEXT("LoggedIn Failed. Reaseon %s"), *Error));
	}
	bIsLoggedIn = bWasSuccessful;
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		if (IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface())
		{
			Identity->ClearOnLoginCompleteDelegates(0,this);
		}
	}

	OnLoginComplete.Broadcast(LocalUserNum, bWasSuccessful, UserId, Error);
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	//Raise events
	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		Log(*FString::Printf(TEXT("No sessions available! Search Success ? %s"), bWasSuccessful? TEXT("true") : TEXT("true")));
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	MultiplayerOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface) {
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}
	Log(*FString::Printf(TEXT("Join Session Completeed. SessionName: %s"), *SessionName.ToString()));
	MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	if (bWasSuccessful && bCreateSessionOnDestroy) {
		bCreateSessionOnDestroy = false;
		CreateSession(LastNumPublicConnections, LastMatchType);
	}

	//Send event out
	MultiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}



void UMultiplayerSessionsSubsystem::Log(FString message)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, message);
	}
}

void UMultiplayerSessionsSubsystem::LogWarning(FString message)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, message);
	}
}

void UMultiplayerSessionsSubsystem::LogError(FString message)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, message);
	}
}
