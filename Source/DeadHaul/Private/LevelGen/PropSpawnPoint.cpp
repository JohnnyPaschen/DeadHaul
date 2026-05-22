// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGen/PropSpawnPoint.h"

// Sets default values
UPropSpawnPoint::UPropSpawnPoint()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void UPropSpawnPoint::SpawnProp()
{
    if (PossibleProp.IsEmpty()) return;

    // Roll against spawn chance
    if (FMath::FRand() > SpawnChance) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    if (bPickRandom)
    {
        TSubclassOf<AActor> Chosen = PossibleProp[FMath::RandRange(0, PossibleProp.Num() - 1)];
        if (Chosen)
        {
            GetWorld()->SpawnActor<AActor>(Chosen, GetComponentTransform(), SpawnParams);
        }
    }
    else
    {
        for (TSubclassOf<AActor>& PropClass : PossibleProp)
        {
            if (PropClass)
                GetWorld()->SpawnActor<AActor>(PropClass, GetComponentTransform(), SpawnParams);
        }
    }
	
}

