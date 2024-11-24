// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/TTSGridManager.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"

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
	if( AmountOfTile <= 0)
		return;

	TArray<FTransform> ListOfTransforms;
	
	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(CastRadius, CastHalfHeight);
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;     // Simplifies collision checks for performance
	QueryParams.AddIgnoredActor(this);     
	
	for (int32 TileIndex = 0; TileIndex < AmountOfTile ; TileIndex++)
	{
		FVector StartTargetLocation = GetTileLocationFromIndex(TileIndex,GridSize.X,GridSize.Y) + FVector(0, 0, 500.0f);
		FVector TargetLocation = GetTileLocationFromIndex(TileIndex,GridSize.X,GridSize.Y) - FVector(0, 0, -50.0f);

		FHitResult hit;

		bool bHit = GetWorld()->SweepSingleByChannel(hit,StartTargetLocation,TargetLocation,FQuat::Identity,
			ECC_Visibility,CapsuleShape,QueryParams);

		if (bHit)
		{
			FTransform LocalTransform;
			//Todo Prendre en compte l'axe Z
			LocalTransform.SetLocation(FVector(hit.Location.X,hit.Location.Y,0));
			ListOfTransforms.Add(LocalTransform);
			AddTileToMaps(TileIndex,LocalTransform.GetLocation(),1);
		}
		
		if(!bShowCast)
			return;

		DrawDebugCapsule(GetWorld(),StartTargetLocation,CastHalfHeight,CastRadius,FQuat::Identity,
			bHit ? FColor::Red : FColor::Green,false,10.0f );
	}

	if(!ListOfTransforms.IsEmpty())
		GridHolder->AddInstances(ListOfTransforms,false);
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

int32 ATTSGridManager::ConvertGridCoordsToGridIndex(FVector Coord)
{
	return (Coord.X * 1000 + Coord.Y) + Coord.Z * 10000;
}

FVector ATTSGridManager::GetTileLocationFromIndex(int32 Index, int32 Row, int32 Column) const
{
	float PosX = (Index % Row) * TileXSize;
	float PosY = (((Index / Row)  - ((Index/ (Row * Column)) * Row)) * TileYSize);
	UE_LOG(LogTemp, Warning, TEXT("Hello : %d = X = %f, Y = %f"), Index, PosX, PosY);
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



