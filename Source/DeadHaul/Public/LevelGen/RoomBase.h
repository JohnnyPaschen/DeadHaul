// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGen/FloorGenTypes.h"
#include "RoomBase.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DEADHAUL_API ARoomBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoomBase();

    // All door connectors belonging to this room
    UPROPERTY(BlueprintReadWrite, Category = "Room")
    TArray<class ADoorConnector*> DoorConnectors;

    // This room's type — used by the generator for placement rules
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Room")
	ERoomType RoomType = ERoomType::Normal;

    // Bounding box half-extents used for overlap checks
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Room")
    FVector RoomExtent = FVector(500.f, 500.f, 200.f);

    // Floor depth assigned by the generator (0 = start room)
    UPROPERTY(BlueprintReadWrite, Category = "Room")
    int32 FloorDepth = 0;

    // Called by the generator after this room is successfully placed
    UFUNCTION(BlueprintNativeEvent, Category = "Room")
    void OnRoomPlaced();
    virtual void OnRoomPlaced_Implementation() {}

    // Called at end of generation — seal any doors that never got connected
    UFUNCTION(BlueprintNativeEvent, Category = "Room")
    void SealOpenDoors();
    virtual void SealOpenDoors_Implementation();

    // Returns only the unconnected door connectors
    UFUNCTION(BlueprintCallable, Category = "Room")
    TArray<ADoorConnector*> GetOpenDoors() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Finds all door connectors that are part of this room and adds them to the DoorConnectors array
	void CollectDoorConnectors();

};
