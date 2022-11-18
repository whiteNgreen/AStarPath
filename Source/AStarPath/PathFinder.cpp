// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFinder.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APathFinder::APathFinder()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void APathFinder::MakeKnotVector()
{
	mKnots.Empty();

	if (mSplineControlPoints.Num() == 2) {
		return;
	}
	if (mSplineControlPoints.Num() == 3) {
		mKnots = { 0, 0, 0, 1, 1, 1 };
		return;
	}

	/* Knot vector */
	int knotpoints = mSplineControlPoints.Num() + mSplineDegree + 1;
	float part = (float)knotpoints - ((mSplineDegree + 1) * 2);
	float step = (1 / (part + 1));

	for (int i{ 1 }; i <= knotpoints; i++)
	{
		if (i <= (mSplineDegree + 1)) {
			mKnots.Add(0);
		}
		else if (i > knotpoints - (mSplineDegree + 1)) {
			mKnots.Add(1);
		}
		else {
			int j = i - (mSplineDegree + 1);
			float knot = step * j;
			mKnots.Add(knot);
		}
	}
}

void APathFinder::MakeBSpline()
{
	if (mSplineControlPoints.Num() == 0) return;

	/* Draw debug lines for the spline */
	if (mSplineControlPoints.Num() == 2) {
		DrawDebugLine(GetWorld(), mSplineControlPoints[0], mSplineControlPoints[1], FColor::Black, false, 0.f, -1, 1.f);
		return;
	}


	int maxres = 100;
	FVector lastpos{};
	FVector pos{};
	for (size_t i{}; i < maxres; i++)
	{
		float t = (float)i / maxres;
		/* går gjennom spline */
		lastpos = pos;
		pos *= 0;
		for (size_t j{}; j < mSplineControlPoints.Num(); j++) {
			pos += mSplineControlPoints[j] * Bid(t, j, mSplineDegree);
		}
	
		/* tvangs setter det siste punktet */
		if (t >= 0.99f) {
			pos = *mSplineControlPoints.end();
		}
		if (!i && StartNode) lastpos = StartNode->GetActorLocation();
		DrawDebugLine(GetWorld(), lastpos, pos, FColor::Black, false, 0.f, -1, 1.f);
	}

}

FVector APathFinder::GetPosAlongSpline(float t)
{
	if (mSplineControlPoints.Num() == 0) {
		if (StartNode) return StartNode->GetActorLocation();
		return FVector();
	}
	FVector pos{};
	for (size_t j{}; j < mSplineControlPoints.Num(); j++) {
		pos += mSplineControlPoints[j] * Bid(t, j, mSplineDegree);
	}
	return pos;
}

float APathFinder::Bid(float t, int it, int d)
{
	if (d == 0) {
		if (mKnots[it] <= t && t < mKnots[it + 1]) {
			return 1.f;
		}
		return 0.0f;
	}
	return (Wid(t, it, d) * Bid(t, it, d - 1)) + ((1 - Wid(t, it + 1, d)) * Bid(t, it + 1, d - 1));
}

float APathFinder::Wid(float t, int it, int d)
{
	if (mKnots[it] < mKnots[it + d]) {
		return (t - mKnots[it]) / (mKnots[it + d] - mKnots[it]);
	}
	return 0.0f;
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
	MakeKnotVector();
}

// Called every frame
void APathFinder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (auto& it : mKnots) {
		PRINTPAR("KnotVector : %f", it);
	}
	PRINTPAR("SplinePoints : %i", mSplineControlPoints.Num());
	MakeBSpline();

	if (splinetime < 1.f) splinetime += DeltaTime;
	else splinetime = 1.f;
	DrawDebugPoint(GetWorld(), GetPosAlongSpline(splinetime), 9.f, FColor::Green, false, 0, -2);
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
					//AStarNode* startnode = Cast<AStarNode>(StartActor);
					//if (startnode)
						//startnode->UnCheckConnected();
				}
			}
		}
		StartActor = Hit.GetActor();

		/* Check the Nodes Connected Nodes */
		//AStarNode* startnode = Cast<AStarNode>(StartActor);
		//if (startnode) {
			//startnode->CheckConnected();
		//}
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
	StartActor = nullptr;
	TargetActor = nullptr;
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
			mSplineControlPoints.Empty();
			mKnots.Empty();

			static TArray<AStarNode*> tmp;
			ClearArray(tmp);
			ClearArray(NodePath);

			NodePath = FindPath(StartNode, TargetNode, tmp);
			for (auto& it : NodePath) {
				//mSplineControlPoints.Add(it->GetActorLocation());	// Node positions for spline curve
				if (it == StartNode) PRINTLONG("CONTAINS START");
				if (it == TargetNode || it == StartNode) { continue; }
				IMaterialChangeInterface* mat = Cast<IMaterialChangeInterface>(it);
				if (mat) mat->MatChange_Pure(EMatType::CL_Path);
			}
			//mSplineControlPoints.Add(StartNode->GetActorLocation())
			for (auto it = NodePath.end(); it != NodePath.begin();) {
				mSplineControlPoints.Add((*--it)->GetActorLocation());
			}
			MakeKnotVector();
			splinetime = 0.f;
			//MakeBSpline();
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


