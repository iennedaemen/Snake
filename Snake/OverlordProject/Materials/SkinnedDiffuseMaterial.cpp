#include "stdafx.h"

#include "SkinnedDiffuseMaterial.h"

#include "ContentManager.h"
#include "TextureData.h"
#include "ModelComponent.h"
#include "ModelAnimator.h"

ID3DX11EffectShaderResourceVariable* SkinnedDiffuseMaterial::m_pDiffuseSRVvariable = nullptr;
ID3DX11EffectMatrixVariable* SkinnedDiffuseMaterial::m_pBoneTransforms = nullptr;

SkinnedDiffuseMaterial::SkinnedDiffuseMaterial() : Material(L"./Resources/Effects/PosNormTex3D_Skinned.fx"),
	m_pDiffuseTexture(nullptr)
{}

void SkinnedDiffuseMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void SkinnedDiffuseMaterial::LoadEffectVariables()
{
	if (!m_pDiffuseSRVvariable)
	{
		m_pDiffuseSRVvariable = GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();
		if (!m_pDiffuseSRVvariable->IsValid())
		{
			Logger::LogWarning(L"SkinnedDiffuseMaterial::LoadEffectVariables() > \'gDiffuseMap\' variable not found!");
			m_pDiffuseSRVvariable = nullptr;
		}
	}

	//TODO: Create a link to the gBones variable
	if (!m_pBoneTransforms)
	{
		m_pBoneTransforms = GetEffect()->GetVariableByName("gBones")->AsMatrix();
		if (!m_pBoneTransforms->IsValid())
		{
			if (!m_pDiffuseSRVvariable->IsValid())
			{
				Logger::LogWarning(L"SkinnedDiffuseMaterial::LoadEffectVariables() > \'gBones\' variable not found!");
				m_pDiffuseSRVvariable = nullptr;
			}
		}
	}
}

void SkinnedDiffuseMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);

	if (m_pDiffuseTexture && m_pDiffuseSRVvariable)
	{
		m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
	}

	//TODO: Set the matrix array (BoneTransforms of the ModelAnimator)}
	if (m_pBoneTransforms)
	{
		std::vector<DirectX::XMFLOAT4X4> boneTransforms = pModelComponent->GetAnimator()->GetBoneTransforms();
		m_pBoneTransforms->SetMatrixArray(reinterpret_cast<float*>(&boneTransforms[0]), 0, boneTransforms.size());
	}
}