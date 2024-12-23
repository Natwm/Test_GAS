// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/TTSActorPathFinding.h"

#include "Grid/TTSGridManager.h"

TMap<int32, FPathData> ATTSActorPathFinding::RunPathFinding(int32 Index, int32 EndIndex, ATTSGridManager* GridManager)
{
	TMap<int32, FPathData> Path;
	/*TArray<int32> ReachableUnitIndexes;
	TArray<FPathData> LocalOpenListTiles;
	int32 LocalCurrentSearchStep = 0;

	/*FPathData TileUnit;
	TileUnit.TileCost = 0;
	TileUnit.ParentTileIndex = StartIndex;
	Path.Add(StartIndex, TileUnit);

	LocalOpenListTiles.Add(TileUnit);*/

	
	/*for (int32 CurrentStep = LocalCurrentSearchStep; CurrentStep < EndIndex -1; CurrentStep++)
	{
		LocalCurrentSearchStep = CurrentStep;
		
	}
	*/
	return Path;
}

/*TArray<FPathData> TTSPathFinding::SearchAndAddAdjacentTiles(int32 searchStep, TMap<int32, FPathData> Path, int32 Move,
	TArray<FPathData> OpenListTiles, TArray<FPathData> DelayedSearchTiles)
{
	TMap<int32,int32> LocalEdges;
	TArray<FPathData> LocalOpenListChildTiles;
	int32 OpenListIndex = 0;
	int32 LocalOpenListIndex;
	
	if (OpenListIndex > OpenListTiles.Num())
		return LocalOpenListChildTiles;

	if (searchStep == OpenListTiles[LocalOpenListIndex].TileCost)
	{
		//this->GridManager->GetGridData().Find(OpenListTiles[LocalOpenListIndex].);
		
		DelayedSearchTiles.Add(OpenListTiles[LocalOpenListIndex]);
	}
}*/

TArray<int32> ATTSActorPathFinding::FindPath(int32 StartPosIndex, int32 EndPosIndex, bool bDiagonals, ATTSGridManager* GridManager)
{
	this->GridGestionManager = GridManager;

	StartIndex = StartPosIndex;
	TargetIndex = EndPosIndex;
	bIncludeDiagonals = bDiagonals;
	
	ClearGeneratedData();

	if (!IsInputDataValid(StartPosIndex,EndPosIndex))
		return TArray<int32>();


	
	int32 cost = GetMinimumCostBtwTwoTiles(StartPosIndex,EndPosIndex,bDiagonals);
	FPathData TileUnit = FPathData(StartPosIndex,1,0,cost) ;
	DiscoverTile(TileUnit);

	while (DiscoverTilesIndexes.Num() > 0)
	{
		if (AnalyseNextDiscoveredTile())
		{
			return GeneratePath();
		}
	}
	return TArray<int32>();

}

bool ATTSActorPathFinding::IsInputDataValid(int32 StartPosIndex, int32 EndPosIndex)
{
	if (!GridGestionManager->GetGridData().Contains(StartPosIndex) || !GridGestionManager->GetGridData().Contains(EndPosIndex))
	{
		return false;
	}
	
	return GridGestionManager->IsTileWalkable(StartPosIndex) && GridGestionManager->IsTileWalkable(EndPosIndex);
}

void ATTSActorPathFinding::DiscoverTile(FPathData TilePath)
{
	PathfindingData.Add(TilePath.Index,TilePath);
	InsertTileInDiscoveredArray(TilePath);
}

bool ATTSActorPathFinding::DiscoverNextNeighbor()
{
	CurrentNeighbor = CurrentNeighbors[0];
	CurrentNeighbors.RemoveAt(0);

	if (AnalysedTilesIndexes.Contains(CurrentNeighbor.Index))
	{
		int32 CostFromStart = CurrentDiscoveredTile.CostFromStart + CurrentNeighbor.TileCost;
		if (CostFromStart >= CurrentNeighbor.CostFromStart)
		{
			return false;
		}
	}

	int32 CostFromStart = CurrentDiscoveredTile.CostFromStart + CurrentNeighbor.CostFromStart;

	int32 IndexInDiscovered = DiscoverTilesIndexes.Find(CurrentNeighbor.Index);

	if (IndexInDiscovered != INDEX_NONE)
	{
		CurrentNeighbor = *PathfindingData.Find(CurrentNeighbor.Index);
		if (CostFromStart < CurrentNeighbor.CostFromStart)
		{
			DiscoverTilesSortingCost.RemoveAt(IndexInDiscovered);
			DiscoverTilesIndexes.RemoveAt(IndexInDiscovered);	
		}
		else
		{
			return false;
		}
	}

	int32 MinCost = GetMinimumCostBtwTwoTiles(CurrentNeighbor.Index,TargetIndex, false);
	FPathData NewPathData = FPathData(CurrentNeighbor.Index,CostFromStart,MinCost,CurrentNeighbor.TileCost,CurrentDiscoveredTile.Index);
	DiscoverTile(NewPathData);

	return CurrentNeighbor.Index == TargetIndex;
}

int32 ATTSActorPathFinding::GetMinimumCostBtwTwoTiles(int32 FirstPosIndex, int32 SecondPosIndex, bool bDiagonals)
{
	return GridGestionManager->GetDistanceBtwTwoTiles_ManhattanWithCost(FirstPosIndex,SecondPosIndex);
}

