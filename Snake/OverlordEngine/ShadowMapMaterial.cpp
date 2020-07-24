//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "ShadowMapMaterial.h"
#include "ContentManager.h"

ShadowMapMaterial::~ShadowMapMaterial()
{
	for (unsigned int i = 0; i < NUM_TYPES; i++)
	{
		SafeRelease(m_pInputLayouts[i]);
	}
}

void ShadowMapMaterial::Initialize(const GameContext& gameContext)
{
	if (!m_IsInitialized)
	{
		// Initialize the effect, techniques, shader variables, input layouts, etc.
		m_pShadowEffect = ContentManager::Load<ID3DX11Effect>(L"./Resources/Effects/ShadowMapGenerator.fx");

		m_pWorldMatrixVariable = m_pShadowEffect->GetVariableByName("gWorld")->AsMatrix();
		m_pBoneTransforms = m_pShadowEffect->GetVariableByName("gBones")->AsMatrix();
		m_pLightVPMatrixVariable = m_pShadowEffect->GetVariableByName("gLightViewProj")->AsMatrix();

		m_pShadowTechs[ShadowGenType::Static] = m_pShadowEffect->GetTechniqueByName("GenerateShadows");
		m_pShadowTechs[ShadowGenType::Skinned] = m_pShadowEffect->GetTechniqueByName("GenerateShadows_Skinned");

		EffectHelper::BuildInputLayout(gameContext.pDevice, m_pShadowTechs[ShadowGenType::Static],
			&m_pInputLayouts[ShadowGenType::Static],
			m_InputLayoutDescriptions[ShadowGenType::Static], m_InputLayoutSizes[ShadowGenType::Static],
			m_InputLayoutIds[ShadowGenType::Static]);

		EffectHelper::BuildInputLayout(gameContext.pDevice, m_pShadowTechs[ShadowGenType::Skinned],
			&m_pInputLayouts[ShadowGenType::Skinned],
			m_InputLayoutDescriptions[ShadowGenType::Skinned], m_InputLayoutSizes[ShadowGenType::Skinned],
			m_InputLayoutIds[ShadowGenType::Skinned]);

		m_IsInitialized = true;
	}
}

void ShadowMapMaterial::SetLightVP(DirectX::XMFLOAT4X4 lightVP) const
{
	m_pLightVPMatrixVariable->SetMatrix(reinterpret_cast<const float*>(&lightVP));
}

void ShadowMapMaterial::SetWorld(DirectX::XMFLOAT4X4 world) const
{
	m_pWorldMatrixVariable->SetMatrix(reinterpret_cast<const float*>(&world));
}

void ShadowMapMaterial::SetBones(const float* pData, int count) const
{
	m_pBoneTransforms->SetMatrixArray(pData, 0, count);
}
