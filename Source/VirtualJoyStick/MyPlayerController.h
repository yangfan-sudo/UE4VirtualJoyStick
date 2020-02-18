// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VIRTUALJOYSTICK_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
		bool TouchPressed = false;
	float CurX = 0;
	float CurY = 0;
	float SwipeSpeed = 8;
	uint32 CurTouchIndex = ETouchIndex::MAX_TOUCHES;
	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	void UpdateTouchDate();
	virtual bool InputTouch(uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, float Force, FDateTime DeviceTimestamp, uint32 TouchpadIndex) override;
	
};
