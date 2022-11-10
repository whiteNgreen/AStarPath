// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "DebugMacros.h"
#include "DrawDebugHelpers.h"

#include "StarNode.generated.h"




UCLASS()
class ASTARPATH_API AStarNode : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStarNode();

public:
	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* mesh{ nullptr };

	/* A* variables */
	TArray<FPath*> ConnectedPaths;

	int value{};


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};


USTRUCT()
struct FPath
{
	GENERATED_USTRUCT_BODY()

	FPath() {}
	FPath(AActor* a, AActor* b)
	{
		PRINTLONG("Created Path");

		Node_a = Cast<AStarNode>(a);
		Node_b = Cast<AStarNode>(b);
		Node_a->ConnectedPaths.Add(this);
		Node_b->ConnectedPaths.Add(this);

		Value = SetValue();
		Color = FColor::White;
	}

	AStarNode* Node_a{ nullptr };
	AStarNode* Node_b{ nullptr };

	int Value{};

	FColor Color;

	int SetValue()
	{
		float L = (Node_a->GetActorLocation() - Node_b->GetActorLocation()).Length();
		L *= 10;
		return (int)L;
	}

	void ShowPath(UWorld* world)
	{
		DrawDebugLine(world, Node_a->GetActorLocation(), Node_b->GetActorLocation(), Color, false, 0, 0, 3.f);
	}
};