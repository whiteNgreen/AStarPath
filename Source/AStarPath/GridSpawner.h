// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "StarNode.h"
#include "DebugMacros.h"

#include "GridSpawner.generated.h"

UCLASS()
class ASTARPATH_API AGridSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridSpawner();

	class UBoxComponent* Box{ nullptr };

	UPROPERTY(EditAnywhere)
		float GridSpacing{ 100.f };
	UPROPERTY(EditAnywhere)
		int GridResolution{ 1 };
	UPROPERTY(EditAnywhere)
		int GridSize{ 1000 };

	void SpawnGrid(int Rez);
	UPROPERTY(EditAnywhere)
		TSubclassOf<AStarNode> SpawnClass;

	//TArray<TUniquePtr<AStarNode>> Nodes;
	TArray<FLine*> paths;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
