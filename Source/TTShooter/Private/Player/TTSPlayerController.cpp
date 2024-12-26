// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TTSPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Character/TTSBaseCharacter.h"
#include "Grid/TTSGridManager.h"
#include "Input/TTSInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TTShooter/TTShooter.h"

void ATTSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(TTSContext);
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem;
	Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if(Subsystem)
	{
		Subsystem->AddMappingContext(TTSContext,0);
	}
	
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock) ;
	InputModeData.SetHideCursorDuringCapture(false);
	
	SetInputMode(InputModeData) ;

	Grid =  Cast<ATTSGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATTSGridManager::StaticClass()));

}

void ATTSPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CursorTraceTileUnderCursor();
	CursorTraceUnitUnderCursor();
}

void ATTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UTTSInputComponent* AuraInputComponent = CastChecked<UTTSInputComponent>(InputComponent);

	AuraInputComponent->BindAction(ClickSelection, ETriggerEvent::Triggered,this, &ATTSPlayerController::SelectTileToAction);
	AuraInputComponent->BindAction(ClickInfoSelection, ETriggerEvent::Triggered,this, &ATTSPlayerController::PrintTileNumber);
}

void ATTSPlayerController::CursorTraceTileUnderCursor()
{
	GetHitResultUnderCursor(ECC_GRID, false, Hit);
	if(!Hit.bBlockingHit)
		return;
	
	int32 TargetedTile = Grid->GetTileIndexFromLocation(Hit.Location);

	if (CurrentHoveredTileIndex == TargetedTile)
	{
		return;
	}

	Grid->UpdateTileState(CurrentHoveredTileIndex, ETileState::HOVERED, true);

	//Add State Hovered on the last hovered Tile
	CurrentHoveredTileIndex =  TargetedTile;
	Grid->UpdateTileState(CurrentHoveredTileIndex, ETileState::HOVERED);

	FVector test = Grid->GetTileLocationUnderCursor(TargetedTile);
	UKismetSystemLibrary::DrawDebugSphere(GetWorld(),test,10,12);
}

void ATTSPlayerController::CursorTraceUnitUnderCursor()
{
	GetHitResultUnderCursor(ECC_UNIT, false, Hit);
	if(!Hit.bBlockingHit)
		return;

	FVector a = Hit.HitObjectHandle.GetLocation();
	int32 TargetedTile = Grid->GetTileIndexFromLocation(a);

	if (CurrentHoveredUnitIndex == TargetedTile)
	{
		return;
	}

	if (auto unit = Grid->GetTileUnitFromIndex(TargetedTile) )
	{
		if (CurrentHoveredUnitIndex >=0)
		{
			unit->UnHighlightActor();
		}
		//Add State Hovered on the last hovered Tile
		CurrentHoveredUnitIndex =  TargetedTile;
		
		Grid->GetTileUnitFromIndex(CurrentHoveredUnitIndex)->HighlightActor();
	}
}

void ATTSPlayerController::SelectTileToAction()
{
	if (ATTSBaseCharacter* SelectedCharacter = Grid->GetTileUnitFromIndex(CurrentHoveredTileIndex))
	{
		//add check on team
		SelectedUnitAtTileIndex = CurrentHoveredTileIndex;
	}
	//Test Path Finding
	/*if ( Grid->GetPathFinding()->StartIndex < 0)
		Grid->GetPathFinding()->StartIndex = CurrentHoveredTileIndex;
	else if (Grid->GetPathFinding()->StartIndex != CurrentHoveredTileIndex)
	{
		Grid->GetPathFinding()->TargetIndex = CurrentHoveredTileIndex;
		TArray<int32> a = Grid->GetPathFinding()->FindPath(Grid->GetPathFinding()->StartIndex,Grid->GetPathFinding()->TargetIndex,false,Grid);
		a.Add(CurrentHoveredTileIndex);
		a.Add(Grid->GetPathFinding()->StartIndex);
		for (auto A : a)
		{
			Grid->UpdateTileState(A, ETileState::PATH, false);
		}
	}*/

	//Test Multiple selection
	/*if (!bCanDoMultipleSelection)
	{
		for (int32 TileIndex : SelectedTileIndex)
		{
			Grid->UpdateTileState(TileIndex, ETileState::SELECTED, true);
		}
		SelectedTileIndex.Empty();
	}
	SelectedTileIndex.Add(CurrentHoveredTileIndex);

	for (int32 TileIndex : SelectedTileIndex)
	{
		Grid->UpdateTileState(TileIndex, ETileState::SELECTED);
	}*/

}

void ATTSPlayerController::PrintTileNumber()
{
	CurrentHoveredTileIndex;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("World delta for current frame equals %d"), CurrentHoveredTileIndex));
}
