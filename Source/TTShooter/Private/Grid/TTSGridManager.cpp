// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/TTSGridManager.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Tiles/TTSTileModifier.h"
#include "TTShooter/TTShooter.h"


//ATTSGridManager* ATTSGridManager::Instance = nullptr;
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

	
	PlayerController = GetWorld()->GetFirstPlayerController();

	if (DefaultTileMesh)
		GridHolder->GetStaticMesh()->SetMaterial(0,DefaultTileMesh->GetMaterial(0));
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

void ATTSGridManager::UpdateTileVisual(int32 TileIndex)
{
	if (FTileData* TileToUpdate = GridData.Find(TileIndex))
	{
		float filled = 0;
		FColor Color = GetTileVisualDependingState(TileToUpdate->TileState, &filled);

		GridHolder->SetCustomDataValue(TileIndex,0,Color.R/10);
		GridHolder->SetCustomDataValue(TileIndex,1,Color.G/10);
		GridHolder->SetCustomDataValue(TileIndex,2,Color.B/10);
		GridHolder->SetCustomDataValue(TileIndex,3,filled/5);
	}
}

FColor ATTSGridManager::GetTileVisualDependingState(TArray<ETileState> TileStates, float* IsFill)
{
	if (TileStates.Num() < 0 || TileStates.IsEmpty())
		return FColor::Black;

	FColor Color;

	for (ETileState Status : TileStates)
	{
		switch (Status)
		{
		case ETileState::NONE:
			Color = FColor::Black;
			*IsFill = 0;
			break;
		case ETileState::HOVERED:
			Color = FColor::Blue;
			*IsFill = 0.8;
			break;
		case ETileState::SELECTED:
			Color = FColor::Green;
			*IsFill = 1;
			break;
		default:
			Color = FColor::Black;
			*IsFill = 0;
			break;
		}
	}
	return Color;
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
		FVector tileCenter = TileObjLocation + FVector(TileXSize / 2, TileYSize / 2,TileZSize );
		AddTileToMaps(TileIndex, tileCenter,LocalTransform, TileCost);
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
		FVector NewLocation = GridData.Find(Index)->TileLocation + offset;
		GridIndexes.Add(NewLocation);
	}
	return GridIndexes;
}

//	int32 Index = int32 Index = GridZ * (GridSize.X * GridSize.Y) + GridY * SizeX + GridX;


int32 ATTSGridManager::ConvertGridCoordsToGridIndex(const FVector& Coord) const
{
	
	// Calculer les coordonnées de la grille en fonction de la taille de la case
	int32 X = FMath::FloorToInt(Coord.X / TileXSize); 
	int32 Y = FMath::FloorToInt(Coord.Y / TileYSize); 
	int32 Z = FMath::FloorToInt(Coord.Z / TileZSize); 


	int32 GridIndex = Y * GridSize.X + X;
	int32 test = GridSize.X;

	if (X < 0 || X >= GridSize.X || Y < 0 || Y >= GridSize.Y)
	{
		return -1; 
	}

	return GridIndex;
}

FVector ATTSGridManager::GetTileLocationFromIndex(int32 Index, int32 Row, int32 Column) const
{
	float PosX = (Index % Row) * TileXSize;
	float PosY = (((Index / Row)  - ((Index/ (Row * Column)) * Row)) * TileYSize);
	return FVector(PosX, PosY, 0);
}

FVector2D ATTSGridManager::GetTilePosOnGridFromLocation(FVector TileLocation) const
{
	FVector LocationOnGrid = ConvertFromWorldToGrid(TileLocation);

	int32 XIndex = FMath::FloorToInt(LocationOnGrid.X / TileXSize);
	int32 YIndex = FMath::FloorToInt(LocationOnGrid.Y / TileYSize);

	return  FIntPoint(XIndex, YIndex);
}

