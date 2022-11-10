// Fill out your copyright notice in the Description page of Project Settings.


#include "StarNode.h"
#include "PathFinder.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AStarNode::AStarNode()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = mesh;

}

// Called when the game starts or when spawned
void AStarNode::BeginPlay()
{
	Super::BeginPlay();
	
	AActor* a = UGameplayStatics::GetActorOfClass(GetWorld(), APathFinder::StaticClass());
	APathFinder* P = Cast<APathFinder>(a);
	if (P)
		P->Nodes_Registered.Add(this);
}

// Called every frame
void AStarNode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (const auto& it : ConnectedPaths)
	{
		//it->ShowPath(GetWorld());
	}
}

