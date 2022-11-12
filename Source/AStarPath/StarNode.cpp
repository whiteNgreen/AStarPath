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

int AStarNode::GetInternalValue(const AStarNode* Target)
{
	return (int)(Target->GetActorLocation() - GetActorLocation()).Length() * floatConverter;
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

		if (tmpNode == target){
			SearchMode = EPathSearch::Found;
			return tmpNode;
		}

		Value = ConnectedPaths[i]->Length + tmpNode->GetInternalValue(target);
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

