#include "stdafx.h"
#include "ShadowMapRenderer.h"
#include "ContentManager.h"
#include "ShadowMapMaterial.h"
#include "RenderTarget.h"
#include "MeshFilter.h"
#include "SceneManager.h"
#include "OverlordGame.h"

ShadowMapRenderer::~ShadowMapRenderer()
{
	SafeDelete(m_pShadowRT);
	SafeDelete(m_pShadowMat);
}

void ShadowMapRenderer::Initialize(const GameContext& gameContext)
{
	if (m_IsInitialized)
		return;

	// Create shadow generator material + initialize it
	m_pShadowMat = new ShadowMapMaterial();
	m_pShadowMat->Initialize(gameContext);
	
	// Create a rendertarget
	m_pShadowRT = new RenderTarget{ gameContext.pDevice };
	RENDERTARGET_DESC renderDesc{};
	renderDesc.EnableColorBuffer = false;
	renderDesc.EnableColorSRV = false;
	renderDesc.EnableDepthBuffer = true;
	renderDesc.EnableDepthSRV = true;
	renderDesc.Width = OverlordGame::GetGameSettings().Window.Width;
	renderDesc.Height = OverlordGame::GetGameSettings().Window.Height;
	m_pShadowRT->Create(renderDesc);
	
	m_IsInitialized = true;
}

void ShadowMapRenderer::SetLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction)
{	
	// Store the input parameters in the appropriate datamembers
	m_LightPosition = position;
	m_LightDirection = direction;
	// Calculate the Light VP matrix (Directional Light only ;)) and store it in the appropriate datamember
	DirectX::XMVECTOR up = DirectX::XMVector2Cross(DirectX::XMVector3Normalize(XMLoadFloat3(&direction)), { 0,1,0 });
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&position), XMLoadFloat3(&direction), up);
	DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixOrthographicLH( 800, 800, 0.1f, 900);
	DirectX::XMStoreFloat4x4(&m_LightVP, viewMatrix * projectionMatrix);
}

void ShadowMapRenderer::Begin(const GameContext& gameContext) const
{
	//Reset Texture Register 5 (Unbind)
	ID3D11ShaderResourceView *const pSRV[] = { nullptr };
	gameContext.pDeviceContext->PSSetShaderResources(1, 1, pSRV);

	// Set the appropriate render target that our shadow generator will write to
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(m_pShadowRT);
	// Clear this render target
	float black[4]{ 0,0,0,1 };
	m_pShadowRT->Clear(gameContext, black);
	// Set the shader variables of this shadow generator material
	m_pShadowMat->SetLightVP(m_LightVP);
}

void ShadowMapRenderer::End(const GameContext& gameContext) const
{
	UNREFERENCED_PARAMETER(gameContext);
	
	// Restore default render target
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(nullptr);
}

void ShadowMapRenderer::Draw(const GameContext& gameContext, MeshFilter* pMeshFilter, DirectX::XMFLOAT4X4 world, const std::vector<DirectX::XMFLOAT4X4>& bones) const
{
	// Update shader variables in material
	m_pShadowMat->SetLightVP(m_LightVP);
	m_pShadowMat->SetWorld(world);
	m_pShadowMat->SetBones(reinterpret_cast<const float*>(bones.data()), bones.size());

	// Set the correct inputlayout, buffers, topology
	ShadowMapMaterial::ShadowGenType genType{};
	if (bones.empty())
		genType = ShadowMapMaterial::ShadowGenType::Static;
	else 
		genType = ShadowMapMaterial::ShadowGenType::Skinned;
	
	gameContext.pDeviceContext->IASetInputLayout(m_pShadowMat->m_pInputLayouts[genType]);
	VertexBufferData vertexBufferData = pMeshFilter->GetVertexBufferData(gameContext, m_pShadowMat->m_InputLayoutIds[genType]);
	UINT offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &vertexBufferData.pVertexBuffer, &vertexBufferData.VertexStride, &offset);
	gameContext.pDeviceContext->IASetIndexBuffer(pMeshFilter->m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Invoke draw call
	ID3DX11EffectTechnique* pTechnique = m_pShadowMat->m_pShadowTechs[genType];
	D3DX11_TECHNIQUE_DESC techDesc{};
	pTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(pMeshFilter->m_IndexCount, 0, 0);
	}
}

void ShadowMapRenderer::UpdateMeshFilter(const GameContext& gameContext, MeshFilter* pMeshFilter)
{
	// Based on the type (Skinned or Static) build the correct vertex buffers for the MeshFilter
	ShadowMapMaterial::ShadowGenType genType{};
	if (pMeshFilter->m_BoneCount == 0)
		genType = ShadowMapMaterial::ShadowGenType::Static;
	else 
		genType = ShadowMapMaterial::ShadowGenType::Skinned;

	pMeshFilter->BuildVertexBuffer(
		gameContext,
		m_pShadowMat->m_InputLayoutIds[genType],
		m_pShadowMat->m_InputLayoutSizes[genType],
		m_pShadowMat->m_InputLayoutDescriptions[genType]);
}

ID3D11ShaderResourceView* ShadowMapRenderer::GetShadowMap() const
{
	return m_pShadowRT->GetDepthShaderResourceView();
}
