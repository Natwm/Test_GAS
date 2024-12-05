// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTSGridManager.generated.h"

class UHierarchicalInstancedStaticMeshComponent;
class UMaterialInterface;
class APlayerController;

UENUM(Blueprintable)
enum class ETileState : uint8
{
	NONE,
	HOVERED,
	SELECTED,
	NEIGHBOUR
};

USTRUCT(BlueprintType, Blueprintable)
struct FTileData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 TileIndex = 0;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
    int32 TileCost = 0;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FTransform TileTransform = FTransform();
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FVector TileLocation = FVector();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<ETileState> TileState ;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<int32> TileNeighbour ;
};

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration|Asset")
	TObjectPtr<UMaterialInterface> DefaultTileMaterial;

	
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
	TMap<int32, FTileData> GridData;

private:	
	float TileXSize = 100;
	float TileYSize = 100;
	float TileZSize = 100;
	FVector const ErrorVector = FVector(-100000000000000000,-1111111000,-1000000000000);
	int32 const ErrorInt = -1111;

public:
	FVector GetErrorVector() const
	{
		return ErrorVector;
	}

	int32 GetErrorInt() const
	{
		return ErrorInt;
	}

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

	void UpdateTileVisual(int32 TileIndex);
	FColor GetTileVisualDependingState(TArray<ETileState> TileStates,float* IsFill);

	//Creation grid Methods
	void ProcessTile(int32 TileIndex, const FVector& TilePosition,	const FCollisionShape& CapsuleShape, const FCollisionQueryParams& QueryParams,	FVector HalfPos, TArray<FTransform>& ListOfTransforms);
	void HandleHits(int32 TileIndex, const TArray<FHitResult>& Hits, TArray<FTransform>& ListOfTransforms, const FVector& TilePosition);
	bool DetermineTileCostAndValidity(const TArray<FHitResult>& Hits, int32& OutTileCost);
	
	// Convert Location and Grid
	FVector ConvertFromGridToWorld(FVector Entry) const;
	FVector ConvertFromWorldToGrid(FVector Entry) const;
	TArray<FVector> ConvertToGridIndexesLocation(TArray<int32> Indexes, FVector offset );
	UFUNCTION(BlueprintCallable, Category="Cursor")
	int32 ConvertGridCoordsToGridIndex(const FVector& Coord) const;
	FVector ConvertIndexToWorldPosition (int32 GridIndex, int32 IndexX, int32 IndexZ, float HeightBetweenLevels) const;
	FVector ConvertIndexToLocation(int32 Index, float ZOffset);

	// Getter On World And Grid
	FVector GetTileLocationFromIndex(int32 Index, int32 Row, int32 Column) const;
	FVector2d GetTilePosOnGridFromLocation(FVector TileLocation) const;
	FVector GetOffsetWorldLocationAtIndex(int32 GridIndex, float Offset);

	//Map Gettter
	FVector GetTileLocationFromMap(int32 Index) const;
	int32 GetTileCostFromMap(int32 Index) const;
	
	// Map Function
	void AddTileToMaps(int32 GridIndex, FVector TileLocation,FTransform TileTransform, int32 TileCost);
	void AddTileToLocationMap(int32 TileIndex, FVector TileLocation);
	void AddTileCostToCostMap(int32 TileIndex, int32 TileCost);

	// Tile Function
	void AddTileState(int32 TileIndex, ETileState StateToAdd);
	void RemoveTileState(int32 TileIndex, ETileState StateToAdd);
	
	// Calcule de distance
	bool CanCrossDistance(int32 TileAIndex, int32 TileBIndex, int32 MaxDistance, bool bCanDoDiagonal);
	float GetDistanceBtwTwoTiles_Manhattan(int32 TileAIndex,int32 TileBIndex);
	float GetDistanceBtwTwoTiles_Euclidienne(int32 TileAIndex,int32 TileBIndex);
	
	// Calcule des voisins
	TArray<int32> GetSelectedTilesNeighbors(int32 TileIndex,const int32 GridWidth, const int32 GridHeight, bool bCanDoDiagonal);

	// Mouse detection
	FVector GetCursorLocationOnGrid() const;
	FVector2D GetTileGridPosUnderCursor();
	FVector GetTileLocationUnderCursor();

public:

	int32 GetTileIndexFromLocation(FVector TileLocation);
	FVector GetTileLocationUnderCursor(int32 TileIndex);
	
	// Mouse detection
	UFUNCTION(BlueprintCallable, Category="Cursor")
	int32 GetTileIndexUnderCursor();

	// Tile Function
	void UpdateTileState(int32 TileIndex, ETileState StateToAdd, bool RemoveState = false);
	FTileData GetTileDataFromIndex(int32 Index) const;
	int32 GetTileAmountOfStateFromIndex(int32 Index);
};
