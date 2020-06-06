#include "stdafx.h"
#include "SnakeHead.h"
#include "ControllerComponent.h"
#include "PhysxManager.h"
#include "Components.h"
#include "ContentManager.h"
#include "../../structs.h"
#include "../Particles/StarsParticle.h"
#include "SoundManager.h"


SnakeHead::SnakeHead()
{

}

void SnakeHead::Initialize(const GameContext& gameContext)
{
	physx::PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();

	// MODEL
	ModelComponent* pModelComp = new ModelComponent(L"Resources/Meshes/Head.ovm");
	AddComponent(pModelComp);
	pModelComp->SetMaterial(11);

	// CHARACTER CONTROLLER
	auto pMat = physX->createMaterial(0, 0, 0);
	m_pController = new ControllerComponent(pMat, 1.0f, 1.0f);
	AddComponent(m_pController);
	m_pController->SetCollisionIgnoreGroups(CollisionGroupFlag::Group2);
	m_pController->SetCollisionGroup(CollisionGroupFlag::Group1);


	// COLLIDER
	m_pColliderObj = new GameObject();
	auto m_pRigid = new RigidBodyComponent();

	std::shared_ptr<physx::PxGeometry> geom = std::make_shared<physx::PxBoxGeometry>(10.0f, 20.0f, 10.0f);
	auto pDefaultMaterial = physX->createMaterial(0.0f, 0.0f, 1.0f);
	auto collider = new ColliderComponent(geom, *pDefaultMaterial);
	collider->EnableTrigger(true);

	m_pColliderObj->AddComponent(m_pRigid);
	m_pColliderObj->AddComponent(collider);
	m_pColliderObj->SetTag(L"SnakeHead");
	AddChild(m_pColliderObj);
	m_pColliderObj->GetTransform()->Translate(0, 0, 1.0f);

	// TURNER

	m_pTurnObj = new GameObject();
	m_pRigid = new RigidBodyComponent();

	geom = std::make_shared<physx::PxBoxGeometry>(5.0f, 20.0f, 1.0f);
	collider = new ColliderComponent(geom, *pDefaultMaterial);
	collider->EnableTrigger(true);

	m_pTurnObj->AddComponent(m_pRigid);
	m_pTurnObj->AddComponent(collider);
	AddChild(m_pTurnObj);

	// INPUT
	InputManager* manager = gameContext.pInput;
	manager->AddInputAction(InputAction{ CharacterMovement::LEFT, InputTriggerState::Pressed, VK_LEFT });
	manager->AddInputAction(InputAction{ CharacterMovement::RIGHT, InputTriggerState::Pressed, VK_RIGHT });
	manager->AddInputAction(InputAction{ CharacterMovement::FORWARD, InputTriggerState::Pressed, VK_UP });
	manager->AddInputAction(InputAction{ CharacterMovement::BACKWARD, InputTriggerState::Pressed, VK_DOWN });

	// SOUND
	SoundManager::GetInstance()->GetSystem()->createSound("./Resources/Sounds/Eat.wav", FMOD_2D, 0, &m_pEatSound);
	m_pEatSound->setMode(FMOD_LOOP_OFF);
	SoundManager::GetInstance()->GetSystem()->createSound("./Resources/Sounds/Bump.wav", FMOD_2D, 0, &m_pBumpSound);
	m_pBumpSound->setMode(FMOD_LOOP_OFF);
}

void SnakeHead::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
}

