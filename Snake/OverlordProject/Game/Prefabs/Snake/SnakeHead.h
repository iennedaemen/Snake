#pragma once
#include "GameObject.h"
#include "ControllerComponent.h"
#include "../../structs.h"
#include "../Environment/Grid.h"
#include <queue>

class SnakeHead : public GameObject
{

public:
	SnakeHead();
	virtual ~SnakeHead() = default;

	bool HitObject()
	{
		return m_HitObject;
	}

	void SetPause(bool pause)
	{
		m_Pause = pause;
	}

	bool GetPickedup()
	{
		return m_PickedUp;
	}

	void setPickedUp(bool pickedup)
	{
		m_PickedUp = pickedup;
	}

	void SetSnakeSize(int snakeSize)
	{
		m_SnakeSize = snakeSize;
	}

	int GetSnakeSize()
	{
		return m_SnakeSize;
	}
	
	SnakeHead(const SnakeHead& other) = delete;
	SnakeHead(SnakeHead&& other) noexcept = delete;
	SnakeHead& operator=(const SnakeHead& other) = delete;
	SnakeHead& operator=(SnakeHead&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;

private:
	ControllerComponent* m_pController = nullptr;
	DirectX::XMFLOAT3 m_Velocity = { 0,0,0 };
	GameObject* m_pColliderObj = nullptr;
	GameObject* m_pTurnObj = nullptr;
	CharacterMovement m_Direction = CharacterMovement::FORWARD;
	CharacterMovement m_PreviousDirection = CharacterMovement::FORWARD;
	bool m_IsRotating = false;
	float m_RotationY = 0.0f;
	float m_NeededRotationY = 0.0f;
	float m_InitRotY = 0.0f;
	bool m_HitObject = false;
	bool m_Turn = false;
	DirectX::XMFLOAT2 m_TurnOffset{ 0.0f, 1.0f };
	bool m_Pause = false;

	bool m_PickedUp = false;
	std::queue<Grid*> m_Grids;
	int m_SnakeSize = 1;

	FMOD::Sound* m_pEatSound = nullptr;
	FMOD::Sound* m_pBumpSound = nullptr;
	FMOD::Channel* m_pChannel = nullptr;
	
	// FUNCTIONS
	void CheckRotation();
	void Trigger();
};

