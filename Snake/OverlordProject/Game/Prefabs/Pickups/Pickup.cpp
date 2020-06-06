#include "stdafx.h"
#include "Pickup.h"
#include "PhysxManager.h"
#include "Components.h"

void Pickup::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	
	physx::PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();
	float size = 1.0f;

	if (m_Type == PickupType::APPLE)
	{
		ModelComponent* pModelComp = new ModelComponent(L"Resources/Meshes/Apple.ovm");
		AddComponent(pModelComp);
		pModelComp->GetTransform()->Scale(0.2f, 0.2f, 0.2f);
		pModelComp->SetMaterial(5);
	}
	if (m_Type == PickupType::ANT)
	{
		ModelComponent* pModelComp = new ModelComponent(L"Resources/Meshes/Ant.ovm");
		AddComponent(pModelComp);
		pModelComp->GetTransform()->Scale(0.01f, 0.01f, 0.01f);
		pModelComp->SetMaterial(6);
	}

	RigidBodyComponent* pRigidBody = new RigidBodyComponent();
	pRigidBody->SetKinematic(true);
	AddComponent(pRigidBody);

	std::shared_ptr<physx::PxGeometry> geom = std::make_shared<physx::PxBoxGeometry>(size * 0.5f, size * 0.5f, size * 0.5f);
	auto pDefaultMaterial = physX->createMaterial(0.0f, 0.0f, 1.0f);
	auto collider = new ColliderComponent(geom, *pDefaultMaterial);
	AddComponent(collider);

	pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group5);
	pRigidBody->SetCollisionIgnoreGroups(CollisionGroupFlag::Group2);
	SetTag(L"Pickup");
}

void Pickup::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

}
