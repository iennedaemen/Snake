#pragma once
#include "Material.h"
#include <string>

class TextureData;

class DiffuseMaterial : public Material
{
public:
	DiffuseMaterial();
	~DiffuseMaterial();

	void SetDiffuseTexture(const std::wstring& assetfile);
	
protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;

private:
	TextureData* m_pDiffuseTexture;
	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;

	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	DiffuseMaterial(const DiffuseMaterial& obj);
	DiffuseMaterial& operator=(const DiffuseMaterial& obj);

	
};

