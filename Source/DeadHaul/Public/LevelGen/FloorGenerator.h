// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGen/RoomDataAsset.h"
#include "FloorGenerator.generated.h"

class ADoorConnector;

UCLASS(BlueprintType, Blueprintable)
class DEADHAUL_API AFloorGenerator : public AActor
{
	GENERATED_BODY()

public:	
	AFloorGenerator();

	virtual void BeginDestroy() override;

	// ============================================
	// Room Configuration Properties
	// ============================================

	/** The class used to spawn the starting room of the floor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	TSubclassOf<ARoomBase> StartRoomClass;

	/** The class used to spawn the ending room of the floor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	TSubclassOf<ARoomBase> EndRoomClass;

	/** Pool of room data assets available for generation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	TArray<URoomDataAsset*> RoomPool;

	/** Target number of rooms to generate for the floor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	int32 TargetRoomCount = 10;

	/** Seed for random generation (0 uses random seed) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	int32 Seed = 0;

	/** Maximum number of placement attempts for each room */
	UPROPERTY(EditAnywhere, Category = "Generation")
	int32 MaxPlacementAttempts = 10;

	/** Minimum number of rooms in the main path from start to end */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
	int32 MinMainPathLength = 5;

	// ============================================
	// Generation Result Properties
	// ============================================

	/** Array of all rooms placed during generation */
	UPROPERTY(BlueprintReadOnly, Category = "Generation")
	TArray<ARoomBase*> PlacedRooms;

	/** Reference to the generated starting room */
	UPROPERTY(BlueprintReadOnly, Category = "Generation")
	ARoomBase* StartRoom = nullptr;

	/** Reference to the generated ending room */
	UPROPERTY(BlueprintReadOnly, Category = "Generation")
	ARoomBase* EndRoom = nullptr;

	/** The seed value actually used for the last generation */
	UPROPERTY(BlueprintReadOnly, Category = "Generation")
	int32 LastUsedSeed = 0;

	// ============================================
	// Floor Generation Functions
	// ============================================

	/** Initiates procedural generation of the floor */
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void GenerateFloor();

	/** Clears all generated rooms and resets the floor */
	UFUNCTION(BlueprintCallable, Category = "Generation")
	void ClearFloor();

	/** Event called when floor generation completes */
	UFUNCTION(BlueprintImplementableEvent, Category = "Generation")
	void OnFloorGenerated();

	/** Attempts to place a dead-end room at the specified open door */
	void TryPlaceDeadEnd(ADoorConnector* OpenDoor);

protected:
	virtual void BeginPlay() override;

private:
	// ============================================
	// Private Properties
	// ============================================

	/** Random stream for deterministic procedural generation */
	FRandomStream RandStream;

	/** Tracks spawn count per room asset to manage variety */
	UPROPERTY()
	TMap<URoomDataAsset*, int32> SpawnCounts;

	// ============================================
	// Private Generation Methods
	// ============================================

	/** Spawns a room instance and places it at the specified transform */
	ARoomBase* SpawnRoom(TSubclassOf<ARoomBase> RoomClass,
		const FVector& Location,
		const FRotator& Rotation);

	/** Attempts to attach a room to an open door connector */
	bool TryAttachRoom(ADoorConnector* OpenDoor, bool bDeadEndOnly = false);

	/** Validates if a placement location is valid (no overlaps, etc) */
	bool IsPlacementValid(const FVector& Center,
		const FVector& Extent, const FRotator& Rotation);

	/** Computes the transform for attaching a room to a door */
	FTransform ComputeRoomTransform(ADoorConnector* OpenDoor,
		ADoorConnector* IncomingDoor);

	/** Places the end room and connects it to the main path */
	void PlaceEndRoom();

	/** Seals all remaining open doors to prevent items falling through */
	void SealAllOpenDoors();

	/** Validates that all rooms are properly connected */
	bool ValidateConnectivity();

	/** Core generation algorithm implementation */
	void GenerateFloorInternal();

};
