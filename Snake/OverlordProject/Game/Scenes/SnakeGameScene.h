#pragma once
#include "GameScene.h"
#include "../Prefabs/Snake/Snake.h"
#include "../../Materials/GrassMaterial.h"
#include "../Prefabs/Pickups/Pickup.h"
#include "../Prefabs/UI/UI.h"
#include "../Prefabs/Environment/PostOutline.h"

class GameOverScene;

class SnakeGameScene final : public GameScene
{	
public:
	SnakeGameScene();
	virtual ~SnakeGameScene() = default;

	SnakeGameScene(const SnakeGameScene& other) = delete;
	SnakeGameScene(SnakeGameScene&& other) noexcept = delete;
	SnakeGameScene& operator=(const SnakeGameScene& other) = delete;
	SnakeGameScene& operator=(SnakeGameScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	//void FinalDraw() override;

	void SceneActivated() override;
	void SceneDeactivated() override;

private:
	Snake* m_pSnake = nullptr;
	GrassMaterial* m_pMatGrass = nullptr;
	Pickup* m_pPickup = nullptr;
	std::vector<Grid*> m_pGrid;
	UI* m_pUI = nullptr;
	float m_GameTime = 0.0f;
	int m_Points = 0;
	PostOutline* m_pOutline = nullptr;

	FMOD::Sound* m_pGameMusic = nullptr;
	FMOD::Channel* m_pChannel = nullptr;

	void Reset();
};


