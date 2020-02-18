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
	float ActiveOpacity=1.f; //��Ծʱʱ͸����
	UPROPERTY(EditAnywhere)
	float InactiveOpacity=0.1f;//����Ծʱ͸����
	UPROPERTY(EditAnywhere)
	float TimeUntilDeactive=0.5f; //Deactiveʱ��
	UPROPERTY(EditAnywhere)
	float TimeUntilReset=2.f; //resetʱ��
	UPROPERTY(EditAnywhere)
	bool bPreventReCenter = true;// ��ֹ�������ĵ�
	UPROPERTY(EditAnywhere)
	JoyStickType E_JoyStickType = JoyStickType::E_Walk;//ҡ������
	

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
