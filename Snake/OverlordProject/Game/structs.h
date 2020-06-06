#pragma once

enum CharacterMovement : UINT
{
	LEFT = 0,
	RIGHT,
	FORWARD,
	BACKWARD,
	NONE,
};

enum class PickupType : UINT
{
	ANT,
	APPLE
};