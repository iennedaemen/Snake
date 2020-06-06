#include "stdafx.h"
#include "PostProcessingMaterial.h"
#include "RenderTarget.h"
#include "OverlordGame.h"
#include "ContentManager.h"
#include <string>

PostProcessingMaterial::PostProcessingMaterial(std::wstring effectFile, unsigned int renderIndex,
                                               std::string technique)
	: m_IsInitialized(false), 
	  m_pInputLayout(nullptr),
	  m_pInputLayoutSize(0),
	  m_effectFile(std::move(effectFile)),
	  m_InputLayoutID(0),
	  m_RenderIndex(renderIndex),
	  m_pRenderTarget(nullptr),
	  m_pVertexBuffer(nullptr),
	  m_pIndexBuffer(nullptr),
	  m_NumVertices(0),
	  m_NumIndices(0),
	  m_VertexBufferStride(0),
	  m_pEffect(nullptr),
	  m_pTechnique(nullptr),
	  m_TechniqueName(std::move(technique))
{
}

PostProcessingMaterial::~PostProcessingMaterial()
{
	//TODO: delete and/or release necessary objects and/or resources
	SafeRelease(m_pInputLayout);
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);;
	delete (m_pRenderTarget);
}

void PostProcessingMaterial::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	if (!m_IsInitialized)
	{
		//TODO: complete
		//1. LoadEffect (LoadEffect(...))
		LoadEffect(gameContext, m_effectFile);
		//2. CreateInputLaytout (CreateInputLayout(...))
		EffectHelper::BuildInputLayout(gameContext.pDevice, m_pTechnique, &m_pInputLayout, m_pInputLayoutDescriptions, m_pInputLayoutSize, m_InputLayoutID);
		//   CreateVertexBuffer (CreateVertexBuffer(...)) > As a TriangleStrip (FullScreen Quad)
		CreateVertexBuffer(gameContext);
		CreateIndexBuffer(gameContext);
		//3. Create RenderTarget (m_pRenderTarget)
		//		Take a look at the class, figure out how to initialize/create a RenderTarget Object
		//		GameSettings > OverlordGame::GetGameSettings()
		m_pRenderTarget = new RenderTarget(gameContext.pDevice);
		RENDERTARGET_DESC desc;
		desc.Width = OverlordGame::GetGameSettings().Window.Width;
		desc.Height = OverlordGame::GetGameSettings().Window.Height;
		desc.EnableColorSRV = true;
		desc.EnableDepthSRV = true;
		desc.EnableDepthBuffer = true;
		desc.EnableColorBuffer = true;
		desc.GenerateMipMaps_Color = true;
		m_pRenderTarget->Create(desc);

		m_IsInitialized = true;
	}
}

bool PostProcessingMaterial::LoadEffect(const GameContext& gameContext, const std::wstring& effectFile)
{
	UNREFERENCED_PARAMETER(gameContext);
	UNREFERENCED_PARAMETER(effectFile);

	//TODO: complete
	//Load Effect through ContentManager
	m_pEffect = ContentManager::Load<ID3DX11Effect>(effectFile);;
	//Check if m_TechniqueName (default constructor parameter) is set
	if(!m_TechniqueName.empty())
	{
		// If SET > Use this Technique (+ check if valid)
		m_pTechnique = m_pEffect->GetTechniqueByName(m_TechniqueName.c_str());
		
		if(!m_pTechnique)
		{
			Logger::LogWarning(L"PostProcessingMaterial::LoadEffect > m_pTechnique is not valid!");
			m_pTechnique = m_pEffect->GetTechniqueByIndex(0);
		}
	}
	// If !SET > Use Technique with index 0
	else m_pTechnique = m_pEffect->GetTechniqueByIndex(0);

	//Call LoadEffectVariables
	LoadEffectVariables();

	return true;
}

void PostProcessingMaterial::Draw(const GameContext& gameContext, RenderTarget* previousRendertarget)
{
	UNREFERENCED_PARAMETER(previousRendertarget);

	//TODO: complete
	//1. Clear the object's RenderTarget (m_pRenderTarget) [Check RenderTarget Class]
	const FLOAT clearColor[4]{ 1.0f, 0.0f, 0.0f, 1.0f };
	m_pRenderTarget->Clear(gameContext, clearColor);
	
	//2. Call UpdateEffectVariables(...)
	UpdateEffectVariables(previousRendertarget);;
	
	//3. Set InputLayout
	gameContext.pDeviceContext->IASetInputLayout(m_pInputLayout);
	
	//4. Set VertexBuffer
	UINT strides = sizeof(VertexPosTex);
	UINT offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &strides, &offset);
	gameContext.pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	
	//5. Set PrimitiveTopology (TRIANGLELIST)
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	//6. Draw 
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	for (UINT p{}; p < techDesc.Passes; ++p)
	{
		m_pTechnique->GetPassByIndex(p)->Apply(0, gameContext.pDeviceContext);
		gameContext.pDeviceContext->DrawIndexed(6, 0, 0);
	}
	
	// Generate Mips
	gameContext.pDeviceContext->GenerateMips(m_pRenderTarget->GetShaderResourceView());
}

void PostProcessingMaterial::CreateVertexBuffer(const GameContext& gameContext)
{
	m_NumVertices = 4;

	UNREFERENCED_PARAMETER(gameContext);
	//TODO: complete
	//Create vertex array containing three elements in system memory
		std::vector<VertexPosTex> vertices =
	{
			VertexPosTex(DirectX::XMFLOAT3(-1, -1, 0), DirectX::XMFLOAT2(0, 1)),
			VertexPosTex(DirectX::XMFLOAT3(-1, 1, 0), DirectX::XMFLOAT2(0, 0)),
			VertexPosTex(DirectX::XMFLOAT3(1, -1, 0), DirectX::XMFLOAT2(1, 1)),
			VertexPosTex(DirectX::XMFLOAT3(1, 1, 0), DirectX::XMFLOAT2(1, 0))
	};
	//fill a buffer description to copy the vertexdata into graphics memory
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(VertexPosTex) * m_NumVertices;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = vertices.data();
	
	//create a ID3D10Buffer in graphics memory containing the vertex info
	gameContext.pDevice->CreateBuffer(&desc, &initData, &m_pVertexBuffer);
}

void PostProcessingMaterial::CreateIndexBuffer(const GameContext& gameContext)
{
	m_NumIndices = 6;

	UNREFERENCED_PARAMETER(gameContext);
	//TODO: complete
	// Create index buffer
	if (m_pIndexBuffer != nullptr)
	return;

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(DWORD) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	std::vector<DWORD> vec = { 0, 1, 2, 2, 3, 1 };
	initData.pSysMem = vec.data();

	auto hr = gameContext.pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	Logger::LogHResult(hr, L"PostProcessingMaterial::CreateIndexBuffer()");
}
