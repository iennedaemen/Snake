#include "stdafx.h"
#include "ParticleEmitterComponent.h"
 #include <utility>
#include "EffectHelper.h"
#include "ContentManager.h"
#include "TextureDataLoader.h"
#include "Particle.h"
#include "TransformComponent.h"

ParticleEmitterComponent::ParticleEmitterComponent(std::wstring  assetFile, int particleCount):
	m_pVertexBuffer(nullptr),
	m_pEffect(nullptr),
	m_pParticleTexture(nullptr),
	m_pInputLayout(nullptr),
	m_pInputLayoutSize(0),
	m_Settings(ParticleEmitterSettings()),
	m_ParticleCount(particleCount),
	m_ActiveParticles(0),
	m_LastParticleInit(0.0f),
	m_AssetFile(std::move(assetFile))
{
	//TODO: See Lab9_2
	for(int i{}; i < m_ParticleCount; ++i)
	{
		m_Particles.push_back(new Particle(m_Settings));
	}
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	//TODO: See Lab9_2
	for (int i{}; i < m_ParticleCount; ++i)
	{
		SafeDelete(m_Particles[i]);
	}
	m_Particles.clear();
	m_pInputLayout->Release();
	m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const GameContext& gameContext)
{
	//TODO: See Lab9_2
	LoadEffect(gameContext);
	CreateVertexBuffer(gameContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::LoadEffect(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
	m_pEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ParticleRenderer.fx");
	m_pDefaultTechnique = m_pEffect->GetTechniqueByIndex(0);
	m_pWvpVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_pViewInverseVariable = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();
	m_pTextureVariable = m_pEffect->GetVariableByName("gParticleTexture")->AsShaderResource();

	EffectHelper::BuildInputLayout(gameContext.pDevice, m_pDefaultTechnique, &m_pInputLayout, m_pInputLayoutSize);
}

void ParticleEmitterComponent::CreateVertexBuffer(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();

	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(ParticleVertex) * m_ParticleCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = m_Particles.data();

	hr = gameContext.pDevice->CreateBuffer(&desc, &initData, &m_pVertexBuffer);
	if (FAILED(hr)) Logger::LogHResult(hr, L"ParticleEmitterComponent::CreateVertexBuffer");
}

void ParticleEmitterComponent::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
	float particleInterval = ((m_Settings.MaxEnergy + m_Settings.MinEnergy) * 0.5f) / m_ParticleCount;

	m_LastParticleInit += gameContext.pGameTime->GetElapsed();

	m_ActiveParticles = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	gameContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ParticleVertex* pBuffer = (ParticleVertex*) mappedResource.pData;

	for(Particle* particle : m_Particles)
	{
		particle->Update(gameContext);

		if(particle->IsActive())
		{
			pBuffer[m_ActiveParticles] = particle->GetVertexInfo();
			++m_ActiveParticles;
		}
		else if(m_LastParticleInit >= particleInterval)
		{
			particle->Init(GetTransform()->GetWorldPosition());
			pBuffer[m_ActiveParticles] = particle->GetVertexInfo();
			++m_ActiveParticles;
			m_LastParticleInit = 0;
		}
	}

	gameContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::Draw(const GameContext& )
{}

void ParticleEmitterComponent::PostDraw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: See Lab9_2
	m_pWvpVariable->SetMatrix((float*)&gameContext.pCamera->GetViewProjection());
	m_pViewInverseVariable->SetMatrix((float*)&gameContext.pCamera->GetViewInverse());
	m_pTextureVariable->SetResource(m_pParticleTexture->GetShaderResourceView());

	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT offset = 0;
	UINT stride = sizeof(ParticleVertex);
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC techDesc;
	m_pDefaultTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pDefaultTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}
