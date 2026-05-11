// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelGen/FloorGenTypes.h"
#include "LevelGen/RoomBase.h"
#include "RoomDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DEADHAUL_API URoomDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
    // The room Blueprint to spawn
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
    TSubclassOf<ARoomBase> RoomClass;

	// Used by the generator to apply special placement rules (e.g. only place corridors between start and end)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
    ERoomType RoomType = ERoomType::Normal;

    // Higher = picked more often
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room", meta = (ClampMin = 0.f))
    float SpawnWeight = 1.f;

    // Won't appear until the generator has gone this many rooms deep
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
    int32 MinDepth = 0;

    // Max times this room can appear on one floor (-1 = unlimited)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
    int32 MaxInstances = -1;
	
};
