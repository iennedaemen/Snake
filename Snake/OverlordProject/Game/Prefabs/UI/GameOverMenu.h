#pragma once
#include "GameObject.h"
#include "SpriteFont.h"

class GameOverMenu : public GameObject
{
public:
	GameOverMenu(int score, float gameTime, bool won);
	virtual ~GameOverMenu() = default;

	bool ShouldReset()
	{
		return m_Reset;
	}
	
	GameOverMenu(const GameOverMenu& other) = delete;
	GameOverMenu(GameOverMenu&& other) noexcept = delete;
	GameOverMenu& operator=(const GameOverMenu& other) = delete;
	GameOverMenu& operator=(GameOverMenu&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override { UNREFERENCED_PARAMETER(gameContext); };
	void Update(const GameContext& gameContext);
	void Draw(const GameContext& gameContext) override;

private:
	GameObject* m_pMainMenuButton = nullptr;
	GameObject* m_pRestartButton = nullptr;
	GameObject* m_pQuitButton = nullptr;
	SpriteFont* m_pFont = nullptr;
	bool m_Reset = false;
	bool m_Won;
	int m_Score;
	float m_GameTime;

	FMOD::Sound* m_pClickSound = nullptr;
	FMOD::Channel* m_pChannel = nullptr;

	FMOD::Sound* m_pGameOverMusic = nullptr;
	FMOD::Channel* m_pChannel2 = nullptr;
};
