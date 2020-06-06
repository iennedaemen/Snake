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
	//TODO: Bind the 'gTexture' variable with 'm_pTextureMapVariable'
	m_pTextureMapVariabele = GetEffect()->GetVariableByName("gTexture")->AsShaderResource();
	//Check if valid!
	if (!m_pTextureMapVariabele) 
		Logger::LogError(L"PostGrayscale::LoadEffectVariables() > 'm_pTextureMapVariabele' not valid!");
}

void PostOutline::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	//TODO: Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget
	m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
}