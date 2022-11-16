// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "DebugMacros.h"
#include "DrawDebugHelpers.h"
#include "../AStarPath/Interfaces/MaterialChangeInterface.h"

#include "StarNode.generated.h"



inline int floatConverter{ 10 };

UENUM(BlueprintType)
enum ENodeType
{
	NT_None,
	NT_Start,
	NT_Path,
	NT_Target,
	NT_Checked,
	NT_Processed,
	NT_Block
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


// FINAL VARIABLES ------------------------------------------------
	/* A* variables */
	int GetDistanceValue(const AStarNode* Target);
	void InitValues(const AStarNode* Start, const AStarNode* Target);
	int F;	// Total DistanceValue
	int G;	// DistanceValue from StartNode to this
	int H;	// DistanceValue from TargetNode to this

	void SetG(int& g) { G = g; F = G + H; }
	void SetH(AStarNode* Target) { H = GetDistanceValue(Target); }
	void AddConnectedNode(AStarNode* Node) { mConnectedNodes.Add(Node); }

	AStarNode* GetConnection() const { return Connection; }
	void SetConnection(AStarNode& node) { Connection = &node; }

	bool IsBlock() const { return NodeType == ENodeType::NT_Block; }
	void SetNodeType(const ENodeType& type) { NodeType = type; }

	TArray<AStarNode*> mConnectedNodes;
private:
	AStarNode* Connection{ nullptr };
	ENodeType NodeType = ENodeType::NT_None;

public:	// To check the nodes connected to the selected node, using material change
	TArray<AStarNode*> checkedNodes;
	void CheckConnected();
	void UnCheckConnected();
//------------------------------------------------------------------

public:
	virtual void MatChange_Pure(EMatType click) override {
		MatType = click;
		IMaterialChangeInterface::Execute_MatChange(this, click);
	}


private:
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
TArray<AStarNode*> FindPath(AStarNode* Start, AStarNode* Target, TArray<AStarNode*>& arr);

