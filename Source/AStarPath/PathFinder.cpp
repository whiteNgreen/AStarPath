// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFinder.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APathFinder::APathFinder()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APathFinder::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* P = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (P)
	{
		P->SetShowMouseCursor(true);
	}
}

// Called every frame
void APathFinder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//PRINTPAR("Array : %i", Nodes_Registered.Num());
}

// Called to bind functionality to input
void APathFinder::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("MouseLeft", IE_Pressed, this, &APathFinder::LeftClick);
	PlayerInputComponent->BindAction("MouseLeft", IE_Released, this, &APathFinder::LeftUnClick);

	PlayerInputComponent->BindAction("MouseRight", IE_Pressed, this, &APathFinder::RightClick);
	PlayerInputComponent->BindAction("MouseRight", IE_Released, this, &APathFinder::RightUnClick);

	PlayerInputComponent->BindAction("RightArrow", IE_Released, this, &APathFinder::RightArrowPress);
	PlayerInputComponent->BindAction("RightArrow", IE_Released, this, &APathFinder::RightArrowUnPress);
}



bool APathFinder::Trace(const EMatType& type)
{
	APlayerController* P = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!P) { return false; }

	FVector2D xy;
	P->GetMousePosition(xy.X, xy.Y);
	FVector WorldPos;
	FVector Direction;
	UGameplayStatics::DeprojectScreenToWorld(P, xy, WorldPos, Direction);

	FHitResult Hit;
	FCollisionQueryParams Params("", false, this);
	const bool b = GetWorld()->LineTraceSingleByChannel(Hit, WorldPos, WorldPos + Direction * 3000.f, ECC_Visibility, Params);
	if (!b) { return false; }


	IMaterialChangeInterface* mat = Cast<IMaterialChangeInterface>(Hit.GetActor());
	if (mat) {
		mat->MatChange_Pure(type);
	}

	if (type == EMatType::CL_Start) {
		if (Hit.GetActor() != StartActor) {
			mat = Cast<IMaterialChangeInterface>(StartActor);
			if (mat) {
				if (mat->MatType == type) {
					mat->MatChange_Pure(EMatType::CL_None);
					CollectedPath.ClearPath();
					CollectedPath.ClearChecked();

					/* UnChecks checked connected nodes */
					//AStarNode* StartNode = Cast<AStarNode>(StartActor);
					//if (StartNode)
						//StartNode->UnCheckConnected();
				}
			}
		}
		StartActor = Hit.GetActor();

		/* Check the Nodes Connected Nodes */
		//AStarNode* StartNode = Cast<AStarNode>(StartActor);
		//if (StartNode) {
		//	CollectedPath.mStartNode = StartNode;
		//	StartNode->CheckConnected();
		//}
		//else { CollectedPath.mStartNode = nullptr; }

		return true;
	}
	if (type == EMatType::CL_Target) {
		if (Hit.GetActor() != TargetActor) {
			mat = Cast<IMaterialChangeInterface>(TargetActor);
			if (mat) {
				if (mat->MatType == type) {
					mat->MatChange_Pure(EMatType::CL_None);
					CollectedPath.ClearPath();
					CollectedPath.ClearChecked();
				}
			}
		}
		TargetActor = Hit.GetActor();
		return true;
	}


	mat = Cast<IMaterialChangeInterface>(ClickedActor);
	if (mat) {
		if (mat->MatType == type) {
			mat->MatChange_Pure(EMatType::CL_None);
			CollectedPath.ClearPath();
			CollectedPath.ClearChecked();
		}
	}
	ClickedActor = Hit.GetActor();
	return true;
}

static bool bLeftClick{};
void APathFinder::LeftClick()
{
	if (!bLeftClick)
	{
		bLeftClick = true;
		Trace(EMatType::CL_Start);
	}
}
void APathFinder::LeftUnClick()
{
	bLeftClick = false;
}

static bool bRightClick{};
void APathFinder::RightClick()
{
	if (!bRightClick)
	{
		bRightClick = true;
		Trace(EMatType::CL_Target);
	}
}
void APathFinder::RightUnClick()
{
	bRightClick = false;
}

static bool bRightArrowPress{};
void APathFinder::RightArrowPress()
{
	if (!bRightArrowPress)
	{
		bRightArrowPress = true;


		AStarNode* StartNode = Cast<AStarNode>(StartActor);
		AStarNode* TargetNode = Cast<AStarNode>(TargetActor);
		if (StartNode && TargetNode)
		{
			CollectedPath.ClearPath();
			CollectedPath.ClearChecked();
			FindPath(CollectedPath, StartNode, TargetNode);
		}
	}
}
void APathFinder::RightArrowUnPress()
{
	bRightArrowPress = false;
}



