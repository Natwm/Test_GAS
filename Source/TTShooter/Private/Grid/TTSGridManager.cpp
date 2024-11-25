// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/TTSGridManager.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Tiles/TTSTileModifier.h"
#include "TTShooter/TTShooter.h"

// Sets default values
ATTSGridManager::ATTSGridManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GridHolder = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("GridHolder"));
}

// Called when the game starts or when spawned
void ATTSGridManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATTSGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATTSGridManager::SetUpDefaultMeshToSpawn() const
{
	if(DefaultTileMesh != nullptr)
		GridHolder->SetStaticMesh(DefaultTileMesh);
}

void ATTSGridManager::SpawnDebugTileGrid()
{
	check(GridHolder);
	GridHolder->ClearInstances();
	//FindAndStoreTileScale();
	//SetUpDefaultMeshToSpawn();
	float AmountOfTile = GridSize.X * GridSize.Y;
	if( AmountOfTile <= 0)
		return;

	TArray<FTransform> ListOfTransforms;
	
	for (int32 TileIndex = 0; TileIndex < AmountOfTile ; TileIndex++)
	{
		FTransform LocalTransform;
		LocalTransform.SetLocation(GetTileLocationFromIndex(TileIndex,GridSize.X,GridSize.Y));
		ListOfTransforms.Add(LocalTransform);
	}

	if(!ListOfTransforms.IsEmpty())
		GridHolder->AddInstances(ListOfTransforms,false);
	
}

void ATTSGridManager::SpawnGridFromWorld()
{
	check(GridHolder);
	float AmountOfTile = GridSize.X * GridSize.Y;
	if (AmountOfTile <= 0)
		return;

	TArray<FTransform> ListOfTransforms;
	FVector HalfPos = FVector(TileXSize/2, TileYSize/2, 0);

	
	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CastRadius, CastHalfHeight);
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(this);

	
	for (int32 TileIndex = 0; TileIndex < AmountOfTile; TileIndex++)
	{
		FVector TilePosition = GetTileLocationFromIndex(TileIndex, GridSize.X, GridSize.Y);
		ProcessTile(TileIndex, TilePosition, CapsuleShape, QueryParams, HalfPos, ListOfTransforms);
	}

	if (!ListOfTransforms.IsEmpty())
		GridHolder->AddInstances(ListOfTransforms, false);
}

void ATTSGridManager::ProcessTile(
	int32 TileIndex,
	const FVector& TilePosition,
	const FCollisionShape& CapsuleShape,
	const FCollisionQueryParams& QueryParams,
	FVector HalfPos,
	TArray<FTransform>& ListOfTransforms
	)
{
	FVector StartTargetLocation = ConvertFromGridToWorld(TilePosition + HalfPos + FVector(0.0f, 0.0f, 50.0f));
	FVector TargetLocation = ConvertFromGridToWorld((TilePosition + HalfPos) + FVector(0.0f, 0.0f, -50.0f));

	
	TArray<FHitResult> Hits;
	bool bHit = GetWorld()->SweepMultiByChannel(
		Hits,
		StartTargetLocation,
		TargetLocation,
		FQuat::Identity,
		ECC_GRID,
		CapsuleShape,
		QueryParams
	);

	if (bHit && Hits.Num() > 0)
	{
		HandleHits(TileIndex, Hits, ListOfTransforms, TilePosition);
	}

	if (bShowCast)
	{
		DrawDebugCapsule(
			GetWorld(),
			StartTargetLocation,
			CastHalfHeight,
			CastRadius,
			FQuat::Identity,
			bHit ? FColor::Red : FColor::Green,
			false,
			240.0f
		);
	}
}

void ATTSGridManager::HandleHits(
	int32 TileIndex,
	const TArray<FHitResult>& Hits,
	TArray<FTransform>& ListOfTransforms,
	const FVector& TilePosition)
{
	FVector TileObjLocation = FVector(Hits[0].Location.X, Hits[0].Location.Y, 0) - FVector(TileXSize / 2, TileYSize / 2, 0.0f);
	FTransform LocalTransform;
	LocalTransform.SetLocation(ConvertFromWorldToGrid(TileObjLocation));

	int32 TileCost = 1;

	if (bool bCanCreateTile = DetermineTileCostAndValidity(Hits, TileCost))
	{
		ListOfTransforms.Add(LocalTransform);
		AddTileToMaps(TileIndex, LocalTransform.GetLocation(), TileCost);
	}
}

bool ATTSGridManager::DetermineTileCostAndValidity(const TArray<FHitResult>& Hits, int32& OutTileCost)
{
	for (const FHitResult& HitElement : Hits)
	{
		if (ATTSTileModifier* TileModifier = Cast<ATTSTileModifier>(HitElement.GetActor()))
		{
			if (TileModifier->GetTileCost() <= 0)
				return false;
			OutTileCost = TileModifier->GetTileCost();
			return true;
		}
	}

	return true;
}

FVector ATTSGridManager::ConvertFromGridToWorld(FVector Entry) const
{
	return GetActorTransform().TransformPosition(Entry);
}

FVector ATTSGridManager::ConvertFromWorldToGrid(FVector Entry) const
{
	return GetActorTransform().InverseTransformPosition(Entry);
}

