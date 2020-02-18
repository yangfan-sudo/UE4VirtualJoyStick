// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "VirtualJoyStickCharacter.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/TouchInterface.h"
#include "Widgets/SViewport.h"
#include "Engine.h"
void AMyPlayerController::TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
	UpdateTouchDate();
	Super::TickActor(DeltaTime, TickType, ThisTickFunction);
}
void AMyPlayerController::UpdateTouchDate()
{
	float LocalX = 0;
	float LocalY = 0;
	bool ispressed = true;

	if (CurTouchIndex < ETouchIndex::MAX_TOUCHES)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("CurTouchIndex:%d  "), CurTouchIndex));
		GetInputTouchState(ETouchIndex::Type(CurTouchIndex), LocalX, LocalY, ispressed);
		if (TouchPressed)
		{
			AVirtualJoyStickCharacter* SolarCharacter = Cast<AVirtualJoyStickCharacter>(GetCharacter());
			float yawx = (LocalX - CurX) / SwipeSpeed;
			SolarCharacter->AddControllerYawInput(yawx);


			float pitchy = (LocalY - CurY) / SwipeSpeed;
			SolarCharacter->AddControllerPitchInput(pitchy);
			CurX = LocalX;
			CurY = LocalY;
		}
		TouchPressed = ispressed;
		//计算滑动插值，要先设置当前位置为手指点击的位置
		if (ispressed)
		{
			CurX = LocalX;
			CurY = LocalY;
		}
		else
		{
			CurTouchIndex = ETouchIndex::MAX_TOUCHES;

		}
	}
}



bool AMyPlayerController::InputTouch(uint32 Handle, ETouchType::Type Type, const FVector2D& TouchLocation, float Force, FDateTime DeviceTimestamp, uint32 TouchpadIndex)
{

	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,FString::Printf(TEXT("type:%d  index:%d Force:%f  Handle:%d"),Type,TouchpadIndex,Force, Handle));
	switch (Type)
	{
	case ETouchType::Began:
		break;
	case ETouchType::Moved:
		CurTouchIndex = Handle;
		break;
	case ETouchType::Stationary:
		break;
	case ETouchType::ForceChanged:
		break;
	case ETouchType::FirstMove:
		break;
	case ETouchType::Ended:
		break;
	case ETouchType::NumTypes:
		break;
	default:
		break;
	}
	return APlayerController::InputTouch(Handle, Type, TouchLocation, Force, DeviceTimestamp, TouchpadIndex);
}
