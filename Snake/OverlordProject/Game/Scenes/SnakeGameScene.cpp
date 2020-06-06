#include "stdafx.h"
#include "SnakeGameScene.h"
#include "FixedCamera.h"
#include "Components.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "../../Materials/ColorMaterial.h"
#include "../../Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "../../Materials/GrassMaterial.h"
#include "../../Materials/Shadow/SkinnedDiffuseMaterial_Shadow.h"
#include "../Prefabs/Environment/Grass.h"
#include "../Prefabs/Snake/Snake.h"
#include "../Prefabs/Pickups/Pickup.h"
#include "../Prefabs/UI/UI.h"
#include "../Prefabs/Environment/PostOutline.h"
#include "ModelAnimator.h"
#include "SceneManager.h"
#include "SoundManager.h"


SnakeGameScene::SnakeGameScene() :
	GameScene(L"SnakeGameScene")
{

}

void SnakeGameScene::Initialize()
{
	auto physX = PhysxManager::GetInstance()->GetPhysics();
	//GetPhysxProxy()->EnablePhysxDebugRendering(true);
	GetGameContext().pShadowMapper->SetLight({ -200,66.1346436f,-41.1850471f }, { 0.740129888f, -0.597205281f, 0.309117377f });

	std::cout << "INFO: Use Arrow Keys to turn and Space to grow the snake.\n";
	
	// MATERIALS
	DiffuseMaterial_Shadow* pMat = new DiffuseMaterial_Shadow();
	pMat->SetDiffuseTexture(L"Resources/Textures/Head.jpg");
	pMat->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
	GetGameContext().pMaterialManager->AddMaterial(pMat, 11);

	pMat = new DiffuseMaterial_Shadow();
	pMat->SetDiffuseTexture(L"Resources/Textures/Body.jpg");
	pMat->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
	GetGameContext().pMaterialManager->AddMaterial(pMat, 12);

	pMat = new DiffuseMaterial_Shadow();
	pMat->SetDiffuseTexture(L"Resources/Textures/DarkTile.png");
	pMat->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
	GetGameContext().pMaterialManager->AddMaterial(pMat, 1);

	pMat = new DiffuseMaterial_Shadow();
	pMat->SetDiffuseTexture(L"Resources/Textures/Apple_BaseColor.png");
	pMat->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
	GetGameContext().pMaterialManager->AddMaterial(pMat, 5);


	pMat = new DiffuseMaterial_Shadow();
	pMat->SetDiffuseTexture(L"Resources/Textures/LightTile.png");
	pMat->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
	GetGameContext().pMaterialManager->AddMaterial(pMat, 2);

	m_pMatGrass = new GrassMaterial();
	m_pMatGrass->SetDiffuseTexture(L"Resources/Textures/LightTile.png");
	m_pMatGrass->SetTime(GetGameContext().pGameTime->GetElapsed());
	m_pMatGrass->SetWindVelocity(4);
	m_pMatGrass->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
	GetGameContext().pMaterialManager->AddMaterial(m_pMatGrass, 3);


	pMat = new DiffuseMaterial_Shadow();
	pMat->SetDiffuseTexture(L"Resources/Textures/Rocks.png");
	pMat->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
	GetGameContext().pMaterialManager->AddMaterial(pMat, 4);

	auto pMat2 = new SkinnedDiffuseMaterial_Shadow();
	pMat2->SetDiffuseTexture(L"Resources/Textures/Ant.png");
	pMat2->SetLightDirection(GetGameContext().pShadowMapper->GetLightDirection());
	GetGameContext().pMaterialManager->AddMaterial(pMat2, 6);

	
	//GROUND PLANE
	auto pBouncyMaterial = physX->createMaterial(0, 0, 1);
	auto pGround = new GameObject();
	pGround->AddComponent(new RigidBodyComponent(true));

	std::shared_ptr<physx::PxGeometry> geom(new physx::PxPlaneGeometry());
	pGround->AddComponent(new ColliderComponent(geom, *pBouncyMaterial, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
	AddChild(pGround);

	// GRASS
	GameObject* pGrass = new Grass();
	AddChild(pGrass);

	
	

	// CAMERA
	const auto gameContext = GetGameContext();
	CameraComponent* cameraComp = new CameraComponent();
	auto camera = new FixedCamera();
	AddChild(camera);
	camera->AddComponent(cameraComp);
	cameraComp->SetActive();
	camera->GetTransform()->Rotate(60, 0, 0);
	camera->GetTransform()->Translate(0, 300, -200);


	// SNAKE
	m_pSnake = new Snake();
	AddChild(m_pSnake);

	
	// GRID
	float offset = 3.5f *10;
	int k = 0;
	int n = 0;
	for (size_t i{}; i < 11; ++i)
	{
		for (size_t j{}; j < 8; ++j)
		{
			k++;
			Grid* pGrid = new Grid();
			m_pGrid.push_back(pGrid);
			pGrid->GetTransform()->Translate(offset * i - (5.0f * 35.0f), 0, offset * j - (3.5f * 35.0f));
			++n;
			k = k % 2;
			if (k == 0)
				pGrid->setIsDark(false);
			else
				pGrid->setIsDark(true);
			AddChild(pGrid);
			if (j == 7)
			{
				k++;
				k = k % 2;
			}
		}
	}

	// PICKUP
	m_pPickup = new Pickup(PickupType::APPLE);
	m_pPickup->GetTransform()->Translate(m_pGrid[46]->GetTransform()->GetPosition());
	AddChild(m_pPickup);
	
	// POST PROCESSING
	m_pOutline = new PostOutline();
	AddPostProcessingEffect(m_pOutline);

	// UI
	m_pUI = new UI();
	AddChild(m_pUI);
	
	// INPUT
	InputManager* manager = GetGameContext().pInput;
	manager->AddInputAction(InputAction{ 10, InputTriggerState::Pressed, VK_SPACE });
	manager->AddInputAction(InputAction{ 11, InputTriggerState::Pressed, VK_F9 });
	manager->AddInputAction(InputAction{ 12, InputTriggerState::Pressed, VK_F8 });
	manager->AddInputAction(InputAction{ 13, InputTriggerState::Pressed, VK_LBUTTON });

	// SOUND
	SoundManager::GetInstance()->GetSystem()->createSound("./Resources/Sounds/Game.mp3", FMOD_2D, 0, &m_pGameMusic);
	m_pGameMusic->setMode(FMOD_LOOP_NORMAL);
	SoundManager::GetInstance()->GetSystem()->playSound(m_pGameMusic, 0, true, &m_pChannel);
}

void SnakeGameScene::Update()
{
	
	if(m_pUI->ShouldReset())
	{
		Reset();
		return;
	}

	if(!m_pSnake->GetChild<SnakeHead>()->HitObject() && !m_pSnake->IsPaused())
		m_GameTime += GetGameContext().pGameTime->GetElapsed();
	
	if(m_pSnake->GetChild<SnakeHead>()->HitObject())
		m_pChannel->setPaused(true);

	
	if (GetGameContext().pInput->IsActionTriggered(10))
	{
		m_pSnake->Grow();
		m_pOutline->SetActive(false);
	}

	if (GetGameContext().pInput->IsActionTriggered(11))
	{
		m_pSnake->SetPause(true);
	}
	if (GetGameContext().pInput->IsActionTriggered(12))
	{
		m_pSnake->SetPause(false);
	}

	m_pMatGrass->SetTime(GetGameContext().pGameTime->GetElapsed());

	if(m_pSnake->GetPickedup())
	{
		m_Points += 50;

		if (m_pSnake->GetChild<SnakeHead>()->GetSnakeSize() < int(m_pGrid.size()-1))
		{
			RemoveChild(m_pPickup);
			m_pPickup->SetActive(false);
			
			int j = std::rand() % m_pGrid.size();
			while (!m_pGrid[j]->GetIsFree())
			{
				j = std::rand() % m_pGrid.size();
			}
			std::cout << j << "\n";
			int r = std::rand() % 2;
			if (r == 0)
				m_pPickup = new Pickup(PickupType::APPLE);
			else
				m_pPickup = new Pickup(PickupType::ANT);
			AddChild(m_pPickup);
			if (r == 0)
				m_pPickup->GetComponent<RigidBodyComponent>()->GetTransform()->Translate(m_pGrid[j]->GetTransform()->GetPosition());
			else
				m_pPickup->GetComponent<RigidBodyComponent>()->GetTransform()->Translate(m_pGrid[j]->GetTransform()->GetPosition().x, m_pGrid[j]->GetTransform()->GetPosition().y + 6, m_pGrid[j]->GetTransform()->GetPosition().z);

			if (m_pPickup->GetComponent<ModelComponent>()->HasAnimator())
			{
				m_pPickup->GetTransform()->Rotate(0, float(std::rand() % 360), 0);
				m_pPickup->GetComponent<ModelComponent>()->GetAnimator()->SetAnimation(0);
				m_pPickup->GetComponent<ModelComponent>()->GetAnimator()->Play();
			}
		}
		m_pSnake->setPickedUp(false);
	}
	else m_pSnake->setPickedUp(false);

	m_pUI->Update(GetGameContext(), m_pSnake);
	m_pUI->SetPoints(m_Points);
	m_pUI->SetGameTime(m_GameTime);
	if (m_pSnake->GetChild<SnakeHead>()->GetSnakeSize() >= int(m_pGrid.size()-1))
	{
		m_pSnake->GetChild<SnakeHead>()->SetPause(true);
		m_pUI->SetWon(true);
	}
}

void SnakeGameScene::Draw()
{
	
}

void SnakeGameScene::Reset()
{
	// SOUND
	m_pChannel->setPaused(false);

	// UI DATA
	m_GameTime = 0.0f;
	m_Points = 0;
	
	//UI
	RemoveChild(m_pUI);
	m_pUI->SetActive(false);
	m_pUI = new UI();
	AddChild(m_pUI);
	m_pUI->SetPoints(0);
	
	// SNAKE
	RemoveChild(m_pSnake);
	m_pSnake->SetActive(false);
	m_pSnake = new Snake();
	AddChild(m_pSnake);

	// GRID
	for(int i{}; i < 88; ++i)
	{
		m_pGrid[i]->SetIsFree(true);
	}

	// PICKUP
	if(m_pPickup->GetType() == PickupType::APPLE)
		m_pPickup->GetComponent<RigidBodyComponent>()->GetTransform()->Translate(m_pGrid[46]->GetTransform()->GetPosition());
	else
		m_pPickup->GetComponent<RigidBodyComponent>()->GetTransform()->Translate(m_pGrid[46]->GetTransform()->GetPosition().x, m_pGrid[46]->GetTransform()->GetPosition().y + 6, m_pGrid[46]->GetTransform()->GetPosition().z);
}

void SnakeGameScene::SceneDeactivated()
{
	Reset();
	m_pChannel->setPaused(true);
}

void SnakeGameScene::SceneActivated()
{
	m_pChannel->setPaused(false);
}
