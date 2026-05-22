// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PropSpawnPoint.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEADHAUL_API UPropSpawnPoint : public USceneComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UPropSpawnPoint();

	UFUNCTION()
	void SpawnProp();

protected:
	// Blueprint class of the prop object to spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<TSubclassOf<AActor>> PossibleProp;

	// Chance (0-1) that a prop will spawn at this point
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (ClampMin = "0.0", ClampMax = "1.0"));
	float SpawnChance = 1.f;

	// If true, pick a random prop from the PossibleProp array. If false, spawn all props in the array.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bPickRandom = true;

	

};
