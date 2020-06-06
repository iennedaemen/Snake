#pragma once
#include "Material.h"
class ModelComponent;
class TextureData;

class OutlineMaterial : public Material
{
public:
	OutlineMaterial();
	~OutlineMaterial();

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
	OutlineMaterial(const OutlineMaterial& obj);
	OutlineMaterial& operator=(const OutlineMaterial& obj);


};

