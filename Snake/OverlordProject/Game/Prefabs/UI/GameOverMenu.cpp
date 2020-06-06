#include "stdafx.h"
#include "GameOverMenu.h"
#include "Components.h"
#include "OverlordGame.h"
#include "../Snake/Snake.h"
#include "SceneManager.h"
#include "ContentManager.h"
#include "TextRenderer.h"
#include "SoundManager.h"

GameOverMenu::GameOverMenu(int score, float gameTime, bool won)
	: m_Score(score)
	, m_GameTime(gameTime)
	, m_Won(won)
{
	
}


void GameOverMenu::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	DirectX::XMFLOAT2 windowSize{ float(OverlordGame::GetGameSettings().Window.Width), float(OverlordGame::GetGameSettings().Window.Height) };

	GameObject* pBackground = new GameObject();
	if(m_Won)
		pBackground->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Background_Win.png"));
	else
		pBackground->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Background_GameOver.png"));
	pBackground->GetTransform()->Translate(150, 120, 0.9f);
	AddChild(pBackground);

	m_pMainMenuButton = new GameObject();
	m_pMainMenuButton->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/MainMenu_Button.png"));
	m_pMainMenuButton->GetTransform()->Translate((windowSize.x - 405) * 0.5f, 300, 0.96f);
	AddChild(m_pMainMenuButton);

	m_pRestartButton = new GameObject();
	m_pRestartButton->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Restart_Button.png"));
	m_pRestartButton->GetTransform()->Translate((windowSize.x - 405) * 0.5f, 400, 0.96f);
	AddChild(m_pRestartButton);

	m_pQuitButton = new GameObject();
	m_pQuitButton->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Quit_Button.png"));
	m_pQuitButton->GetTransform()->Translate((windowSize.x - 405) * 0.5f, 500, 0.96f);
	AddChild(m_pQuitButton);

	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/ChickenPie_72.fnt");

	// SOUND
	SoundManager::GetInstance()->GetSystem()->createSound("./Resources/Sounds/Click.wav", FMOD_2D, 0, &m_pClickSound);
	m_pClickSound->setMode(FMOD_LOOP_OFF);

	SoundManager::GetInstance()->GetSystem()->createSound("./Resources/Sounds/GameOver.mp3", FMOD_2D, 0, &m_pGameOverMusic);
	m_pGameOverMusic->setMode(FMOD_LOOP_NORMAL);
	SoundManager::GetInstance()->GetSystem()->playSound(m_pGameOverMusic, 0, false, &m_pChannel2);
}

void GameOverMenu::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	float xMousePos = float(gameContext.pInput->GetMousePosition().x);
	float yMousePos = float(gameContext.pInput->GetMousePosition().y);
	
	// MAIN MENU
	DirectX::XMFLOAT2 pos = DirectX::XMFLOAT2(m_pMainMenuButton->GetTransform()->GetPosition().x, m_pMainMenuButton->GetTransform()->GetPosition().y);
	if (xMousePos >= pos.x && xMousePos <= pos.x + 405 && yMousePos >= pos.y && yMousePos <= pos.y + 85)
	{
		m_pMainMenuButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/MainMenu_Button_Selected.png");
		if (gameContext.pInput->IsMouseButtonDown(VK_LBUTTON))
		{
			SoundManager::GetInstance()->GetSystem()->playSound(m_pClickSound, 0, false, &m_pChannel);
			m_pChannel2->setPaused(true);
			SceneManager::GetInstance()->SetActiveGameScene(L"MainMenuScene");
		}
	}
	else m_pMainMenuButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/MainMenu_Button.png");


	// RESTART
	pos = DirectX::XMFLOAT2(m_pRestartButton->GetTransform()->GetPosition().x, m_pRestartButton->GetTransform()->GetPosition().y);
	if (xMousePos >= pos.x && xMousePos <= pos.x + 405 && yMousePos >= pos.y && yMousePos <= pos.y + 85)
	{
		m_pRestartButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Restart_Button_Selected.png");
		if (gameContext.pInput->IsMouseButtonDown(VK_LBUTTON))
		{
			SoundManager::GetInstance()->GetSystem()->playSound(m_pClickSound, 0, false, &m_pChannel);
			m_pChannel2->setPaused(true);
			m_Reset = true;
		}
	}
	else m_pRestartButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Restart_Button.png");


	// QUIT
	pos = DirectX::XMFLOAT2(m_pQuitButton->GetTransform()->GetPosition().x, m_pQuitButton->GetTransform()->GetPosition().y);
	if (xMousePos >= pos.x && xMousePos <= pos.x + 405 && yMousePos >= pos.y && yMousePos <= pos.y + 85)
	{
		m_pQuitButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Quit_Button_Selected.png");
		if (gameContext.pInput->IsMouseButtonDown(VK_LBUTTON))
		{
			SoundManager::GetInstance()->GetSystem()->playSound(m_pClickSound, 0, false, &m_pChannel);
			PostQuitMessage(0);
		}
	}
	else m_pQuitButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Quit_Button.png");
}

void GameOverMenu::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	
	DirectX::XMFLOAT2 windowSize{ float(OverlordGame::GetGameSettings().Window.Width), float(OverlordGame::GetGameSettings().Window.Height) };

	std::wstringstream ss;
	ss << "Time: " << std::floor(m_GameTime) << " sec";
	TextRenderer::GetInstance()->DrawText(m_pFont, ss.str(), DirectX::XMFLOAT2(windowSize.x * 0.5f + 80, 200), DirectX::XMFLOAT4(0, 0, 0, 1));

	std::wstringstream ss2;
	ss2 << "Score: " << std::floor(m_Score);
	TextRenderer::GetInstance()->DrawText(m_pFont, ss2.str(), DirectX::XMFLOAT2(windowSize.x * 0.5f - 350, 200), DirectX::XMFLOAT4(0, 0, 0, 1));
}
