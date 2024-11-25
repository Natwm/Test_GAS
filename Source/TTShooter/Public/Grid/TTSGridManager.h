// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTSGridManager.generated.h"

class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class TTSHOOTER_API ATTSGridManager : public AActor
{
	GENERATED_BODY()

public:
	// Création de l'objet
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> GridHolder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|Asset")
	TObjectPtr<UStaticMesh> DefaultTileMesh;

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration")
	FVector2D GridSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Physic")
	float CastRadius = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Physic")
	float CastHalfHeight = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configuration|Flag")
	bool bShowCast;
	
	
	//MAP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	TMap<int32, FVector> GridLocations;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Map")
	TMap<int32, int32> GridCost;

private:
	float TileXSize = 100;
	float TileYSize = 100;
	
public:	
	// Sets default values for this actor's properties
	ATTSGridManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private :
	//Set Up Object
	void SetUpDefaultMeshToSpawn() const;
	void FindAndStoreTileScale();

	// Set Up Grid
	UFUNCTION(BlueprintCallable, Category="Init Grid")
	void SpawnDebugTileGrid();
	
	UFUNCTION(BlueprintCallable, Category="Init Grid")
	void SpawnGridFromWorld();

	//Creation grid Methods
	void ProcessTile(int32 TileIndex, const FVector& TilePosition,	const FCollisionShape& CapsuleShape, const FCollisionQueryParams& QueryParams,	FVector HalfPos, TArray<FTransform>& ListOfTransforms);
	void HandleHits(int32 TileIndex, const TArray<FHitResult>& Hits, TArray<FTransform>& ListOfTransforms, const FVector& TilePosition);
	bool DetermineTileCostAndValidity(const TArray<FHitResult>& Hits, int32& OutTileCost);

	
	// Convert Location and Grid
	FVector ConvertFromGridToWorld(FVector Entry) const;
	FVector ConvertFromWorldToGrid(FVector Entry) const;
	TArray<FVector> ConvertToGridIndexesLocation(TArray<int32> Indexes, FVector offset );
	int32 ConvertGridCoordsToGridIndex(FVector Coord) const;
	FVector ConvertIndexToWorldPosition (int32 GridIndex, int32 IndexX, int32 IndexZ, float HeightBetweenLevels) const;
	FVector ConvertIndexToLocation(int32 Index, float ZOffset);

	// Getter On World And Grid
	FVector GetTileLocationFromIndex(int32 Index, int32 Row, int32 Column) const;
	FVector GetOffsetWorldLocationAtIndex(int32 GridIndex, float offset);

	//Map Gettter
	FVector GetTileLocationFromMap(int32 Index) const;
	int32 GetTileCostFromMap(int32 Index) const;
	
	// Map Function
	void AddTileToMaps(int32 GridIndex, FVector TileLocation, int32 TileCost);
	void AddTileToLocationMap(int32 TileIndex, FVector TileLocation);
	void AddTileCostToCostMap(int32 TileIndex, int32 TileCost);

	// Calcule de distance
	bool CanCrossDistance(int32 TileAIndex, int32 TileBIndex, int32 MaxDistance, bool bCanDoDiagonal);
	float GetDistanceBtwTwoTiles_Manhattan(int32 TileAIndex,int32 TileBIndex);
	float GetDistanceBtwTwoTiles_Euclidienne(int32 TileAIndex,int32 TileBIndex);
	
	// Calcule des voisins
	TArray<int32> GetSelectedTilesNeighbors(int32 TileIndex,const int32 GridWidth, const int32 GridHeight, bool bCanDoDiagonal);
};
