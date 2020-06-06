#include "stdafx.h"
#include "SnakeBody.h"
#include "ControllerComponent.h"
#include "PhysxManager.h"
#include "Components.h"
#include "ContentManager.h"
#include <queue>


SnakeBody::SnakeBody(bool hasHitBox, bool active)
	: m_HasHitBox(hasHitBox)
	, m_Active(active)
{

}

void SnakeBody::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	ModelComponent* pModelComp = new ModelComponent(L"Resources/Meshes/Body.ovm");
	AddComponent(pModelComp);
	pModelComp->GetTransform()->Scale(0.0f, 0.0f, 0.0f);
	pModelComp->SetMaterial(12);

	if (m_HasHitBox)
	{
		RigidBodyComponent* pRigidBody = new RigidBodyComponent();
		pRigidBody->SetKinematic(true);
		AddComponent(pRigidBody);

		physx::PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();
		physx::PxMaterial* pGroundMat = physX->createMaterial(0.5f, 0.5f, 0.0f);
		std::shared_ptr<physx::PxGeometry> pBoxGeometry(new physx::PxBoxGeometry(physx::PxVec3{ 5, 5, 5 }));
		ColliderComponent* pCollider = new ColliderComponent(pBoxGeometry, *pGroundMat);
		AddComponent(pCollider);

		pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group2);
		pRigidBody->SetCollisionIgnoreGroups(CollisionGroupFlag::Group1);
		pRigidBody->SetCollisionIgnoreGroups(CollisionGroupFlag::Group5);

		SetTag(L"SnakeBody");
	}
	
	if (!m_Active)
		GetTransform()->Translate(-1000, 0, 0);
}

void SnakeBody::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

}

void SnakeBody::Update(const GameContext& gameContext, DirectX::XMFLOAT3 lookatPos)
{
	if (m_Active)
	{
		const float elapsedSec = gameContext.pGameTime->GetElapsed();

		// Look at
		DirectX::XMFLOAT3 targetPoint;
		targetPoint.x = (lookatPos.x - GetTransform()->GetPosition().x);
		targetPoint.y = (lookatPos.y - GetTransform()->GetPosition().y);
		targetPoint.z = (lookatPos.z - GetTransform()->GetPosition().z);

		float targetRotation = atan2(targetPoint.x, targetPoint.z);
		GetTransform()->Rotate(0, GetTransform()->GetRotation().y + DirectX::XMConvertToDegrees(targetRotation), 0);

		// Move
		if (!m_Start)
		{
			m_AcumulatedSec += elapsedSec;
			if (m_Scale < 1.0f)
				m_Scale += 1.0f * elapsedSec;
			else m_Scale = 1.0f;
			
			if (m_AcumulatedSec > 1.0f)
			{
				m_Scale = 1.0f;
				m_Start = true;
				m_CanGetTail = true;
			}
			GetComponent<ModelComponent>()->GetTransform()->Scale(m_Scale, m_Scale, m_Scale);
		}

		if (m_Start)
		{
			if (!m_Positions.empty())
			{
				GetTransform()->Translate(m_Positions.front().x, m_Positions.front().y, m_Positions.front().z);
				m_Positions.pop();
			}

		}
	}

}

void SnakeBody::AddPosition(DirectX::XMFLOAT3 position)
{
	m_Positions.push(position);
}

void SnakeBody::Activate()
{
	m_Active = true;
}

bool SnakeBody::CanGetTail()
{
	return m_CanGetTail;
}
void SnakeBody::SetCanGetTail(bool canGetTail)
{
	m_CanGetTail = canGetTail;
}