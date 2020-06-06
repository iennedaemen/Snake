#include "stdafx.h"
#include "Grid.h"
#include "CubePrefab.h"
#include "PhysxManager.h"
#include "Components.h"

void Grid::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	
	ModelComponent* pModelComp = new ModelComponent(L"Resources/Meshes/Tile.ovm");
	AddComponent(pModelComp);
	//pModelComp->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
	if(m_IsDark)
		pModelComp->SetMaterial(1);
	else
		pModelComp->SetMaterial(2);
	
	physx::PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();
	float size = 2.0f;

	RigidBodyComponent* pRigidBody = new RigidBodyComponent(true);
	AddComponent(pRigidBody);

	std::shared_ptr<physx::PxGeometry> geom = std::make_shared<physx::PxBoxGeometry>(size * 0.5f, size * 0.5f, size * 0.5f);
	auto pDefaultMaterial = physX->createMaterial(0.0f, 0.0f, 1.0f);
	auto collider = new ColliderComponent(geom, *pDefaultMaterial);
	AddComponent(collider);

	pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group5);
	pRigidBody->SetCollisionIgnoreGroups(CollisionGroupFlag::Group2);

	SetTag(L"Grid");
}

void Grid::setIsDark(bool isDark)
{
	m_IsDark = isDark;
}