// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TTSPlayerController.generated.h"

class ATTSGridManager;
class UDamageTextComponent;
struct FGameplayTag;
class UAuraInputConfig;
class UAuraAbilitySystemComponent;
class UInputMappingContext;
class UInputAction;
class IEnemyInterface;
class USplineComponent;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class TTSHOOTER_API ATTSPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupInputComponent() override;
	
private:
	UPROPERTY(EditAnywhere, Category= "Input" )
	TObjectPtr<UInputMappingContext> TTSContext;

	UPROPERTY(EditAnywhere, Category= "Input" )
	TObjectPtr<UInputAction> ClickSelection;

	//Reference
	TObjectPtr<ATTSGridManager> Grid;
	
	//Tile Elements
	int32 CurrentHoveredTileIndex = -1;
	TArray<int32> SelectedTileIndex;
	

	//Flag
	bool bCanDoMultipleSelection = false;
	
	// Collision
	FHitResult Hit;
	

private:
	void CursorTrace();
	void SelectLocationToAction();
};
