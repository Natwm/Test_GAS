// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TTSCharacterBase.h"

// Sets default values
ATTSCharacterBase::ATTSCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATTSCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATTSCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATTSCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

