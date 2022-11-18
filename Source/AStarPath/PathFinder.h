// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "StarNode.h"
#include "DebugMacros.h"
#include "Interfaces/MaterialChangeInterface.h"

#include "PathFinder.generated.h"

UCLASS()
class ASTARPATH_API APathFinder : public APawn,
	public IMaterialChangeInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APathFinder();


	TArray<AStarNode*> NodePath;
	TArray<AStarNode*> Nodes_Registered;//old
	void ClearArray(TArray<AStarNode*>& arr){
		for (auto& it : arr) {
			if (it == StartNode || it == TargetNode) { continue; }
			IMaterialChangeInterface* mat = Cast<IMaterialChangeInterface>(it);
			if (mat) {
				if (mat->IsType(EMatType::CL_Block)) { continue; }
				mat->MatChange_Pure(EMatType::CL_None);
			}
		}
		arr.Empty();
	}
	TArray<AStarNode*> BlockedNodes;

	AStarNode* StartNode{ nullptr };
	AStarNode* TargetNode{ nullptr };

	void LeftClick();
	void LeftUnClick();

	void RightClick();
	void RightUnClick();

	void MiddleClick();
	void MiddleUnClick();

	void LeftArrowPress();
	void LeftArrowUnPress();

	void RightArrowPress();
	void RightArrowUnPress();

	bool Trace(const EMatType& type);

	UFUNCTION(BlueprintImplementableEvent)
		void RightClick_Event(const FHitResult& Hit, EMatType click);
	UFUNCTION(BlueprintImplementableEvent)
		void LeftClick_Event(const FHitResult& Hit, EMatType click);

	AActor* ClickedActor{ nullptr };
	AActor* TargetActor{ nullptr };
	AActor* StartActor{ nullptr };

	AStarNode* current{ nullptr };
	//FPath CollectedPath;

	/*--------------- B-SPLINE ----------------*/
	int mSplineDegree = 2;
	TArray<FVector> mSplineControlPoints{
		FVector(0,0,0),
		FVector(0,200,0),
		FVector(200,200,0),
		FVector(200,0,0)
	};
	TArray<float> mKnots;
	void MakeKnotVector();
	void MakeBSpline();
	float splinetime{};
	FVector GetPosAlongSpline(float t);
	float Bid(float t, int it, int d);
	float Wid(float t, int it, int d);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MatChange_Pure(EMatType click) override {}
};
