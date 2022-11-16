// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSpawner.h"
#include "Components/BoxComponent.h"

// Sets default values
AGridSpawner::AGridSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("BOX"));
	RootComponent = Box;
}

// Called when the game starts or when spawned
void AGridSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnGrid(GridResolution);
}

void AGridSpawner::SpawnGrid(int Rez)
{
	TArray<AStarNode*> list;
	AStarNode* a{ nullptr };
	AStarNode* b{ nullptr };

	for (size_t x{}; x < 10; x++)
	{
		for (size_t y{}; y < 10; y++)
		{
			FVector Location = FVector(GetActorLocation().X + (GridSpacing * x) - (float)GridSize / 2, GetActorLocation().Y + (GridSpacing * y)-(float)GridSize / 2, GetActorLocation().Z);
			FRotator Rotation;
			AStarNode* t{ nullptr };
			t = GetWorld()->SpawnActor<AStarNode>(SpawnClass, Location, Rotation);
			list.AddUnique(t);
		}
	}
	for (size_t i{1}; i < list.Num(); i++)
	{
		if (list.Num() < i + 1) { /*PRINTLONG("+1 CONTINUE");*/ break;  }

		a = list[i-1];
		b = list[i];
		if (i%10 != 0 || i==0){
			a->AddConnectedNode(b);
			b->AddConnectedNode(a);
		}

		if (list.Num() < i + 10) { /*PRINTLONG("*1 CONTINUE");*/ continue; }
		b = list[i-1 + 10];
		a->AddConnectedNode(b);
		b->AddConnectedNode(a);
	}
}

// Called every frame
void AGridSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

