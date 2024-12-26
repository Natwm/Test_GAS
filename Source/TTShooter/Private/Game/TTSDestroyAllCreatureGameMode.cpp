// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/TTSDestroyAllCreatureGameMode.h"

ATTSDestroyAllCreatureGameMode::ATTSDestroyAllCreatureGameMode()
{
	AllyTeamCharacters = TArray<ATTSBaseCharacter*>();
	EnemyTeamCharacters = TArray<ATTSBaseCharacter*>();
}

void ATTSDestroyAllCreatureGameMode::UpdateGridCharacterData(TObjectPtr<ATTSBaseCharacter> Character, bool bCharacterIsDead,
                                                             bool bIsAlly)
{
	if (bCharacterIsDead)
	{
		RemoveCharacterFromMap(Character,bIsAlly);
	}
	else
	{
		AddCharacterFromMap(Character,bIsAlly);
		UE_LOG(LogTemp, Warning, TEXT("Updated Team Data -> Ally: %d, Enemy: %d"), 
				AllyTeamCharacters.Num(), 
				EnemyTeamCharacters.Num());	}
}

void ATTSDestroyAllCreatureGameMode::RemoveCharacterFromMap(TObjectPtr<ATTSBaseCharacter> Character, bool bIsAlly)
{
	if (bIsAlly)
	{
		if (AllyTeamCharacters.Contains(Character))
		{
			AllyTeamCharacters.Remove(Character);
			if (IsAllAllyIsDead())
			{
				UE_LOG(LogTemp, Warning, TEXT("GameOver"));
			}
		}
		
	}
	else
	{
		if (EnemyTeamCharacters.Contains(Character))
		{
			EnemyTeamCharacters.Remove(Character);
			if (IsAllEnemyIsDead())
			{
				UE_LOG(LogTemp, Warning, TEXT("Victory"));
			}
		}
	}
}

void ATTSDestroyAllCreatureGameMode::AddCharacterFromMap(TObjectPtr<ATTSBaseCharacter> Character, bool bIsAlly)
{
	if (bIsAlly)
	{
		AllyTeamCharacters.AddUnique(Character);
	}
	else
		EnemyTeamCharacters.AddUnique(Character);
}

bool ATTSDestroyAllCreatureGameMode::IsAllEnemyIsDead() const
{
	return EnemyTeamCharacters.Num() > 0;
}

bool ATTSDestroyAllCreatureGameMode::IsAllAllyIsDead() const
{
	return AllyTeamCharacters.Num() > 0;

}