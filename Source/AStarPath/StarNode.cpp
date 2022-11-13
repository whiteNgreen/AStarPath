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

void AStarNode::CheckConnected()
{
	for (auto& it : ConnectedPaths)
	{
		AStarNode* Node = it->GetOtherNode(this);
		Node->MatChange_Pure(EMatType::CL_Checked);
		checkedNodes.Add(Node);
		PRINTPARLONG("Node : %s", *Node->GetName());
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

	if (bClicked)
		PRINTPAR("Node paths = %i", ConnectedPaths.Num());

	//for (const auto& it : ConnectedPaths)
	//{
	//	//it->ShowPath(GetWorld());
	//}
}

int AStarNode::GetDistanceValue(const AStarNode* Target)
{
	return (int)(Target->GetActorLocation() - GetActorLocation()).Length() * floatConverter;
}
void AStarNode::InitValues(const AStarNode* Start, const AStarNode* Target)
{
	//G = GetDistanceValue(Start);
	H = GetDistanceValue(Target);
	F = G + H;
}
bool FindPath(FPath& Path, AStarNode* Start, const AStarNode* Target)
{
	if (!Start || !Target) { return false; }

	//AStarNode* current{ Start };
	AStarNode* nextnode{ nullptr };
	FLine* line{ nullptr };
	EPathSearch Searchtype = EPathSearch::Start;

	nextnode = Start->FindNextNode(Searchtype, Path, Start, Target);

	if (Searchtype == EPathSearch::Found) { return true; }
	return false;
}
AStarNode* AStarNode::FindNextNode(EPathSearch& SearchMode, FPath& path, const AStarNode* Start, const AStarNode* target)
{
	if (this == target) {
		SearchMode = EPathSearch::Found;
		PRINTPARLONG("FOUND TARGET");
		return this;
	}

	int Final{ INT_MAX };
	int Value{};
	AStarNode* Node{ nullptr };
	FLine* Path{ nullptr };

	for (size_t i{}; i < ConnectedPaths.Num(); i++)
	{
		AStarNode* tmpNode = ConnectedPaths[i]->GetOtherNode(this);
		if (path.ContainsNode(tmpNode)) { tmpNode = nullptr; continue; }
		if (tmpNode == Start) { continue; }
		if (tmpNode->IsBlock()) { continue; }

		if (tmpNode == target){
			SearchMode = EPathSearch::Found;
			return tmpNode;
		}

		Value = ConnectedPaths[i]->Length + tmpNode->GetDistanceValue(target);
		if (tmpNode != Start) {
			tmpNode->MatChange_Pure(EMatType::CL_Checked);
			path.AddCheckedNode(tmpNode);
		}

		if (Value < Final) {
			Final = Value;
			Path = ConnectedPaths[i];
			SearchMode = EPathSearch::Search;
			Node = tmpNode;
		}
	}
	if (Node) {
		path.AddPath(Node, Path, Value);
		Node->MatChange_Pure(EMatType::CL_Path);

		Node->FindNextNode(SearchMode, path, Start, target);
	}
	return Node;
}

//auto ChangeMat = [&](AStarNode* node, EMatType type) {
//	IMaterialChangeInterface* mat = Cast<IMaterialChangeInterface>(node);
//	if (mat) {
//		mat->
//	}
//}

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
			auto currentTile = Target;
			while (currentTile != Start) {
				finalPath.Add(currentTile);
				currentTile = currentTile->Connection;
			}
			return finalPath;
		}

		Processed.Add(current);
		ToSearch.Remove(current);

		for (size_t t{}; t < current->ConnectedPaths.Num(); t++)
		{
			AStarNode* Neighbor = current->ConnectedPaths[t]->GetOtherNode(current);
			bool inSearch = ToSearch.Contains(Neighbor);

			if (Neighbor->IsBlock()) { continue; }
			if (Processed.Contains(Neighbor)) { continue; }

			int CostToNeighbor = current->G + current->ConnectedPaths[t]->Length;

			if (!inSearch || CostToNeighbor < Neighbor->G)
			{
				Neighbor->SetG(CostToNeighbor);
				Neighbor->Connection = current;

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