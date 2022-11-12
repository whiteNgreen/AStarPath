// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MaterialChangeInterface.generated.h"

UENUM(BlueprintType)
enum EMatType
{
	CL_None,
	CL_Start,
	CL_Target,
	CL_Path,
	CL_Checked
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMaterialChangeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
	*
	*/
class ASTARPATH_API IMaterialChangeInterface
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	//virtual void Reset() = 0;
	EMatType MatType;

	UFUNCTION(BlueprintNativeEvent, Category = "Functions")
		void MatChange(EMatType click);
	virtual void MatChange_Pure(EMatType click)=0;
	//UFUNCTION(BlueprintNativeEvent, Category = "Functions")
	//	void LookedAt();		  
	//UFUNCTION(BlueprintNativeEvent, Category = "Functions")
	//	void Start();
	//UFUNCTION(BlueprintNativeEvent, Category = "Functions")
	//	void Target();

};
