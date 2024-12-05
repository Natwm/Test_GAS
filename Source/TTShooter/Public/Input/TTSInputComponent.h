// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "TTSInputConfig.h"
#include "TTSInputComponent.generated.h"

struct FTTSInputAction;
class UTTSInputConfig;
/**
 * 
 */
UCLASS()
class TTSHOOTER_API UTTSInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

	public :
	template<class UserClass, typename PressFuncType, typename ReleaseFuncType, typename HeldFuncType>
	void BindAbilityActions(const UTTSInputConfig* InputConfig, UserClass* Object, PressFuncType PressedFunc, ReleaseFuncType ReleaseFunc, HeldFuncType HeldFunc);
	
};


template <class UserClass, typename PressFuncType, typename ReleaseFuncType, typename HeldFuncType>
void UTTSInputComponent::BindAbilityActions(const UTTSInputConfig* InputConfig, UserClass* Object,
	PressFuncType PressedFunc, ReleaseFuncType ReleaseFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);

	for (const FTTSInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if(PressedFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Started,Object,PressedFunc, Action.InputTag);
			}
			
			if(HeldFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Triggered,Object,HeldFunc, Action.InputTag);
			}

			if(ReleaseFunc)
			{
				BindAction(Action.InputAction,ETriggerEvent::Completed,Object,ReleaseFunc, Action.InputTag);
			}
		}
	}
}
