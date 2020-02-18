// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "Input/Reply.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SLeafWidget.h"
#include "Widgets/SCompoundWidget.h"

//class FPaintArgs;
//class FSlateWindowElementList;

DECLARE_DELEGATE_OneParam(AnalogDelegate, float);

UENUM(BlueprintType)
enum class JoyStickType :uint8 //设置uint8类型
{
	E_Walk,
	E_RotateAngleOfView,

};
/**
 * 
 */
class VIRTUALJOYSTICK_API SWalkWidget : public SLeafWidget
{

public:
	/** The settings and current state of each zone we render */
	struct FControlInfo
	{
		FControlInfo()
		{
			// default to all 0
			FMemory::Memzero(this, sizeof(*this));
			CapturedPointerIndex = -1;
			InputScale = FVector2D(1.f, 1.f);
		}

		// Set by the game

		/** The brush to use to draw the thumb for joysticks, or clicked for buttons */
		FSlateBrush* Image1;
		
		/** The brush to use to draw the background for joysticks, or unclicked for buttons */
		FSlateBrush* Image2;

		/** The actual center of the control */
		FVector2D Center;

		/** The size of a joystick that can be re-centered within InteractionSize area */
		FVector2D VisualSize;

		/** The size of the thumb that can be re-centered within InteractionSize area */
		FVector2D ThumbSize;


		/** The scale for control input */
		FVector2D InputScale;
		

		/** Positioned center in viewport */
		FVector2D PositionedCenter;

	private:
		friend SWalkWidget;

		/**
		* Reset the control to a centered/inactive state
		*/
		void Reset();

		// Current state

		/** The position of the thumb, in relation to the VisualCenter */
		FVector2D ThumbPosition;

		/** For recentered joysticks, this is the re-center location */
		FVector2D VisualCenter;

		/** The corrected actual center of the control */
		FVector2D CorrectedCenter;

		/** The corrected size of a joystick that can be re-centered within InteractionSize area */
		FVector2D CorrectedVisualSize;

		/** The corrected size of the thumb that can be re-centered within InteractionSize area */
		FVector2D CorrectedThumbSize;

		/** The corrected size of a the interactable area around Center */
		FVector2D CorrectedInteractionSize;

		/** The corrected scale for control input */
		FVector2D CorrectedInputScale;

		/** Which pointer index is interacting with this control right now, or -1 if not interacting */
		int32 CapturedPointerIndex;

		/** Time to activate joystick **/
		float ElapsedTime;

		/** Visual center to be updated */
		FVector2D NextCenter;

		/** Whether or not to send one last "release" event next tick */
		bool bSendOneMoreEvent;

		/** Whether or not we need position the control against the geometry */
		bool bHasBeenPositioned;

		/** Whether or not to update center position */
		bool bNeedUpdatedCenter;
	};

	SLATE_BEGIN_ARGS(SWalkWidget)
	{}
	SLATE_END_ARGS()


		void Construct(const FArguments& InArgs);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual FVector2D ComputeDesiredSize(float) const override;

	virtual FReply OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	virtual FReply OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event) override;
	virtual FReply OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event) override;

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	void SetJoyStickData(FSlateBrush* ImageBg, FSlateBrush* ImageThumb, FVector2D VisualSize
		, FVector2D ThumbSize, FVector2D InputScale, float ActiveOpacity, float InactiveOpacity,
		float TimeUntilDeactive, float TimeUntilReset,bool bPreventReCenter, JoyStickType joysticktype);
protected:
	/** Callback for handling display metrics changes. */
	virtual void HandleDisplayMetricsChanged(const FDisplayMetrics& NewDisplayMetric);
	void AlignBoxIntoScreen(FVector2D& Position, const FVector2D& Size, const FVector2D& ScreenSize);
	/**
	 * Process a touch event (on movement and possibly on initial touch)
	 *
	 * @return true if the touch was successful
	 */
	virtual bool HandleTouch(const FVector2D& LocalCoord, const FVector2D& ScreenSize);

	/**
	 * Return the target opacity to lerp to given the current state
	 */
	FORCEINLINE float GetBaseOpacity();
	
	FControlInfo Control;
	
	float ActiveOpacity; //活跃时时不透明度
	float InactiveOpacity;//不活跃时不透明度
	float TimeUntilDeactive; //Deactive时间
	float TimeUntilReset; //reset时间
	float ActivationDelay; //激活延迟
	float StartupDelay;// 开始 延迟

	enum EVirtualJoystickState
	{
		State_Active,
		State_CountingDownToInactive,
		State_CountingDownToReset,
		State_Inactive,
		State_WaitForStart,
		State_CountingDownToStart,
	};

	/** The current state of all controls */
	EVirtualJoystickState State;

	/** True if the joystick should be visible */
	uint32 bVisible : 1;


	/** If true, ignore re-centering */
	uint32 bPreventReCenter =true;

	JoyStickType E_JoyStickType = JoyStickType::E_Walk;

	/** Target opacity */
	float CurrentOpacity;

	/* Countdown until next state change */
	float Countdown;

	/** Last used scaling value for  */
	float PreviousScalingFactor = 0.0f;
public:
	AnalogDelegate OnLeftAnalogDelegateX;
	AnalogDelegate OnLeftAnalogDelegateY;
	AnalogDelegate OnRightAnalogDelegateX;
	AnalogDelegate OnRightAnalogDelegateY;

};
