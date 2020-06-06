#include "stdafx.h"
#include "Particle.h"

// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
using namespace DirectX;

Particle::Particle(const ParticleEmitterSettings& emitterSettings):
	m_VertexInfo(ParticleVertex()),
	m_EmitterSettings(emitterSettings),
	m_IsActive(false),
	m_TotalEnergy(0),
	m_CurrentEnergy(0),
	m_SizeGrow(0),
	m_InitSize(0)
{}

void Particle::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
	if (!m_IsActive) 
		return;

	// Energy
	float elapsedSec = gameContext.pGameTime->GetElapsed();
	m_CurrentEnergy -= elapsedSec;
	if(m_CurrentEnergy < 0)
	{
		m_IsActive = false;
		return;
	}

	// Position
	m_VertexInfo.Position.x += m_EmitterSettings.Velocity.x *elapsedSec;
	m_VertexInfo.Position.y += m_EmitterSettings.Velocity.y * elapsedSec;
	m_VertexInfo.Position.z += m_EmitterSettings.Velocity.z * elapsedSec;

	// Color
	m_VertexInfo.Color = m_EmitterSettings.Color;
	float particleLifePercent = m_CurrentEnergy / m_TotalEnergy;
	m_VertexInfo.Color.w = particleLifePercent * 2;

	// Size
	if(m_SizeGrow < 1)
	{
		m_VertexInfo.Size = m_InitSize * m_SizeGrow * particleLifePercent;
	}
	else if(m_SizeGrow > 1)
	{
		m_VertexInfo.Size = m_InitSize * m_SizeGrow * (1 - particleLifePercent);
	}
	
}

void Particle::Init(XMFLOAT3 initPosition)
{
	UNREFERENCED_PARAMETER(initPosition);
	//TODO: See Lab9_2
	m_IsActive = true;

	// Energy
	m_TotalEnergy = randF(m_EmitterSettings.MinEnergy, m_EmitterSettings.MaxEnergy);
	m_CurrentEnergy = m_TotalEnergy;

	// Position
	XMVECTOR randomDirectionVec = { 1,0,0 };
	XMMATRIX randomRotationMatrix = XMMatrixRotationRollPitchYaw(randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI), randF(-XM_PI, XM_PI));
	randomDirectionVec = XMVector3TransformNormal(randomDirectionVec, randomRotationMatrix);
	DirectX::XMFLOAT3 randomDirection;
	XMStoreFloat3(&randomDirection, randomDirectionVec);
	
	float randomDistance = randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);
	
	m_VertexInfo.Position.x = randomDirection.x * randomDistance + initPosition.x;
	m_VertexInfo.Position.y = randomDirection.y * randomDistance + initPosition.y;
	m_VertexInfo.Position.z = randomDirection.z * randomDistance + initPosition.z;

	// Size
	m_VertexInfo.Size = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
	m_InitSize = m_VertexInfo.Size;
	m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow, m_EmitterSettings.MaxSizeGrow);

	// Rotation
	m_VertexInfo.Rotation = randF(-XM_PI, XM_PI);
	
}
