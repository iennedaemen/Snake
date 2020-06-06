#pragma once
#include "Material.h"

class TextureData;

class GrassMaterial final: public Material
{
public:
	GrassMaterial();
	~GrassMaterial();

	void SetDiffuseTexture(const std::wstring& assetFile);
	void SetTime(const float time);
	void SetWindVelocity(const float velocity);
	void SetLightDirection(DirectX::XMFLOAT3 dir);
	
protected:
	virtual void LoadEffectVariables();
	virtual void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent);

	TextureData* m_pDiffuseTexture;
	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;
	float m_Time = 0;
	static ID3DX11EffectScalarVariable* m_TimeVariable;
	float m_WindVelocity = 4;
	static ID3DX11EffectScalarVariable* m_WindVelocityVariable;

	DirectX::XMFLOAT3 m_LightDirection = { -0.577f, -0.577f, 0.577f };
	static ID3DX11EffectShaderResourceVariable* m_pShadowSRVvariable;
	static ID3DX11EffectVectorVariable* m_pLightDirectionVariable;
	static ID3DX11EffectMatrixVariable* m_pLightWVPvariable;
	
private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	GrassMaterial(const GrassMaterial&obj);
	GrassMaterial& operator=(const GrassMaterial& obj);
};

