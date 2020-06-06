#pragma once
#include "GameScene.h"
#include "SpriteFont.h"

class MainMenuScene final : public GameScene
{
public:
	MainMenuScene();
	virtual ~MainMenuScene() = default;

	MainMenuScene(const MainMenuScene& other) = delete;
	MainMenuScene(MainMenuScene&& other) noexcept = delete;
	MainMenuScene& operator=(const MainMenuScene& other) = delete;
	MainMenuScene& operator=(MainMenuScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

	void SceneActivated() override;
	void SceneDeactivated() override;

private:
	GameObject* m_pBackground = nullptr;
	GameObject* m_pPlayButton = nullptr;
	GameObject* m_pInfoButton = nullptr;
	GameObject* m_pQuitButton = nullptr;

	SpriteFont* m_pFontTitle = nullptr;

	float m_AccumulatedSec = 0;

	FMOD::Sound* m_pClickSound = nullptr;
	FMOD::Channel* m_pChannel = nullptr;
	FMOD::Sound* m_pMusic = nullptr;
	FMOD::Channel* m_pChannel2 = nullptr;
};


