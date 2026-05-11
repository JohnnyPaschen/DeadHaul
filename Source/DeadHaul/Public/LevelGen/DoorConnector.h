// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGen/FloorGenTypes.h"
#include "DoorConnector.generated.h"

class ARoomBase;
class UArrowComponent;
class UBoxComponent;

UCLASS(BlueprintType, Blueprintable)
class DEADHAUL_API ADoorConnector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorConnector();

	// Direction door faces (outward from the room) - North, South, East, West
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	EDoorDirection Direction;

	// Set to true once another room is connected to this door
	UPROPERTY(BlueprintReadWrite, Category = "Door")
	bool bIsConnected = false;

	// Room that owns this connector
	UPROPERTY(BlueprintReadWrite, Category = "Door")
	ARoomBase* OwningRoom = nullptr;

	// Connector on other side that this door is connected to (if any)
	UPROPERTY(BlueprintReadWrite, Category = "Door")
	ADoorConnector* LinkedConnector = nullptr;

	// Arrow so you can see door direction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UArrowComponent* ArrowComponent;

	// Box for overlap testing during generation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* DebugBox;

};
