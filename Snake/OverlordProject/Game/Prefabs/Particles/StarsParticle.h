#pragma once
#include "GameObject.h"
#include "ParticleEmitterComponent.h"

class StarsParticle : public GameObject
{
public:
	StarsParticle(DirectX::XMFLOAT3 position);
	virtual ~StarsParticle() = default;


	StarsParticle(const StarsParticle& other) = delete;
	StarsParticle(StarsParticle&& other) noexcept = delete;
	StarsParticle& operator=(const StarsParticle& other) = delete;
	StarsParticle& operator=(StarsParticle&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override { UNREFERENCED_PARAMETER(gameContext); };
	void Update(const GameContext& gameContext) override;

private:
	DirectX::XMFLOAT3 m_Position;
	ParticleEmitterComponent* m_pParticleEmitter;
	float m_Angle = 0;
};
