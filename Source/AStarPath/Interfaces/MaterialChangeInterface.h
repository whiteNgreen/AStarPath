// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MaterialChangeInterface.generated.h"

UENUM(BlueprintType)
enum class EMatType : uint8
{
	CL_None,
	CL_Start,
	CL_Target,
	CL_Path,
	CL_Checked,
	CL_Processed,
	CL_Block
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
	EMatType MatType = EMatType::CL_None;
	EMatType GetMatType() const { return MatType; }
	bool IsType(EMatType const& type) { 
		if (type == MatType) return true;
		return false;
	}

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
