// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkWidget.h"
#include "Engine.h"
TSharedRef<SWidget> UWalkWidget::RebuildWidget()
{
	MyWalkWidget = SNew(SWalkWidget);
	MyWalkWidget->OnLeftAnalogDelegateX.BindUObject(this, &UWalkWidget::OnLeftAnalogDelegateX);
	MyWalkWidget->OnLeftAnalogDelegateY.BindUObject(this, &UWalkWidget::OnLeftAnalogDelegateY);
	MyWalkWidget->OnRightAnalogDelegateX.BindUObject(this, &UWalkWidget::OnRightAnalogDelegateX);
	MyWalkWidget->OnRightAnalogDelegateY.BindUObject(this, &UWalkWidget::OnRightAnalogDelegateY);
	return MyWalkWidget.ToSharedRef();
}

void UWalkWidget::OnLeftAnalogDelegateX(float x)
{
	FSlateApplication::Get().OnControllerAnalog(FGamepadKeyNames::LeftAnalogX, 0, x);
}
void UWalkWidget::OnLeftAnalogDelegateY(float y)
{
	FSlateApplication::Get().OnControllerAnalog(FGamepadKeyNames::LeftAnalogY, 0, y);
}
void UWalkWidget::OnRightAnalogDelegateX(float x)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("My Location is: %d"), x));
	//GEngine->AddOnScreenDebugMessage(0, 0.5f, FColor::Red, FString::Printf(TEXT(" OnRightAnalogDelegateX:  %s  "), x));
	//FSlateApplication::Get().OnControllerAnalog(FGamepadKeyNames::RightAnalogX, 0, x);
	FSlateApplication::Get().OnControllerAnalog(FGamepadKeyNames::LeftAnalogX, 0, x);
}
void UWalkWidget::OnRightAnalogDelegateY(float y)
{
	FSlateApplication::Get().OnControllerAnalog(FGamepadKeyNames::RightAnalogY, 0, y);
}
void UWalkWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	MyWalkWidget->SetJoyStickData(&JoyStickThumb, &JoyStickBG,VisualSize,ThumbSize,InputScale
		,ActiveOpacity,InactiveOpacity,TimeUntilDeactive,TimeUntilReset,bPreventReCenter, E_JoyStickType);
	
}