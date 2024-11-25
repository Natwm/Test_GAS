// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTSTileModifier.generated.h"

UCLASS()
class TTSHOOTER_API ATTSTileModifier : public AActor
{
	GENERATED_BODY()


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Modifiers")
	float TileCost = 1;

	
public:	
	// Sets default values for this actor's properties
	ATTSTileModifier();

	float GetTileCost() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
