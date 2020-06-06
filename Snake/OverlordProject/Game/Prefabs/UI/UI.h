#pragma once
#include "GameObject.h"
#include "PauseMenu.h"
#include "SpriteFont.h"
#include "GameOverMenu.h"

class Snake;

class UI : public GameObject
{
public:
	UI(){};
	virtual ~UI() = default;

	bool ShouldReset()
	{
		if (m_pGameOverMenu)
			return m_pGameOverMenu->ShouldReset();
		if (m_pPauseMenu)
			return m_pPauseMenu->ShouldReset();
		return false;
	}

	void SetPoints(int points)
	{
		m_Points = points;
	}

	void SetGameTime(float time)
	{
		m_GameTime = time;
	}

	bool Won()
	{
		return m_Won;
	}

	void SetWon(bool won)
	{
		m_Won = won;
	}

	UI(const UI& other) = delete;
	UI(UI&& other) noexcept = delete;
	UI& operator=(const UI& other) = delete;
	UI& operator=(UI&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override { UNREFERENCED_PARAMETER(gameContext); };
	void Update(const GameContext& gameContext, Snake* snake);
	void Draw(const GameContext& gameContext) override;

private:
	GameObject* m_pPauseButton = nullptr;
	PauseMenu* m_pPauseMenu = nullptr;
	GameOverMenu* m_pGameOverMenu = nullptr;
	SpriteFont* m_pFont = nullptr;
	bool m_PauseGame = false;
	bool m_Won = false;
	float m_GameTime = 0.0f;
	int m_Points = 0;

	FMOD::Sound* m_pClickSound = nullptr;
	FMOD::Channel* m_pChannel = nullptr;
};