void SnakeHead::Update(const GameContext& gameContext)
{
	const float elapsedSec = gameContext.pGameTime->GetElapsed();

	if (!m_HitObject && !m_Pause)
	{
		const float speed{ 3.5f *10};
		const float rotSpeed{ 900 };

		// ROTATION
		if (!m_IsRotating || !m_Turn)
		{
			if (gameContext.pInput->IsActionTriggered(CharacterMovement::LEFT))
			{
				if(m_PreviousDirection == CharacterMovement::LEFT)
				{
					m_Velocity = { -speed,0,0 };
					m_IsRotating = false;
				}
				else if (m_PreviousDirection != CharacterMovement::RIGHT)
				{
					m_Direction = CharacterMovement::LEFT;
					if (m_PreviousDirection == CharacterMovement::FORWARD)
						m_NeededRotationY = -90;
					if (m_PreviousDirection == CharacterMovement::BACKWARD)
						m_NeededRotationY = 90;
					m_Velocity = { -speed,0,0 };
					m_InitRotY = m_RotationY;
					m_IsRotating = true;
				}
			}
			if (gameContext.pInput->IsActionTriggered(CharacterMovement::RIGHT))
			{
				if (m_PreviousDirection == CharacterMovement::RIGHT)
				{
					m_Velocity = { speed,0,0 };
					m_IsRotating = false;
				}
				else if (m_PreviousDirection != CharacterMovement::LEFT)
				{
					if (m_PreviousDirection == CharacterMovement::FORWARD)
						m_NeededRotationY = 90;
					if (m_PreviousDirection == CharacterMovement::BACKWARD)
						m_NeededRotationY = -90;
					m_Direction = CharacterMovement::RIGHT;
					m_Velocity = { speed,0,0 };
					m_InitRotY = m_RotationY;
					m_IsRotating = true;
				}
			}
			if (gameContext.pInput->IsActionTriggered(CharacterMovement::FORWARD))
			{
				if(m_PreviousDirection == CharacterMovement::FORWARD)
				{
					m_Velocity = { 0,0,speed };
					m_IsRotating = false;
				}
				else if (m_PreviousDirection != CharacterMovement::BACKWARD)
				{
					if (m_PreviousDirection == CharacterMovement::RIGHT)
						m_NeededRotationY = -90;
					if (m_PreviousDirection == CharacterMovement::LEFT)
						m_NeededRotationY = 90;
					m_Direction = CharacterMovement::FORWARD;
					m_Velocity = { 0,0,speed };
					m_InitRotY = m_RotationY;
					m_IsRotating = true;
				}
			}
			if (gameContext.pInput->IsActionTriggered(CharacterMovement::BACKWARD))
			{
				if(m_PreviousDirection == CharacterMovement::BACKWARD)
				{
					m_Velocity = { 0,0,-speed };
					m_IsRotating = false;
				}
				else if (m_PreviousDirection != CharacterMovement::FORWARD)
				{
					if (m_PreviousDirection == CharacterMovement::RIGHT)
						m_NeededRotationY = 90;
					if (m_PreviousDirection == CharacterMovement::LEFT)
						m_NeededRotationY = -90;
					m_Direction = CharacterMovement::BACKWARD;
					m_Velocity = { 0,0,-speed };
					m_InitRotY = m_RotationY;
					m_IsRotating = true;
				}
			}
		}

		if (m_Direction != m_PreviousDirection && m_IsRotating && m_Turn)
		{
			if (m_NeededRotationY > 0)
			{
				if (m_RotationY < m_InitRotY + m_NeededRotationY)
				{
					m_RotationY = m_RotationY + rotSpeed * elapsedSec;
					GetTransform()->Rotate(0, m_RotationY, 0);
					m_pColliderObj->GetTransform()->Rotate(0, m_RotationY, 0);
					m_pTurnObj->GetTransform()->Rotate(0, m_RotationY, 0);
				}
				else
				{
					m_PreviousDirection = m_Direction;
					m_IsRotating = false;
					m_Turn = false;
					CheckRotation();
				}
			}
			else
			{
				if (m_RotationY > m_InitRotY + m_NeededRotationY)
				{
					m_RotationY = m_RotationY - rotSpeed * elapsedSec;
					GetTransform()->Rotate(0, m_RotationY, 0);
					m_pColliderObj->GetTransform()->Rotate(0, m_RotationY, 0);
					m_pTurnObj->GetTransform()->Rotate(0, m_RotationY, 0);
				}
				else
				{
					m_PreviousDirection = m_Direction;
					m_IsRotating = false;
					m_Turn = false;
					CheckRotation();
				}
			}
		}

		// MOVEMENT
		m_Velocity.x = speed * GetTransform()->GetForward().x;
		m_Velocity.y = speed * GetTransform()->GetForward().y;
		m_Velocity.z = speed * GetTransform()->GetForward().z;
		m_pController->Move(DirectX::XMFLOAT3{ m_Velocity.x * elapsedSec,m_Velocity.y * elapsedSec,m_Velocity.z * elapsedSec });

		m_pColliderObj->GetTransform()->Translate(this->GetTransform()->GetWorldPosition());
		m_pTurnObj->GetTransform()->Translate(this->GetTransform()->GetWorldPosition());

		// TRIGGER
		Trigger();
	}
}


