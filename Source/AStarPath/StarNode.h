// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "DebugMacros.h"
#include "DrawDebugHelpers.h"
#include "../AStarPath/Interfaces/MaterialChangeInterface.h"

#include "StarNode.generated.h"


inline int floatConverter{ 10 };

USTRUCT()
struct FPath
{
	GENERATED_USTRUCT_BODY()

	FPath(){}
	~FPath(){}
	FPath(class AStarNode* start) { mStartNode = start; }
	class AStarNode* mStartNode;
	TArray<class AStarNode*> mNodes;
	TArray<class AStarNode*> mCheckedNodes;
	TArray<struct FLine*> mLines;
	int Value{};

	void AddPath(class AStarNode* N, struct FLine* L, int v)
	{
		mNodes.Add(N);
		mLines.Add(L);
		Value += v;
	}
	void AddCheckedNode(class AStarNode* N)
	{
		mCheckedNodes.Add(N);
	}
	bool ContainsNode(class AStarNode* N)
	{
		if (N == mStartNode) { return true; }
		for (const auto& it : mNodes)
			if (it == N) { return true; }
		return false;
	}
	void ClearPath()
	{
		for (auto& it : mNodes) {
			IMaterialChangeInterface* mat = Cast<IMaterialChangeInterface>(it);
			if (mat)
				mat->MatChange_Pure(EMatType::CL_None);
		}
		mNodes.Empty();
		mLines.Empty();
	}
	void ClearChecked()
	{
		for (auto& k : mCheckedNodes) {
			IMaterialChangeInterface* check = Cast<IMaterialChangeInterface>(k);
			if (check)
				check->MatChange_Pure(EMatType::CL_None);
		}
		mCheckedNodes.Empty();
	}
};

UENUM()
enum EPathSearch
{
	Start,
	Search,
	Found
};

UCLASS()
class ASTARPATH_API AStarNode : public AActor,
	public IMaterialChangeInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStarNode();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* mesh{ nullptr };

	bool bClicked{};

	/* A* variables */
	TArray<FLine*> ConnectedPaths;
	int GetInternalValue(const AStarNode* Target);

	AStarNode* FindNextNode(EPathSearch& SearchMode, FPath& path, const AStarNode* Start, const AStarNode* target);

	int value{};
	virtual void MatChange_Pure(EMatType click) override {
		MatType = click;
		IMaterialChangeInterface::Execute_MatChange(this, click);
	}

	TArray<AStarNode*> checkedNodes;
	void CheckConnected();
	void UnCheckConnected();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};


USTRUCT()
struct FLine
{
	GENERATED_USTRUCT_BODY()

	FLine() {}
	FLine(AActor* a, AActor* b)
	{
		Node_a = Cast<AStarNode>(a);
		Node_b = Cast<AStarNode>(b);
		Node_a->ConnectedPaths.Add(this);
		Node_b->ConnectedPaths.Add(this);

		Length = SetLength();
		Color = FColor::White;
	}
	~FLine(){}

	AStarNode* Node_a{ nullptr };
	AStarNode* Node_b{ nullptr };

	int Length{};

	FColor Color;

	int SetLength()
	{
		float L = (Node_a->GetActorLocation() - Node_b->GetActorLocation()).Length();
		L *= floatConverter;
		return (int)L;
	}

	AStarNode* GetOtherNode(const AStarNode* current)
	{
		AStarNode* P;
		current == Node_a ? P = Node_b : P = Node_a;
		return P;
	}

	void ShowPath(UWorld* world)
	{
		DrawDebugLine(world, Node_a->GetActorLocation(), Node_b->GetActorLocation(), Color, false, 0, 0, 3.f);
	}
};

bool FindPath(FPath& path, AStarNode* Start, const AStarNode* Target);

