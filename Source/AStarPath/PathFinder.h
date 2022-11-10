// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "StarNode.h"
#include "DebugMacros.h"

#include "PathFinder.generated.h"

UCLASS()
class ASTARPATH_API APathFinder : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APathFinder();


	TArray<AStarNode*> Nodes_Registered;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
