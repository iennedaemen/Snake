#pragma once
#include "GameObject.h"
#include "SpriteFont.h"

class Snake;

class PauseMenu : public GameObject
{
public:
	PauseMenu() {};
	virtual ~PauseMenu() = default;

	bool GetClose()
	{
		return m_Close;
	}

	bool ShouldReset()
	{
		if (m_Close) return false;
		return m_Reset;
	}
	
	PauseMenu(const PauseMenu& other) = delete;
	PauseMenu(PauseMenu&& other) noexcept = delete;
	PauseMenu& operator=(const PauseMenu& other) = delete;
	PauseMenu& operator=(PauseMenu&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override { UNREFERENCED_PARAMETER(gameContext); };
	void Update(const GameContext& gameContext, Snake* snake);

private:
	GameObject* m_pCrossButton = nullptr;
	GameObject* m_pMainMenuButton = nullptr;
	GameObject* m_pRestartButton = nullptr;
	GameObject* m_pQuitButton = nullptr;
	SpriteFont* m_pFontButton = nullptr;
	bool m_Close = false;
	bool m_Reset = false;

	FMOD::Sound* m_pClickSound = nullptr;
	FMOD::Channel* m_pChannel = nullptr;
};
