// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGen/DoorConnector.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ADoorConnector::ADoorConnector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(Root);
	ArrowComponent->ArrowColor = FColor::Cyan;
	ArrowComponent->ArrowSize = 2.5f;

	DebugBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DebugBox"));
	DebugBox->SetupAttachment(Root);
	DebugBox->SetBoxExtent(FVector(50.f, 100.f, 150.f));
	DebugBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

