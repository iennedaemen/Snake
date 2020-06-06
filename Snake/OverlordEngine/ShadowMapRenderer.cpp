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
	//TODO: make sure you don't have memory leaks and/or resource leaks :) -> Figure out if you need to do something here
	SafeDelete(m_pShadowRT);
	SafeDelete(m_pShadowMat);
}

void ShadowMapRenderer::Initialize(const GameContext& gameContext)
{
	if (m_IsInitialized)
		return;

	//TODO: create shadow generator material + initialize it
	m_pShadowMat = new ShadowMapMaterial();
	m_pShadowMat->Initialize(gameContext);
	
	//TODO: create a rendertarget with the correct settings (hint: depth only) for the shadow generator using a RENDERTARGET_DESC
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
	UNREFERENCED_PARAMETER(position);
	UNREFERENCED_PARAMETER(direction);
	
	//TODO: store the input parameters in the appropriate datamembers
	m_LightPosition = position;
	m_LightDirection = direction;
	//TODO: calculate the Light VP matrix (Directional Light only ;)) and store it in the appropriate datamember
	DirectX::XMVECTOR up = DirectX::XMVector2Cross(DirectX::XMVector3Normalize(XMLoadFloat3(&direction)), { 0,1,0 });
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&position), XMLoadFloat3(&direction), up);
	DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixOrthographicLH( 800, 800, 0.1f, 900);
	DirectX::XMStoreFloat4x4(&m_LightVP, viewMatrix * projectionMatrix);


	
	//DirectX::XMMATRIX p = DirectX::XMMatrixPerspectiveFovLH(0.78f, 16.0f / 9.0f, 0.1f, 2500.0f);
	////XMMATRIX p = XMMatrixOrthographicLH(25.0f * 16.0f/9.0f, 25.0f, 0.1f, 2500.0f);

	//DirectX::XMVECTOR worldPosition = XMLoadFloat3(&m_LightPosition);
	//DirectX::XMVECTOR dir = XMLoadFloat3(&m_LightDirection);
	//DirectX::XMMATRIX v = DirectX::XMMatrixLookAtLH(worldPosition, DirectX::XMVectorAdd(worldPosition, dir), { 0,1,0 });

	//XMStoreFloat4x4(&m_LightVP, v * p);

}

void ShadowMapRenderer::Begin(const GameContext& gameContext) const
{
	//Reset Texture Register 5 (Unbind)
	ID3D11ShaderResourceView *const pSRV[] = { nullptr };
	gameContext.pDeviceContext->PSSetShaderResources(1, 1, pSRV);

	UNREFERENCED_PARAMETER(gameContext);
	//TODO: set the appropriate render target that our shadow generator will write to (hint: use the OverlordGame::SetRenderTarget function through SceneManager)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(m_pShadowRT);
	//TODO: clear this render target
	float black[4]{ 0,0,0,1 };
	m_pShadowRT->Clear(gameContext, black);
	//TODO: set the shader variables of this shadow generator material
	m_pShadowMat->SetLightVP(m_LightVP);
}

void ShadowMapRenderer::End(const GameContext& gameContext) const
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: restore default render target (hint: passing nullptr to OverlordGame::SetRenderTarget will do the trick)
	SceneManager::GetInstance()->GetGame()->SetRenderTarget(nullptr);
}

void ShadowMapRenderer::Draw(const GameContext& gameContext, MeshFilter* pMeshFilter, DirectX::XMFLOAT4X4 world, const std::vector<DirectX::XMFLOAT4X4>& bones) const
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pMeshFilter);
	UNREFERENCED_PARAMETER(world);
	UNREFERENCED_PARAMETER(bones);

	//TODO: update shader variables in material
	m_pShadowMat->SetLightVP(m_LightVP);
	m_pShadowMat->SetWorld(world);
	m_pShadowMat->SetBones(reinterpret_cast<const float*>(bones.data()), bones.size());

	//TODO: set the correct inputlayout, buffers, topology (some variables are set based on the generation type Skinned or Static)
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

	//TODO: invoke draw call
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
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(pMeshFilter);
	//TODO: based on the type (Skinned or Static) build the correct vertex buffers for the MeshFilter (Hint use MeshFilter::BuildVertexBuffer)
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
	//TODO: return the depth shader resource view of the shadow generator render target
	return m_pShadowRT->GetDepthShaderResourceView();
}
