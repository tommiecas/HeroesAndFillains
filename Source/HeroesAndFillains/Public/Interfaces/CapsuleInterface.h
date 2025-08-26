// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CapsuleInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCapsuleInterface : public UInterface
{
	GENERATED_BODY()
	
public:

};

/**
 * 
 */
class HEROESANDFILLAINS_API ICapsuleInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
		virtual double GetCharacterCapsuleHeight();
		virtual double GetCharacterCapsuleRadius();
		virtual void SetCharacterCapsuleHeight(double Height);
		virtual void SetCharacterCapsuleRadius(double Radius);
};
