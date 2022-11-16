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

	PlayerInputComponent->BindAction("MouseMiddle", IE_Pressed, this, &APathFinder::MiddleClick);
	PlayerInputComponent->BindAction("MouseMiddle", IE_Released, this, &APathFinder::MiddleUnClick);

	PlayerInputComponent->BindAction("RightArrow", IE_Pressed, this, &APathFinder::RightArrowPress);
	PlayerInputComponent->BindAction("RightArrow", IE_Released, this, &APathFinder::RightArrowUnPress);

	PlayerInputComponent->BindAction("LeftArrow", IE_Pressed, this, &APathFinder::LeftArrowPress);
	PlayerInputComponent->BindAction("LeftArrow", IE_Released, this, &APathFinder::LeftArrowUnPress);
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


	if (type == EMatType::CL_Start) {
		if (Hit.GetActor() != StartActor) {
			mat = Cast<IMaterialChangeInterface>(StartActor);
			if (mat) {
				if (mat->MatType == type) {
					mat->MatChange_Pure(EMatType::CL_None);
					/* UnChecks checked connected nodes */
					AStarNode* startnode = Cast<AStarNode>(StartActor);
					if (startnode)
						startnode->UnCheckConnected();
				}
			}
		}
		StartActor = Hit.GetActor();

		/* Check the Nodes Connected Nodes */
		AStarNode* startnode = Cast<AStarNode>(StartActor);
		if (startnode) {
			startnode->CheckConnected();
		}
		mat = Cast<IMaterialChangeInterface>(Hit.GetActor());
		if (mat) mat->MatChange_Pure(type);
		return true;
	}

	if (type == EMatType::CL_Target) {
		if (Hit.GetActor() != TargetActor) {
			mat = Cast<IMaterialChangeInterface>(TargetActor);
			if (mat) {
				if (mat->MatType == type) {
					mat->MatChange_Pure(EMatType::CL_None);
				}
			}
		}
		TargetActor = Hit.GetActor();

		mat = Cast<IMaterialChangeInterface>(Hit.GetActor());
		if (mat) mat->MatChange_Pure(type);
		return true;
	}

	if (type == EMatType::CL_Block)
	{
		if (mat)
		{
			AStarNode* N = Cast<AStarNode>(Hit.GetActor());
			EMatType MT = EMatType::CL_None;
			if (mat->GetMatType() == EMatType::CL_Block) { // Un-Blocking nodes
				MT = EMatType::CL_None;
				N->SetNodeType(ENodeType::NT_None);
				BlockedNodes.Remove(N);
			}
			else {	// Blocking Node
				MT = EMatType::CL_Block;
				N->SetNodeType(ENodeType::NT_Block);
				BlockedNodes.Add(N);
			}
			mat->MatChange_Pure(MT);
		}
		return true;
	}

	mat = Cast<IMaterialChangeInterface>(ClickedActor);
	if (mat) {
		if (mat->MatType == type) {
			mat->MatChange_Pure(EMatType::CL_None);
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
		Trace(EMatType::CL_Block);
	}
}
void APathFinder::RightUnClick()
{
	bRightClick = false;
}

static bool bMiddleClick{};
void APathFinder::MiddleClick()
{
	if (!bMiddleClick)
	{
		bMiddleClick = true;
		Trace(EMatType::CL_Target);
	}
}
void APathFinder::MiddleUnClick()
{
	bMiddleClick = false;
}


static bool bRightArrowPress{};
void APathFinder::RightArrowPress()
{
	if (!bRightArrowPress)
	{
		bRightArrowPress = true;


		StartNode = Cast<AStarNode>(StartActor);
		TargetNode = Cast<AStarNode>(TargetActor);
		if (StartNode && TargetNode)
		{
			static TArray<AStarNode*> tmp;
			//ClearArray(tmp);
			ClearArray(NodePath);

			NodePath = FindPath(StartNode, TargetNode, tmp);
			for (auto& it : NodePath) {
				if (it == TargetNode || it == StartNode) { continue; }
				IMaterialChangeInterface* mat = Cast<IMaterialChangeInterface>(it);
				if (mat) mat->MatChange_Pure(EMatType::CL_Path);
			}
		}
	}
}
void APathFinder::RightArrowUnPress()
{
	bRightArrowPress = false;
}


static bool bLeftArrowPress{};
void APathFinder::LeftArrowPress()
{
	if (!bLeftArrowPress)
	{
		bLeftArrowPress = true;

		// Clear blocked nodes
		for (auto& it : BlockedNodes)
		{
			it->MatChange_Pure(EMatType::CL_None);
			it->SetNodeType(ENodeType::NT_None);
		}
		BlockedNodes.Empty();
	}
}
void APathFinder::LeftArrowUnPress()
{
	bLeftArrowPress = false;
}


