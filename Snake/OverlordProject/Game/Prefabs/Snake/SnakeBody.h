#pragma once
#include "GameObject.h"
#include "ControllerComponent.h"
#include <queue>

class SnakeBody : public GameObject
{
public:
	SnakeBody(bool hasHitBox, bool active);
	virtual ~SnakeBody() = default;

	SnakeBody(const SnakeBody& other) = delete;
	SnakeBody(SnakeBody&& other) noexcept = delete;
	SnakeBody& operator=(const SnakeBody& other) = delete;
	SnakeBody& operator=(SnakeBody&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext, DirectX::XMFLOAT3 lookatPos);

	void AddPosition(DirectX::XMFLOAT3 position);
	void Activate();
	bool CanGetTail();
	void SetCanGetTail(bool canGetTail);
	
	void SetActive(bool active)
	{
		m_Active = active;
	}

private:
	bool m_Start = false;
	bool m_Active;
	float m_AcumulatedSec = 0;
	float m_Scale = 0;
	std::queue<DirectX::XMFLOAT3> m_Positions;
	bool m_HasHitBox;
	bool m_CanGetTail = false;
};

