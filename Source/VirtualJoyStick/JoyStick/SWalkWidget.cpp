// Fill out your copyright notice in the Description page of Project Settings.


#include "SWalkWidget.h"
#include "Rendering/DrawElements.h"
#include "Misc/ConfigCacheIni.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine.h"

//BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

//END_SLATE_FUNCTION_BUILD_OPTIMIZATION

const float OPACITY_LERP_RATE = 3.f;

static FORCEINLINE float GetScaleFactor(const FGeometry& Geometry)
{
	const float DesiredWidth = 1024.0f;

	float UndoDPIScaling = 1.0f / Geometry.Scale;
	return (Geometry.GetDrawSize().GetMax() / DesiredWidth) * UndoDPIScaling;
}
//获得当前状态的不透明度
FORCEINLINE float SWalkWidget::GetBaseOpacity()
{
	return (State == State_Active || State == State_CountingDownToInactive) ? ActiveOpacity : InactiveOpacity;
}
void SWalkWidget::FControlInfo::Reset()
{
	// snap the visual center back to normal (for controls that have a center on touch)
	VisualCenter = CorrectedCenter;
}
void SWalkWidget::Construct(const FArguments& InArgs)
{
	bVisible = 1;


	// just set some defaults
	ActiveOpacity = 1.0f; //活跃不透明度
	InactiveOpacity = 0.1f;// 不活跃不透明度
	TimeUntilDeactive = 0.5f;
	TimeUntilReset = 2.0f;
	ActivationDelay = 0.f;
	CurrentOpacity = InactiveOpacity;
	StartupDelay = 0.f;
	


	UTexture2D* Tex(0);
	/*Control.Image1 = FCoreStyle::GetDynamicImageBrush("Engine.Joystick.Image1", Tex, "VirtualJoystick_Thumb");
	Control.Image2 = FCoreStyle::GetDynamicImageBrush("Engine.Joystick.Image2", Tex, "VirtualJoystick_Background");*/
	Control.Center = FVector2D(100.f, 300.f);
	Control.VisualSize = FVector2D(600.f, 600.f);
	Control.ThumbSize = FVector2D(200.f, 200.f);

	Control.InputScale = FVector2D(1.f, 1.f);
	/*OnAnalogDelegateX = InArgs._OnAnalogDelegateX;
	OnAnalogDelegateY = InArgs._OnAnalogDelegateY;*/
	FSlateApplication::Get().GetPlatformApplication()->OnDisplayMetricsChanged().AddSP(this, &SWalkWidget::HandleDisplayMetricsChanged);

}
void SWalkWidget::HandleDisplayMetricsChanged(const FDisplayMetrics& NewDisplayMetric)
{
	// Mark all controls to be repositioned on next tick
	
	Control.bHasBeenPositioned = false;
	
}
static int32 ResolveRelativePosition(float Position, float RelativeTo, float ScaleFactor)
{
	// absolute from edge
	if (Position < -1.0f)
	{
		return RelativeTo + Position * ScaleFactor;
	}
	// relative from edge
	else if (Position < 0.0f)
	{
		return RelativeTo + Position * RelativeTo;
	}
	// relative from 0
	else if (Position <= 1.0f)
	{
		return Position * RelativeTo;
	}
	// absolute from 0
	else
	{
		return Position * ScaleFactor;
	}

}

static bool PositionIsInside(const FVector2D& Center, const FVector2D& Position, const FVector2D& BoxSize)
{
	return
		Position.X >= Center.X - BoxSize.X * 0.5f &&
		Position.X <= Center.X + BoxSize.X * 0.5f &&
		Position.Y >= Center.Y - BoxSize.Y * 0.5f &&
		Position.Y <= Center.Y + BoxSize.Y * 0.5f;
}

//在这里绘制两个Image,大圆圈的先绘制，小圆圈的后绘制
int32 SWalkWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyClippingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 RetLayerId = LayerId;

	if (bVisible)
	{
		FLinearColor ColorAndOpacitySRGB = InWidgetStyle.GetColorAndOpacityTint();
		ColorAndOpacitySRGB.A = CurrentOpacity;

		if (Control.Image2)
		{
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				RetLayerId++,
				AllottedGeometry.ToPaintGeometry(
					Control.VisualCenter - FVector2D(Control.CorrectedVisualSize.X * 0.5f, Control.CorrectedVisualSize.Y * 0.5f),
					Control.CorrectedVisualSize),
				Control.Image2,
				ESlateDrawEffect::None,
				ColorAndOpacitySRGB
			);
		}

		if (Control.Image1)
		{
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				RetLayerId++,
				AllottedGeometry.ToPaintGeometry(
					Control.VisualCenter + Control.ThumbPosition - FVector2D(Control.CorrectedThumbSize.X * 0.5f, Control.CorrectedThumbSize.Y * 0.5f),
					Control.CorrectedThumbSize),
				Control.Image1,
				ESlateDrawEffect::None,
				ColorAndOpacitySRGB
			);		
		}
	}

	return RetLayerId;
}

