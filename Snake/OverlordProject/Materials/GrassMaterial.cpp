#include "stdafx.h"
#include "GrassMaterial.h"
#include "ContentManager.h"
#include "TextureData.h"
#include "Components.h"

ID3DX11EffectShaderResourceVariable* GrassMaterial::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectScalarVariable* GrassMaterial::m_TimeVariable = nullptr;
ID3DX11EffectScalarVariable* GrassMaterial::m_WindVelocityVariable = nullptr;
ID3DX11EffectShaderResourceVariable* GrassMaterial::m_pShadowSRVvariable = nullptr;
ID3DX11EffectVectorVariable* GrassMaterial::m_pLightDirectionVariable = nullptr;
ID3DX11EffectMatrixVariable* GrassMaterial::m_pLightWVPvariable = nullptr;



GrassMaterial::GrassMaterial() : Material(L"./Resources/Effects/GrassShader.fx"),
	m_pDiffuseTexture(nullptr)
{}

GrassMaterial::~GrassMaterial()
{}

void GrassMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void GrassMaterial::SetTime(const float time)
{
	m_Time += time;
}

void GrassMaterial::SetWindVelocity(const float velocity)
{
	m_WindVelocity = velocity;
}

void GrassMaterial::SetLightDirection(DirectX::XMFLOAT3 dir)
{
	m_LightDirection = dir;
}

void GrassMaterial::LoadEffectVariables()
{
	if (!m_pDiffuseSRVvariable)
	{
		m_pDiffuseSRVvariable = GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseSRVvariable->IsValid())
		{
			Logger::LogWarning(L"DiffuseMaterial::LoadEffectVariables() > \'gDiffuseMap\' variable not found!");
			m_pDiffuseSRVvariable = nullptr;
		}
	}
	if (!m_TimeVariable)
	{
		m_TimeVariable = GetEffect()->GetVariableByName("m_Time") -> AsScalar();
		if (!m_TimeVariable->IsValid())
		{
			Logger::LogWarning(L"DiffuseMaterial::LoadEffectVariables() >\'m_Time\' variable not found!");
				m_TimeVariable = nullptr;
		}
	}

	if (!m_WindVelocityVariable)
	{
		m_WindVelocityVariable = GetEffect()->GetVariableByName("m_WindVelocity")->AsScalar();
		if (!m_WindVelocityVariable->IsValid())
		{
			Logger::LogWarning(L"DiffuseMaterial::LoadEffectVariables() >\'m_WindVelocity\' variable not found!");
			m_WindVelocityVariable = nullptr;
		}
	}

	m_pShadowSRVvariable = GetEffect()->GetVariableByName("gShadowMap")->AsShaderResource();
	m_pLightDirectionVariable = GetEffect()->GetVariableByName("gLightDirection")->AsVector();
	m_pLightWVPvariable = GetEffect()->GetVariableByName("gWorldViewProj_Light")->AsMatrix();
}

void GrassMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	if (m_pDiffuseTexture && m_pDiffuseSRVvariable)
	{
		m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	}
	if (m_TimeVariable)
	{
		m_TimeVariable->SetFloat(m_Time);
	}
	if (m_WindVelocityVariable)
	{
		m_WindVelocityVariable->SetFloat(m_WindVelocity);
	}

	m_pShadowSRVvariable->SetResource(gameContext.pShadowMapper->GetShadowMap());
	m_pLightDirectionVariable->SetFloatVector(reinterpret_cast<const float*>(&m_LightDirection));

	DirectX::XMFLOAT4X4 lightVP = gameContext.pShadowMapper->GetLightVP();
	DirectX::XMFLOAT4X4 world = pModelComponent->GetTransform()->GetWorld();
	DirectX::XMFLOAT4X4 lightWVP{};
	DirectX::XMStoreFloat4x4(&lightWVP, DirectX::XMLoadFloat4x4(&world) * DirectX::XMLoadFloat4x4(&lightVP));
	m_pLightWVPvariable->SetMatrix(reinterpret_cast<float*>(&lightWVP));
}