#include "stdafx.h"
#include "Grass.h"
#include "Components.h"
#include "CubePrefab.h"
#include "PhysxManager.h"

void Grass::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);	
	ModelComponent* pModelComp = new ModelComponent(L"Resources/Meshes/GrassPlane.ovm");
	AddComponent(pModelComp);
	//pModelComp->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
	pModelComp->SetMaterial(3);

	GameObject* pRocks = new GameObject();
	pModelComp = new ModelComponent(L"Resources/Meshes/Rocks.ovm");
	pRocks->AddComponent(pModelComp);
	pModelComp->GetTransform()->Scale(1.05f, 1.05f, 1.05f);
	pModelComp->SetMaterial(4);
	AddChild(pRocks);


	// LEFT WALL
	CreateWall({ 10,10,300 }, { -200, 10,0 });
	CreateWall({ 10,10,300 }, { 200, 10,0 });
	CreateWall({ 400,10,10 }, { 0, 10,150 });
	CreateWall({ 400,10,10 }, { 0, 10,-150 });
}


void Grass::CreateWall(DirectX::XMFLOAT3 dimension, DirectX::XMFLOAT3 position)
{
	physx::PxPhysics* physX = PhysxManager::GetInstance()->GetPhysics();
	
	auto pWall = new GameObject();
	pWall->GetTransform()->Translate(position);

	auto pRigidBody = new RigidBodyComponent();
	pRigidBody->SetKinematic(true);
	pWall->AddComponent(pRigidBody);

	physx::PxMaterial* pGroundMat = physX->createMaterial(0.5f, 0.5f, 0.0f);
	std::shared_ptr<physx::PxGeometry> pBoxGeometry(new physx::PxBoxGeometry(physx::PxVec3{ dimension.x * 0.5f, dimension.y * 0.5f, dimension.z * 0.5f }));
	auto pCollider = new ColliderComponent(pBoxGeometry, *pGroundMat);
	pWall->AddComponent(pCollider);

	AddChild(pWall);
	pWall->SetTag(L"Wall");
}
