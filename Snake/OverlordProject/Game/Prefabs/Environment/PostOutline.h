#pragma once
#include "PostProcessingMaterial.h"

class ID3D11EffectShaderResourceVariable;

class PostOutline : public PostProcessingMaterial
{
public:
	PostOutline();
	PostOutline(const PostOutline& other) = delete;
	PostOutline(PostOutline&& other) noexcept = delete;
	PostOutline& operator=(const PostOutline& other) = delete;
	PostOutline& operator=(PostOutline&& other) noexcept = delete;
	virtual ~PostOutline() = default;

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;
private:
	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;
};