int32 ATTSGridManager::GetTileIndexFromLocation(FVector TileLocation)
{
	FVector LocationOnGrid = ConvertFromWorldToGrid(TileLocation);

	return  ConvertGridCoordsToGridIndex(LocationOnGrid);
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

FVector ATTSGridManager::GetOffsetWorldLocationAtIndex(int32 GridIndex, float Offset)
{
	FVector Location = GridData.Find(GridIndex)->TileLocation;

	return GetActorTransform().TransformPosition(FVector(Location.X, Location.Y, Location.Z + Offset));
}

FVector ATTSGridManager::GetTileLocationFromMap(int32 Index) const
{
	if (!GridData.Contains(Index))
		return ErrorVector;
	
	return GridData.Find(Index)->TileLocation;
}

int32 ATTSGridManager::GetTileCostFromMap(int32 Index) const
{
	if (!GridData.Contains(Index))
		return ErrorInt;
	
	return GridData.Find(Index)->TileCost;
}

void ATTSGridManager::AddTileToMaps(int32 GridIndex, FVector TileLocation, FTransform TileTransform, int32 TileCost)
{
	FTileData TileData;
	TileData.TileIndex = GridIndex;
	TileData.TileLocation = TileLocation;
	TileData.TileTransform = TileTransform;
	TileData.TileCost = TileCost;
	
	GridData.Add(GridIndex, TileData);
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
	FTileData* TileAData = GridData.Find(TileAIndex);
	FTileData* TileBData = GridData.Find(TileBIndex);
	
	if (TileAData == nullptr || TileBData == nullptr)
		return -1;
	
	FVector TileAPos = TileAData->TileLocation;
	FVector TileBPos = TileBData->TileLocation;

	return  FMath::Abs(TileBPos.X - TileAPos.X) +
			FMath::Abs(TileBPos.Y - TileAPos.Y) +
			FMath::Abs(TileBPos.Z - TileAPos.Z);
}

float ATTSGridManager::GetDistanceBtwTwoTiles_Euclidienne(int32 TileAIndex, int32 TileBIndex)
{
	FTileData* TileAData = GridData.Find(TileAIndex);
	FTileData* TileBData = GridData.Find(TileBIndex);
	
	if (TileAData == nullptr || TileBData == nullptr)
		return -1;

	FVector TileAPos = TileAData->TileLocation;
	FVector TileBPos = TileBData->TileLocation;
	
	float Distance = FVector::Dist(TileAPos, TileBPos);

	return FMath::RoundToInt(Distance);
}

void ATTSGridManager::UpdateTileState(int32 TileIndex, ETileState StateToAdd, bool RemoveState)
{
	if (RemoveState)
		RemoveTileState(TileIndex, StateToAdd);
	else
		AddTileState(TileIndex, StateToAdd);

	UpdateTileVisual(TileIndex);
}

FTileData ATTSGridManager::GetTileDataFromIndex(int32 Index) const
{
	if (!GridData.Contains(Index))
		return FTileData();
	return *GridData.Find(Index);
}

int32 ATTSGridManager::GetTileAmountOfStateFromIndex(int32 Index)
{
	if (FTileData* CurrentTileData = GridData.Find(Index))
	{
		return CurrentTileData->TileState.Num();
	}
	return -1;
}

void ATTSGridManager::AddTileState(const int32 TileIndex, const ETileState StateToAdd)
{
	if (FTileData* CurrentTileData = GridData.Find(TileIndex))
	{
		CurrentTileData->TileState.AddUnique(StateToAdd);
	}
}

void ATTSGridManager::RemoveTileState(const int32 TileIndex, const ETileState StateToAdd)
{
	if (FTileData* CurrentTileData = GridData.Find(TileIndex))
	{
		CurrentTileData->TileState.Remove(StateToAdd);
	}
}

TArray<int32> ATTSGridManager::GetSelectedTilesNeighbors(const int32 TileIndex,const int32 GridWidth, const int32 GridHeight, bool bCanDoDiagonal)
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

FVector ATTSGridManager::GetCursorLocationOnGrid() const
{
	FVector WorldLocation, WorldDirection;
	
	if (!PlayerController)
	{
		return ErrorVector;
	}
	
	FHitResult HitResult;
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_GRID);

	bool bHit = PlayerController->GetHitResultUnderCursorByChannel(TraceType, false, HitResult);

	if (bHit)
	{
		return HitResult.Location;
	}

	if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		// Calculer une ligne très éloignée pour l'intersection
		FVector LineEnd = WorldLocation + (WorldDirection * 999999.0f);
		FVector Intersection;
		float Value;

		FPlane MyPlane = FPlane(GetActorLocation(), FVector(0,0,1));

		bHit = UKismetMathLibrary::LinePlaneIntersection(
			WorldLocation,    // Début de la ligne
			LineEnd,          // Fin de la ligne
			MyPlane,       // Résultat de l'intersection
			Value,
			Intersection
		);

		if (bHit)
			return Intersection;
		
		return ErrorVector;
	}
	return ErrorVector;

}

FVector2D ATTSGridManager::GetTileGridPosUnderCursor()
{
	const FVector CursorLocation = GetCursorLocationOnGrid();
	return GetTilePosOnGridFromLocation(CursorLocation);
}

int32 ATTSGridManager::GetTileIndexUnderCursor()
{
	const FVector CursorLocation = GetCursorLocationOnGrid();
	int32 TileIndex = GetTileIndexFromLocation(CursorLocation);

	return TileIndex;
}



FVector ATTSGridManager::GetTileLocationUnderCursor()
{
	int32 TileIndex = GetTileIndexUnderCursor();

	return GetTileLocationFromMap(TileIndex);
}

FVector ATTSGridManager::GetTileLocationUnderCursor(int32 TileIndex)
{
	return GetTileLocationFromMap(TileIndex);
}




