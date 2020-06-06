#include "stdafx.h"
#include "OutlineMaterial.h"
#include "ContentManager.h"
#include "TextureData.h"

ID3DX11EffectShaderResourceVariable* OutlineMaterial::m_pDiffuseSRVvariable = nullptr;

OutlineMaterial::OutlineMaterial() :
	Material(L"./Resources/Effects/Outline.fx")
{}

OutlineMaterial::~OutlineMaterial()
{}

void OutlineMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
}

void OutlineMaterial::LoadEffectVariables()
{
	m_pDiffuseSRVvariable = Material::GetEffect()->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

void OutlineMaterial::UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pModelComponent);
	m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());
}