void SnakeHead::CheckRotation()
{
	if (m_Direction == CharacterMovement::FORWARD)
	{
		GetTransform()->Rotate(0, 0, 0);
		m_pColliderObj->GetTransform()->Rotate(0, 0, 0);
		m_pTurnObj->GetTransform()->Rotate(0, 0, 0);
		m_RotationY = 0;
	}
	if (m_Direction == CharacterMovement::BACKWARD)
	{
		GetTransform()->Rotate(0, 180, 0);
		m_pColliderObj->GetTransform()->Rotate(0, 180, 0);
		m_pTurnObj->GetTransform()->Rotate(0, 180, 0);
		m_RotationY = 180;
	}
	if (m_Direction == CharacterMovement::LEFT)
	{
		GetTransform()->Rotate(0, -90, 0);
		m_pColliderObj->GetTransform()->Rotate(0, -90, 0);
		m_pTurnObj->GetTransform()->Rotate(0, -90, 0);
		m_RotationY = -90;
	}
	if (m_Direction == CharacterMovement::RIGHT)
	{
		GetTransform()->Rotate(0, 90, 0);
		m_pColliderObj->GetTransform()->Rotate(0, 90, 0);
		m_pTurnObj->GetTransform()->Rotate(0, 90, 0);
		m_RotationY = 90;
	}
}


void SnakeHead::Trigger()
{
	m_pColliderObj->SetOnTriggerCallBack([this]
	(GameObject* triggerobject, GameObject* otherobject, GameObject::TriggerAction action)
	{
		UNREFERENCED_PARAMETER(triggerobject);
		if (action == GameObject::TriggerAction::ENTER)
		{
			if (otherobject->GetTag() == L"SnakeBody" || otherobject->GetTag() == L"Wall")
			{
				m_HitObject = true;
				std::cout << "Ouch!\n";
				StarsParticle* pStarsParticle = new StarsParticle({ 0,20,0 });
				AddChild(pStarsParticle);
				SoundManager::GetInstance()->GetSystem()->playSound(m_pBumpSound, 0, false, &m_pChannel);
			}
			if (otherobject->GetTag() == L"Pickup")
			{
				m_PickedUp = true;
				std::cout << "Njom!\n";
				SoundManager::GetInstance()->GetSystem()->playSound(m_pEatSound, 0, false, &m_pChannel);
			}
		}
	}
	);

	m_pTurnObj->SetOnTriggerCallBack([this]
	(GameObject* triggerobject, GameObject* otherobject, GameObject::TriggerAction action)
	{
		UNREFERENCED_PARAMETER(triggerobject);
		if (action == GameObject::TriggerAction::ENTER)
		{
			if (otherobject->GetTag() == L"Grid")
			{
				std::cout << "Grid!\n";
				m_Turn = true;
				m_Grids.push(static_cast<Grid*>(otherobject));
				m_Grids.back()->SetIsFree(false);
				if (int(m_Grids.size()) > m_SnakeSize)
				{
					m_Grids.front()->SetIsFree(true);
					m_Grids.pop();
				}
			}
		}
		if (action == GameObject::TriggerAction::LEAVE)
		{
			if (otherobject->GetTag() == L"Grid")
			{
				if (!m_IsRotating)
					m_Turn = false;
			}
		}
	}
	);
}