bool ATTSActorPathFinding::AnalyseNextDiscoveredTile()
{
	CurrentDiscoveredTile = PullCheapestTileOutDiscoverList();
	CurrentNeighbors = GetValidTileNeighbors(CurrentDiscoveredTile.Index);

	while (CurrentNeighbors.Num() > 0)
	{
		if (DiscoverNextNeighbor())
			return true;
	}
	
	return false;
}

TArray<int32> ATTSActorPathFinding::GeneratePath()
{
	int32 CurrentTileIndex = TargetIndex;
	TArray<int32> InvertPath, TrajectoryPath;

	while (CurrentTileIndex != StartIndex)
	{
		InvertPath.Add(CurrentTileIndex);
		FPathData* TileData = PathfindingData.Find(CurrentTileIndex);
		if (!TileData)
		{
			// Gestion de l'erreur : Tuile sans parent
			break;
		}
		CurrentTileIndex = TileData->ParentTileIndex;
	}

	for (int32 Path : InvertPath)
	{
		TrajectoryPath.Add(Path);	
	}
	
	return TrajectoryPath;
}

FPathData ATTSActorPathFinding::PullCheapestTileOutDiscoverList()
{
	int32 TileIndex = DiscoverTilesIndexes[0];

	DiscoverTilesSortingCost.RemoveAt(0);
	DiscoverTilesIndexes.RemoveAt(0);

	AnalysedTilesIndexes.Add(TileIndex);
	
	return *PathfindingData.Find(TileIndex);
}

/*TArray<FPathData> ATTSActorPathFinding::GetValidTileNeighbors(int32 TileIndex)
{
	TArray<FPathData> Neighbors;

	int32 GridWidth =  GridGestionManager->GetGridSize().X;
	int32 GridHeight =  GridGestionManager->GetGridSize().Y;
	
	TArray<int32> NeighborsIndex = GetNeihborsIndexes(TileIndex,GridWidth,GridHeight,false);

	for (int32 Neighbor : NeighborsIndex)
	{
		int32 TileX = Neighbor % GridWidth;
		int32 TileY = Neighbor / GridWidth;

		if (GridGestionManager->GetGridData().Contains(Neighbor)/*TileX >= 0 && TileX < GridWidth && TileY >= 0 && TileY < GridHeight)
		{
			FTileData* TileData = GridGestionManager->GetGridData().Find(Neighbor);

			//Vérifier la data de la tile avant de l'ajouter dans les voisins 
			if ((TileData->TileCost >= 0 && TileData->TileCost < 99) && !TileData->TileState.Contains(ETileState::NOTWALKABLE))
			{
				FPathData Data = FPathData(Neighbor,1,999,999,TileIndex); 
				Neighbors.Add(Data);
			}

			//Todo:: Ajouter une vérification sur la hauteur.

		}

	}
	return Neighbors;
}*/

// V2
TArray<FPathData> ATTSActorPathFinding::GetValidTileNeighbors(int32 TileIndex)
{
	TArray<FPathData> Neighbors;
	TArray<int32> TileNeighbors = GridGestionManager->GetGridData().Find(TileIndex)->TileNeighbour;
	for (int32 Neighbor : TileNeighbors)
	{
		int32 cost = GridGestionManager->GetGridData().Find(Neighbor)->TileCost;
		FPathData Data = FPathData(Neighbor, 1, 999, cost, TileIndex);
		Neighbors.Add(Data);

	}


	return Neighbors;
}

TArray<int32> ATTSActorPathFinding::GetNeihborsIndexes(int32 index, int32 GridWidth, int32 GridHeight, bool bCanDoDiagonal)
{
	int32 TileX = index % GridWidth;
	int32 TileY = index / GridWidth;

	TArray<int32> Neighbors;
	
	for (int32 OffsetY = -1; OffsetY <= 1; ++OffsetY)
	{
		for (int32 OffsetX = -1; OffsetX <= 1; ++OffsetX)
		{
			// Ignorer la case centrale (celle qu'on vérifie)
			if (OffsetX == 0 && OffsetY == 0)
				continue;

			// Exclure les cases diagonales si demandé
			if (!bCanDoDiagonal && FMath::Abs(OffsetX) == FMath::Abs(OffsetY))
				continue;

			// Calcul des coordonnées du voisin
			int32 NeighborX = TileX + OffsetX;
			int32 NeighborY = TileY + OffsetY;
			int32 NeighborIndex = NeighborY * GridWidth + NeighborX;
			Neighbors.Add(NeighborIndex);
		}
	}

	return Neighbors;
}

void ATTSActorPathFinding::InsertTileInDiscoveredArray(FPathData TileData)
{
	int32 SortingCost = TileData.TileCost + TileData.MinCostToTarget;
	if (DiscoverTilesSortingCost.Num() == 0)
	{
		DiscoverTilesSortingCost.Add(SortingCost);
		DiscoverTilesIndexes.Add(TileData.Index);
	}
	else
	{
		if (SortingCost >= DiscoverTilesSortingCost.Last())
		{
			DiscoverTilesSortingCost.Add(SortingCost);
			DiscoverTilesIndexes.Add(TileData.Index);
		}
		else
		{
			int32 index =0;
			for (int32 Tile : DiscoverTilesSortingCost)
			{
				if (Tile >= SortingCost)
				{
					DiscoverTilesSortingCost.Insert(SortingCost, index);
					DiscoverTilesIndexes.Insert(TileData.Index, index);
					break;
				}
				index++;
			}
		}
	}
	
}

void ATTSActorPathFinding::ClearGeneratedData()
{
	PathfindingData.Empty();
	DiscoverTilesSortingCost.Empty();
	DiscoverTilesIndexes.Empty();
	AnalysedTilesIndexes.Empty();
}

