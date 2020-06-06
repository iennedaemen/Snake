#include "stdafx.h"
#include "StarsParticle.h"
#include "Components.h"

StarsParticle::StarsParticle(DirectX::XMFLOAT3 position)
	: m_Position(position)
{
	
}

void StarsParticle::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	GetTransform()->Translate(m_Position);
	
	m_pParticleEmitter = new ParticleEmitterComponent(L"./Resources/Textures/Star.png", 3);
	m_pParticleEmitter->SetMinSize(8.0f);
	m_pParticleEmitter->SetMaxSize(8.0f);
	m_pParticleEmitter->SetMinEnergy(1.0f);
	m_pParticleEmitter->SetMaxEnergy(1.0f);
	m_pParticleEmitter->SetMinSizeGrow(2.0f);
	m_pParticleEmitter->SetMaxSizeGrow(2.0f);
	m_pParticleEmitter->SetMinEmitterRange(0.0f);
	m_pParticleEmitter->SetMaxEmitterRange(0.0f);
	m_pParticleEmitter->SetColor(DirectX::XMFLOAT4(1.f, 1.f, 0.1f, 1.0f));
	AddComponent(m_pParticleEmitter);
}

void StarsParticle::Update(const GameContext& gameContext)
{
	float elapsedSec = gameContext.pGameTime->GetElapsed();
	
	m_Angle += 50 * elapsedSec;
	float radius = 8;

	float x = m_Position.x + cos(m_Angle) * radius;
	float z = m_Position.z + sin(m_Angle) * radius;
	
	GetTransform()->Translate( x, m_Position.y, z);
}
