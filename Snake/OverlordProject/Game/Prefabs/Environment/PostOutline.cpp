#include "stdafx.h"
#include "PostOutline.h"
#include "RenderTarget.h"

PostOutline::PostOutline()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Outline.fx", 1),
	m_pTextureMapVariabele(nullptr)
{
}

void PostOutline::LoadEffectVariables()
{
	m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();

	if (!m_pTextureMapVariabele) 
		Logger::LogError(L"PostGrayscale::LoadEffectVariables() > 'm_pTextureMapVariabele' not valid!");
}

void PostOutline::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
}