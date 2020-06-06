#pragma once
#include "GameObject.h"

class Grass : public GameObject
{
public:
	Grass() {};
	virtual ~Grass() = default;

	Grass(const Grass& other) = delete;
	Grass(Grass&& other) noexcept = delete;
	Grass& operator=(const Grass& other) = delete;
	Grass& operator=(Grass&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override { UNREFERENCED_PARAMETER(gameContext); };
	void Update(const GameContext& gameContext) override { UNREFERENCED_PARAMETER(gameContext); };

private:
	bool m_IsDark = true;

	void CreateWall(DirectX::XMFLOAT3 dimension, DirectX::XMFLOAT3 position);
};