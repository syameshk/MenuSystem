// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (GameState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		Log(FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers));

		APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();
		if (PlayerState)
		{
			FString PlayerName = PlayerState->GetPlayerName();
			Log(FString::Printf(TEXT("%s joined the game"), *PlayerName));
		}
	}

}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	if (GameState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
		Log(FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers-1));

		APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();
		if (PlayerState)
		{
			FString PlayerName = PlayerState->GetPlayerName();
			Log(FString::Printf(TEXT("%s left the game"), *PlayerName));
		}
	}
}

void ALobbyGameMode::Log(FString message)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, message);
	}
}
