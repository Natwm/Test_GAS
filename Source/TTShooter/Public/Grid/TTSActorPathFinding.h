// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TTSActorPathFinding.generated.h"
class ATTSGridManager;

USTRUCT(BlueprintType)
struct FPathData
{
	GENERATED_BODY()
	
	FPathData()
		: Index(-1), CostFromStart(999), MinCostToTarget(999), TileCost(1), ParentTileIndex(-1)
	{
	}

	FPathData(int32 PathIndex,int32 CostStart,int32 MinCost,int32 CurrentTileCost, int32 PreviousIndex = -1)
	{
		Index = PathIndex;
		CostFromStart = CostStart;
		MinCostToTarget = MinCost;
		TileCost = CurrentTileCost;
		ParentTileIndex = PreviousIndex;
	}

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 Index = -1;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 CostFromStart = 999;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 MinCostToTarget = 999;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 TileCost = 1;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 ParentTileIndex = -1;
};
/**
 * 
 */

UCLASS()
class TTSHOOTER_API ATTSActorPathFinding : public AActor
{
	GENERATED_BODY()
	
private:
	TObjectPtr<ATTSGridManager> GridGestionManager;

	TArray<int32> DiscoverTilesIndexes;
	TArray<int32> AnalysedTilesIndexes;
	TArray<int32> DiscoverTilesSortingCost;

	FPathData CurrentDiscoveredTile;
	TArray<FPathData> CurrentNeighbors;

	TMap<int32,FPathData> PathfindingData;

	FPathData CurrentNeighbor;
	
	bool bIncludeDiagonals = false;

public:	
	int32 StartIndex = -1;
	int32 TargetIndex = -1;

public:
	TArray<FPathData> GetValidTileNeighbors(int32 TileIndex);
	TArray<int32> FindPath(int32 StartPosIndex, int32 EndPosIndex, bool bDiagonals,ATTSGridManager* GridManager);

private:
	TMap<int32,FPathData>RunPathFinding(int32 Index, int32 EndIndex, ATTSGridManager* GridManager);

	//TArray<FPathData> SearchAndAddAdjacentTiles(int32 searchStep,TMap<int32,FPathData>Path,int32 Move, TArray<FPathData> OpenListTiles, TArray<FPathData> DelayedSearchTiles);


	bool IsInputDataValid(int32 StartPosIndex, int32 EndPosIndex);

	void DiscoverTile(FPathData TilePath);
	bool DiscoverNextNeighbor();

	int32 GetMinimumCostBtwTwoTiles(int32 FirstPosIndex, int32 SecondPosIndex, bool bDiagonals);

	bool AnalyseNextDiscoveredTile();

	TArray<int32> GeneratePath();

	FPathData PullCheapestTileOutDiscoverList();
	
	TArray<int32> GetNeihborsIndexes(int32 index,int32 GridWidth, int32 GridHeight, bool bCanDoDiagonal);

	void InsertTileInDiscoveredArray(FPathData TileData);

	void ClearGeneratedData();
};
