// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/TTSGameModeBase.h"
#include "TTSDestroyAllCreatureGameMode.generated.h"

class ATTSBaseCharacter;
/**
 * 
 */
UCLASS()
class TTSHOOTER_API ATTSDestroyAllCreatureGameMode : public ATTSGameModeBase
{
	GENERATED_BODY()

	ATTSDestroyAllCreatureGameMode();
private :
	TArray<ATTSBaseCharacter*> AllyTeamCharacters;
	TArray<ATTSBaseCharacter*> EnemyTeamCharacters;

	
public:
	void UpdateGridCharacterData(TObjectPtr<ATTSBaseCharacter> Character, bool bCharacterIsDead, bool bIsAlly);

private:
	void RemoveCharacterFromMap(TObjectPtr<ATTSBaseCharacter> Character,bool bIsAlly);
	void AddCharacterFromMap(TObjectPtr<ATTSBaseCharacter> Character,bool bIsAlly);

	bool IsAllEnemyIsDead() const;
	bool IsAllAllyIsDead() const;
};
