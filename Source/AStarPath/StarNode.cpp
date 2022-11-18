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

	//root = CreateDefaultSubobject<USceneComponent>(TEXT("ROOT"));
	//RootComponent = root;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	mesh->SetupAttachment(RootComponent);
	//RootComponent = mesh;

}

void AStarNode::CheckConnected()
{
	for (auto& it : mConnectedNodes)
	{
		it->MatChange_Pure(EMatType::CL_Checked);
		checkedNodes.Add(it);
		PRINTPARLONG("Node : %s", *it->GetName());
	}
}

void AStarNode::UnCheckConnected()
{
	for (auto& it : checkedNodes){
		it->MatChange_Pure(EMatType::CL_None);
	}
	checkedNodes.Empty();
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

}

int AStarNode::GetDistanceValue(const AStarNode* Target)
{
	return (int)(Target->GetActorLocation() - GetActorLocation()).Length() * floatConverter;
}
void AStarNode::InitValues(const AStarNode* Start, const AStarNode* Target)
{
	H = GetDistanceValue(Target);
	F = G + H;
}
TArray<AStarNode*> FindPath(AStarNode* Start, AStarNode* Target, TArray<AStarNode*>& arr)
{
	PRINTLONG("NEW FIND PATH");
	TArray<AStarNode*> finalPath;

	TArray<AStarNode*> ToSearch;
	ToSearch.Add(Start);
	TArray<AStarNode*> Processed;

	while (ToSearch.Num() > 0)
	{
		AStarNode* current = ToSearch[0];
		current->InitValues(Start, Target);
		arr.Add(current);	// Material
		if (current != Start && current != Target) current->MatChange_Pure(EMatType::CL_Checked);	// Material
		for (auto& it : ToSearch)
		{
			if (it == current) { continue; } // skip if it is the same node
			it->InitValues(Start, Target);
			if (it->F < current->F || it->F == current->F && it->H < current->H)
				current = it;
		}


		if (current == Target) {
			/* Går bakover fra Target til Start */
			AStarNode* currentTile = Target;
			while (currentTile != Start) {
				finalPath.Add(currentTile);
				currentTile = currentTile->GetConnection();
			}
			finalPath.Add(Start);
			return finalPath;
		}

		Processed.Add(current);
		ToSearch.Remove(current);


		for (size_t t{}; t < current->mConnectedNodes.Num(); t++)
		{
			AStarNode* Neighbor = current->mConnectedNodes[t];
			bool inSearch = ToSearch.Contains(Neighbor);

			if (Neighbor->IsBlock()) { continue; }
			if (Processed.Contains(Neighbor)) { continue; }

			int CostToNeighbor = current->G + current->GetDistanceValue(Neighbor);

			if (!inSearch || CostToNeighbor < Neighbor->G)
			{
				Neighbor->SetG(CostToNeighbor);
				Neighbor->SetConnection(*current);

				if (!inSearch) {
					Neighbor->SetH(Target);
					ToSearch.Add(Neighbor);
					arr.Add(Neighbor);	// Material
				}
			}
		}
		if (current != Start && current != Target) current->MatChange_Pure(EMatType::CL_Processed);	// Material
	}

	return finalPath;
}