// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VirtualJoyStick/JoyStick/SWalkWidget.h"
#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "WalkWidget.generated.h"



/**
 * 
 */
UCLASS()
class VIRTUALJOYSTICK_API UWalkWidget : public UWidget
{
	GENERATED_BODY()

public:
	/** Image to draw */
	
	UPROPERTY(EditAnywhere)
	FSlateBrush JoyStickThumb;
	UPROPERTY(EditAnywhere)
	FSlateBrush JoyStickBG;

	UPROPERTY(EditAnywhere)
	FVector2D VisualSize;
	UPROPERTY(EditAnywhere)
	FVector2D ThumbSize;
	UPROPERTY(EditAnywhere)
	FVector2D InputScale;

	UPROPERTY(EditAnywhere)
	float ActiveOpacity=1.f; //活跃时时透明度
	UPROPERTY(EditAnywhere)
	float InactiveOpacity=0.1f;//不活跃时透明度
	UPROPERTY(EditAnywhere)
	float TimeUntilDeactive=0.5f; //Deactive时间
	UPROPERTY(EditAnywhere)
	float TimeUntilReset=2.f; //reset时间
	UPROPERTY(EditAnywhere)
	bool bPreventReCenter = true;// 阻止重置中心点
	UPROPERTY(EditAnywhere)
	JoyStickType E_JoyStickType = JoyStickType::E_Walk;//摇杆类型
	

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;


protected:
	/** Native Slate Widget */
	TSharedPtr<SWalkWidget> MyWalkWidget;

	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface
public:
	void OnLeftAnalogDelegateX(float x);
	void OnLeftAnalogDelegateY(float y);

	void OnRightAnalogDelegateX(float x);
	void OnRightAnalogDelegateY(float y);
};
