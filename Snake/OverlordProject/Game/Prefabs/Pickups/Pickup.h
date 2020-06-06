#pragma once
#include "GameObject.h"
#include "../../structs.h"

class Pickup : public GameObject
{
public:
	Pickup(PickupType type)
		: m_Type(type)
	{}
	virtual ~Pickup() = default;

	PickupType GetType()
	{
		return m_Type;
	}

	Pickup(const Pickup& other) = delete;
	Pickup(Pickup&& other) noexcept = delete;
	Pickup& operator=(const Pickup& other) = delete;
	Pickup& operator=(Pickup&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override { UNREFERENCED_PARAMETER(gameContext); };
	void Update(const GameContext& gameContext) override;

private:
	PickupType m_Type;
};
