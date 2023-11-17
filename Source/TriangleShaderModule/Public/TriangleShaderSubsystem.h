// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "TriangleShaderSubsystem.generated.h"

class FTriangleViewExtension;
/**
 * 
 */
UCLASS()
class TRIANGLESHADERMODULE_API UTriangleShaderSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

	// Add ViewExtension Pointer in Protected
protected:
	TSharedPtr<FTriangleViewExtension,ESPMode::ThreadSafe> TrianglePass;
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};
