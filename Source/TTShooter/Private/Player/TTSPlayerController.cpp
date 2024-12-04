// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TTSPlayerController.h"

#include "EnhancedInputSubsystems.h"
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
	CursorTrace();
}

void ATTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UTTSInputComponent* AuraInputComponent = CastChecked<UTTSInputComponent>(InputComponent);

	AuraInputComponent->BindAction(ClickSelection, ETriggerEvent::Triggered,this, &ATTSPlayerController::SelectLocationToAction);
}

void ATTSPlayerController::CursorTrace()
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

void ATTSPlayerController::SelectLocationToAction()
{
	if (!bCanDoMultipleSelection)
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
	}

}