FVector2D SWalkWidget::ComputeDesiredSize(float) const
{
	return FVector2D(100, 100);
}

FReply SWalkWidget::OnTouchStarted(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	FVector2D LocalCoord = MyGeometry.AbsoluteToLocal(Event.GetScreenSpacePosition());

	if (PositionIsInside(Control.CorrectedCenter, LocalCoord, Control.CorrectedInteractionSize))
	{
		// Align Joystick inside of Panel
		AlignBoxIntoScreen(LocalCoord, Control.CorrectedVisualSize, MyGeometry.GetLocalSize());

		Control.CapturedPointerIndex = Event.GetPointerIndex();

		if (ActivationDelay == 0.f)
		{
			CurrentOpacity = ActiveOpacity;

			if (!bPreventReCenter)
			{
				Control.VisualCenter = LocalCoord;
			}

			if (HandleTouch(LocalCoord, MyGeometry.GetLocalSize())) // Never fail!
			{
				return FReply::Handled().CaptureMouse(SharedThis(this));
			}
		}
		else
		{
			Control.bNeedUpdatedCenter = true;
			Control.ElapsedTime = 0.f;
			Control.NextCenter = LocalCoord;

			return FReply::Unhandled();
		}
	}

	return FReply::Handled();
}

FReply SWalkWidget::OnTouchMoved(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	FVector2D LocalCoord = MyGeometry.AbsoluteToLocal(Event.GetScreenSpacePosition());
	// is this control the one captured to this pointer?
	if (Control.CapturedPointerIndex == Event.GetPointerIndex())
	{
		if (Control.bNeedUpdatedCenter)
		{
			return FReply::Unhandled();
		}
		else if (HandleTouch(LocalCoord, MyGeometry.GetLocalSize()))
		{
			return FReply::Handled();
		}
	}
	
	

	return FReply::Handled();
}

FReply SWalkWidget::OnTouchEnded(const FGeometry& MyGeometry, const FPointerEvent& Event)
{
	// is this control the one captured to this pointer?
	if (Control.CapturedPointerIndex == Event.GetPointerIndex())
	{
		// release and center the joystick
		Control.ThumbPosition = FVector2D(0, 0);
		Control.CapturedPointerIndex = -1;

		// send one more joystick update for the centering
		Control.bSendOneMoreEvent = true;

		// Pass event as unhandled if time is too short
		if (Control.bNeedUpdatedCenter)
		{
			Control.bNeedUpdatedCenter = false;
			return FReply::Unhandled();
		}

		return FReply::Handled().ReleaseMouseCapture();
	}
	return FReply::Handled();
}
bool SWalkWidget::HandleTouch(const FVector2D& LocalCoord, const FVector2D& ScreenSize)
{

	// figure out position around center
	FVector2D Offset = LocalCoord - Control.VisualCenter;
	// only do work if we aren't at the center
	if (Offset == FVector2D(0, 0))
	{
		Control.ThumbPosition = Offset;
	}
	else
	{
		// clamp to the ellipse of the stick (snaps to the visual size, so, the art should go all the way to the edge of the texture)
		float DistanceToTouchSqr = Offset.SizeSquared();
		float Angle = FMath::Atan2(Offset.Y, Offset.X);

		// length along line to ellipse: L = 1.0 / sqrt(((sin(T)/Rx)^2 + (cos(T)/Ry)^2))
		float CosAngle = FMath::Cos(Angle);
		float SinAngle = FMath::Sin(Angle);
		float XTerm = CosAngle / (Control.CorrectedVisualSize.X * 0.5f);
		float YTerm = SinAngle / (Control.CorrectedVisualSize.Y * 0.5f);
		float DistanceToEdge = FMath::InvSqrt(XTerm * XTerm + YTerm * YTerm);

		// only clamp 
		if (DistanceToTouchSqr > FMath::Square(DistanceToEdge))
		{
			Control.ThumbPosition = FVector2D(DistanceToEdge * CosAngle, DistanceToEdge * SinAngle);
		}
		else
		{
			Control.ThumbPosition = Offset;
		}
	}

	FVector2D AbsoluteThumbPos = Control.ThumbPosition + Control.VisualCenter;
	AlignBoxIntoScreen(AbsoluteThumbPos, Control.CorrectedThumbSize, ScreenSize);
	Control.ThumbPosition = AbsoluteThumbPos - Control.VisualCenter;

	return true;
}

void SWalkWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (State == State_WaitForStart || State == State_CountingDownToStart)
	{
		CurrentOpacity = 0.f;
	}
	else
	{
		// lerp to the desired opacity based on whether the user is interacting with the joystick
		CurrentOpacity = FMath::Lerp(CurrentOpacity, GetBaseOpacity(), OPACITY_LERP_RATE * InDeltaTime);
	}

	// figure out how much to scale the control sizes
	float ScaleFactor = GetScaleFactor(AllottedGeometry);

	if (Control.bNeedUpdatedCenter)
	{
		Control.ElapsedTime += InDeltaTime;
		if (Control.ElapsedTime > ActivationDelay)
		{
			Control.bNeedUpdatedCenter = false;
			CurrentOpacity = ActiveOpacity;

			if (!bPreventReCenter)
			{
				Control.VisualCenter = Control.NextCenter;
			}

			HandleTouch(Control.NextCenter, AllottedGeometry.GetLocalSize());
		}
	}

	// calculate absolute positions based on geometry
	// @todo: Need to manage geometry changing!
	if (!Control.bHasBeenPositioned || ScaleFactor != PreviousScalingFactor)
	{
		
		// update all the sizes
		Control.CorrectedCenter = FVector2D(AllottedGeometry.GetLocalSize().X/2, AllottedGeometry.GetLocalSize().Y/2);
		Control.VisualCenter = Control.CorrectedCenter;
		Control.CorrectedVisualSize = FVector2D(ResolveRelativePosition(Control.VisualSize.X, AllottedGeometry.GetLocalSize().X, ScaleFactor), ResolveRelativePosition(Control.VisualSize.Y, AllottedGeometry.GetLocalSize().Y, ScaleFactor));
		Control.CorrectedInteractionSize = FVector2D(AllottedGeometry.GetLocalSize().X, AllottedGeometry.GetLocalSize().Y);
		Control.CorrectedThumbSize = FVector2D(ResolveRelativePosition(Control.ThumbSize.X, AllottedGeometry.GetLocalSize().X, ScaleFactor), ResolveRelativePosition(Control.ThumbSize.Y, AllottedGeometry.GetLocalSize().Y, ScaleFactor));
		Control.CorrectedInputScale = Control.InputScale; // *ScaleFactor;
		Control.bHasBeenPositioned = true;

		
	}

	if (Control.CapturedPointerIndex >= 0 || Control.bSendOneMoreEvent)
	{
		Control.bSendOneMoreEvent = false;

		// Get the corrected thumb offset scale (now allows ellipse instead of assuming square)
		FVector2D ThumbScaledOffset = FVector2D(Control.ThumbPosition.X * 2.0f / Control.CorrectedVisualSize.X, Control.ThumbPosition.Y * 2.0f / Control.CorrectedVisualSize.Y);
		float ThumbSquareSum = ThumbScaledOffset.X * ThumbScaledOffset.X + ThumbScaledOffset.Y * ThumbScaledOffset.Y;
		float ThumbMagnitude = FMath::Sqrt(ThumbSquareSum);
		FVector2D ThumbNormalized = FVector2D(0.f, 0.f);
		if (ThumbSquareSum > SMALL_NUMBER)
		{
			const float Scale = 1.0f / ThumbMagnitude;
			ThumbNormalized = FVector2D(ThumbScaledOffset.X * Scale, ThumbScaledOffset.Y * Scale);
		}

		// Find the scale to apply to ThumbNormalized vector to project onto unit square
		float ToSquareScale = fabs(ThumbNormalized.Y) > fabs(ThumbNormalized.X) ? FMath::Sqrt((ThumbNormalized.X * ThumbNormalized.X) / (ThumbNormalized.Y * ThumbNormalized.Y) + 1.0f)
			: ThumbNormalized.X == 0.0f ? 1.0f : FMath::Sqrt((ThumbNormalized.Y * ThumbNormalized.Y) / (ThumbNormalized.X * ThumbNormalized.X) + 1.0f);

		// Apply proportional offset corrected for projection to unit square
		FVector2D NormalizedOffset = ThumbNormalized * Control.CorrectedInputScale * ThumbMagnitude * ToSquareScale;

		// now pass the fake joystick events to the game
		/*const FGamepadKeyNames::Type XAxis = (FGamepadKeyNames::LeftAnalogX);
		const FGamepadKeyNames::Type YAxis = (FGamepadKeyNames::LeftAnalogY);*/

		FSlateApplication::Get().SetAllUserFocusToGameViewport();
		/*FSlateApplication::Get().OnControllerAnalog(FGamepadKeyNames::LeftAnalogX, 0, NormalizedOffset.X);
		FSlateApplication::Get().OnControllerAnalog(FGamepadKeyNames::LeftAnalogY, 0, -NormalizedOffset.Y);*/

		switch (E_JoyStickType)
		{
		case JoyStickType::E_Walk:
			OnLeftAnalogDelegateX.ExecuteIfBound(NormalizedOffset.X);
			OnLeftAnalogDelegateY.ExecuteIfBound(-NormalizedOffset.Y);
			break;
		case JoyStickType::E_RotateAngleOfView:
			OnRightAnalogDelegateX.ExecuteIfBound(NormalizedOffset.X);
			OnRightAnalogDelegateY.ExecuteIfBound(-NormalizedOffset.Y);
			break;
		}
	}

	

	// we need to store the computed scale factor so we can compare it with the value computed in the following frame and, if necessary, recompute widget position
	PreviousScalingFactor = ScaleFactor;
	bool ControlActive = false;
	if (Control.CapturedPointerIndex != -1)
	{
		ControlActive=true;
	}
	if (ControlActive)
	{
		State = State_Active;
	}
	else
	{
		switch (State)
		{
		case State_WaitForStart:
		{
			State = State_CountingDownToStart;
			Countdown = StartupDelay;
		}
		break;
		case State_CountingDownToStart:
			// update the countdown
			Countdown -= InDeltaTime;
			if (Countdown <= 0.0f)
			{
				State = State_Inactive;
			}
			break;
		case State_Active:

			// start going to inactive
			State = State_CountingDownToInactive;
			Countdown = TimeUntilDeactive;
			break;

		case State_CountingDownToInactive:
			// update the countdown
			Countdown -= InDeltaTime;
			if (Countdown <= 0.0f)
			{
				// should we start counting down to a control reset?
				if (TimeUntilReset > 0.0f)
				{
					State = State_CountingDownToReset;
					Countdown = TimeUntilReset;
				}
				else
				{
					// if not, then just go inactive
					State = State_Inactive;
				}
			}
			break;

		case State_CountingDownToReset:
			Countdown -= InDeltaTime;
			if (Countdown <= 0.0f)
			{

				Control.Reset();
				// finally, go inactive
				State = State_Inactive;
			}
			break;
		}
	}
}
void SWalkWidget::AlignBoxIntoScreen(FVector2D& Position, const FVector2D& Size, const FVector2D& ScreenSize)
{
	if (Size.X > ScreenSize.X || Size.Y > ScreenSize.Y)
	{
		return;
	}

	// Align box to fit into screen
	if (Position.X - Size.X * 0.5f < 0.f)
	{
		Position.X = Size.X * 0.5f;
	}

	if (Position.X + Size.X * 0.5f > ScreenSize.X)
	{
		Position.X = ScreenSize.X - Size.X * 0.5f;
	}

	if (Position.Y - Size.Y * 0.5f < 0.f)
	{
		Position.Y = Size.Y * 0.5f;
	}

	if (Position.Y + Size.Y * 0.5f > ScreenSize.Y)
	{
		Position.Y = ScreenSize.Y - Size.Y * 0.5f;
	}
}

void SWalkWidget::SetJoyStickData(FSlateBrush* ImageThumb, FSlateBrush* ImageBg, FVector2D VisualSize
	, FVector2D ThumbSize, FVector2D InputScale, float _ActiveOpacity, float _InactiveOpacity,
	float _TimeUntilDeactive, float _TimeUntilReset, bool _bPreventReCenter, JoyStickType joysticktype)
{
	Control.Image1 = ImageThumb;
	Control.Image2 = ImageBg;
	Control.VisualSize = VisualSize;
	Control.ThumbSize = ThumbSize;
	Control.InputScale = InputScale;
	ActiveOpacity = _ActiveOpacity;
	InactiveOpacity = _InactiveOpacity;
	TimeUntilDeactive = _TimeUntilDeactive;
	TimeUntilReset = _TimeUntilReset;
	bPreventReCenter = _bPreventReCenter;
	E_JoyStickType = joysticktype;
}

