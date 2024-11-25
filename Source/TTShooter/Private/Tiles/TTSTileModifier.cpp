// Fill out your copyright notice in the Description page of Project Settings.


#include "Tiles/TTSTileModifier.h"

// Sets default values
ATTSTileModifier::ATTSTileModifier()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
}

float ATTSTileModifier::GetTileCost() const
{
	return TileCost;
}

// Called when the game starts or when spawned
void ATTSTileModifier::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATTSTileModifier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

