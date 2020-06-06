#pragma once
#include "GameObject.h"
#include "ControllerComponent.h"
#include "SnakeHead.h"
#include "SnakeBody.h"

class Snake : public GameObject
{
public:
	Snake();
	virtual ~Snake() = default;

	void Grow();
	void SetPause(bool pause);
	bool IsPaused();

	bool GetPickedup()
	{
		return m_PickedUp;
	}

	void setPickedUp(bool pickedup)
	{
		m_PickedUp = pickedup;
	}
	
	Snake(const Snake& other) = delete;
	Snake(Snake&& other) noexcept = delete;
	Snake& operator=(const Snake& other) = delete;
	Snake& operator=(Snake&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;

private:
	SnakeHead* m_pHead = nullptr;
	std::vector<SnakeBody*> m_pBodies;
	bool m_Pause = false;
	int m_SnakeSize = 0;

	bool m_LastFull = false;
	bool m_PickedUp = false;
	int k = 0;

	//std::vector<DirectX::XMFLOAT3> m_Positions;
};