TArray<FVector> ATTSGridManager::ConvertToGridIndexesLocation(TArray<int32> Indexes, FVector offset)
{
	TArray<FVector> GridIndexes;
	for (int32 Index : Indexes)
	{
		FVector NewLocation = *GridLocations.Find(Index) + offset;
		GridIndexes.Add(NewLocation);
	}
	return GridIndexes;
}

int32 ATTSGridManager::ConvertGridCoordsToGridIndex(FVector Coord) const
{
	// Calcul de l'index X et Y
	int32 IndexX = FMath::FloorToInt(Coord.X / TileXSize);
	int32 IndexY = FMath::FloorToInt(Coord.Y / TileYSize);

	return (IndexY * GridSize.X) + IndexX;
}

FVector ATTSGridManager::GetTileLocationFromIndex(int32 Index, int32 Row, int32 Column) const
{
	float PosX = (Index % Row) * TileXSize;
	float PosY = (((Index / Row)  - ((Index/ (Row * Column)) * Row)) * TileYSize);
	return FVector(PosX, PosY, 0);
}

FVector ATTSGridManager::ConvertIndexToWorldPosition(int32 GridIndex, int32 IndexX,
	int32 IndexZ, float HeightBetweenLevels) const
{
	float X = ((GridIndex / IndexX) % IndexX) * TileXSize;
	float Y = (GridIndex % IndexX) * TileYSize;
	float Z = (GridIndex / IndexZ) * HeightBetweenLevels;
	
	return FVector(X, Y, Z);
}

FVector ATTSGridManager::ConvertIndexToLocation(int32 Index, float ZOffset)
{
	return GetOffsetWorldLocationAtIndex(Index, ZOffset);
}

void ATTSGridManager::FindAndStoreTileScale()
{
	TileXSize = TileYSize = DefaultTileMesh->GetBounds().BoxExtent.X * 2;
}

FVector ATTSGridManager::GetOffsetWorldLocationAtIndex(int32 GridIndex, float offset)
{
	FVector* Location = GridLocations.Find(GridIndex);

	return GetActorTransform().TransformPosition(FVector(Location->X, Location->Y, Location->Z + offset));
}

FVector ATTSGridManager::GetTileLocationFromMap(int32 Index) const
{
	return *GridLocations.Find(Index);
}

int32 ATTSGridManager::GetTileCostFromMap(int32 Index) const
{
	return *GridCost.Find(Index);
}

void ATTSGridManager::AddTileToMaps(int32 GridIndex, FVector TileLocation, int32 TileCost)
{
	AddTileToLocationMap(GridIndex,TileLocation);
	AddTileCostToCostMap(GridIndex,TileCost);
}

void ATTSGridManager::AddTileToLocationMap(int32 TileIndex, FVector TileLocation)
{
	GridLocations.Add(TileIndex, TileLocation);
}

void ATTSGridManager::AddTileCostToCostMap(int32 TileIndex, int32 TileCost)
{
	GridCost.Add(TileIndex, TileCost);
}

bool ATTSGridManager::CanCrossDistance(int32 TileAIndex, int32 TileBIndex, int32 MaxDistance, bool bCanDoDiagonal)
{
	if (!bCanDoDiagonal)
		return GetDistanceBtwTwoTiles_Manhattan(TileAIndex, TileBIndex) <= MaxDistance;
	else
		return GetDistanceBtwTwoTiles_Euclidienne(TileAIndex, TileBIndex) <= MaxDistance;
		
}

float ATTSGridManager::GetDistanceBtwTwoTiles_Manhattan(int32 TileAIndex, int32 TileBIndex)
{
	if (GridLocations.Find(TileAIndex) == nullptr || GridLocations.Find(TileBIndex) == nullptr)
		return -1;
	
	FVector TileAPos = *GridLocations.Find(TileAIndex);
	FVector TileBPos = *GridLocations.Find(TileBIndex);

	return  FMath::Abs(TileBPos.X - TileAPos.X) +
			FMath::Abs(TileBPos.Y - TileAPos.Y) +
			FMath::Abs(TileBPos.Z - TileAPos.Z);
}

float ATTSGridManager::GetDistanceBtwTwoTiles_Euclidienne(int32 TileAIndex, int32 TileBIndex)
{
	if (GridLocations.Find(TileAIndex) == nullptr || GridLocations.Find(TileBIndex) == nullptr)
		return -1;

	FVector TileAPos = *GridLocations.Find(TileAIndex);
	FVector TileBPos = *GridLocations.Find(TileBIndex);
	
	float Distance = FVector::Dist(TileAPos, TileBPos);

	return FMath::RoundToInt(Distance);
}

TArray<int32> ATTSGridManager::GetSelectedTilesNeighbors(int32 TileIndex,const int32 GridWidth, const int32 GridHeight, bool bCanDoDiagonal)
{
	TArray<int32> Neighbors;

	// Calcul de la position (x, y) à partir de l'index
	int32 TileX = TileIndex % GridWidth;
	int32 TileY = TileIndex / GridWidth;
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

			// Vérifier que les coordonnées restent dans les limites de la grille
			if (NeighborX >= 0 && NeighborX < GridWidth && NeighborY >= 0 && NeighborY < GridHeight)
			{
				// Calculer l'index du voisin
				int32 NeighborIndex = NeighborY * GridWidth + NeighborX;
				Neighbors.Add(NeighborIndex);
			}
		}
	}

	return Neighbors;
}





