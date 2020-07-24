#include "stdafx.h"
#include "PauseMenu.h"
#include "Components.h"
#include "OverlordGame.h"
#include "../Snake/Snake.h"
#include "SceneManager.h"
#include "ContentManager.h"
#include "TextRenderer.h"
#include "SoundManager.h"

void PauseMenu::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	DirectX::XMFLOAT2 windowSize{ float(OverlordGame::GetGameSettings().Window.Width), float(OverlordGame::GetGameSettings().Window.Height) };
	
	GameObject* pBackground = new GameObject();
	pBackground->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Background_Pause.png"));
	pBackground->GetTransform()->Translate(150, 120, 0.99f);
	AddChild(pBackground);

	m_pCrossButton = new GameObject();
	m_pCrossButton->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Cross_Button.png"));
	m_pCrossButton->GetTransform()->Translate(windowSize.x - 220, 140, 0.98f);
	AddChild(m_pCrossButton);

	m_pMainMenuButton = new GameObject();
	m_pMainMenuButton->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/MainMenu_Button.png"));
	m_pMainMenuButton->GetTransform()->Translate((windowSize.x - 405) * 0.5f, 190, 0.98f);
	AddChild(m_pMainMenuButton);

	m_pRestartButton = new GameObject();
	m_pRestartButton->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Restart_Button.png"));
	m_pRestartButton->GetTransform()->Translate((windowSize.x - 405) * 0.5f, 320, 0.98f);
	AddChild(m_pRestartButton);

	m_pQuitButton = new GameObject();
	m_pQuitButton->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Quit_Button.png"));
	m_pQuitButton->GetTransform()->Translate((windowSize.x - 405)*0.5f, 450, 0.98f);
	AddChild(m_pQuitButton);

	// SOUND
	SoundManager::GetInstance()->GetSystem()->createSound("./Resources/Sounds/Click.wav", FMOD_2D, 0, &m_pClickSound);
	m_pClickSound->setMode(FMOD_LOOP_OFF);
}

void PauseMenu::Update(const GameContext& gameContext, Snake* snake)
{
	float xMousePos = float(gameContext.pInput->GetMousePosition().x);
	float yMousePos = float(gameContext.pInput->GetMousePosition().y);
	DirectX::XMFLOAT2 windowSize{ float(OverlordGame::GetGameSettings().Window.Width), float(OverlordGame::GetGameSettings().Window.Height) };

	// CROSS
	DirectX::XMFLOAT3 pos = m_pCrossButton->GetTransform()->GetPosition();
	if (xMousePos >= pos.x && xMousePos <= pos.x + 60 && yMousePos >= pos.y && yMousePos <= pos.y + 60)
	{
		m_pCrossButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Cross_Button_Selected.png");
		if (gameContext.pInput->IsMouseButtonDown(VK_LBUTTON))
		{
			SoundManager::GetInstance()->GetSystem()->playSound(m_pClickSound, 0, false, &m_pChannel);
			snake->SetPause(false);
			m_Close = true;
		}
	}
	else m_pCrossButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Cross_Button.png");


	// MAIN MENU
	pos = m_pMainMenuButton->GetTransform()->GetPosition();
	if (xMousePos >= pos.x && xMousePos <= pos.x + 405 && yMousePos >= pos.y && yMousePos <= pos.y + 85)
	{
		m_pMainMenuButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/MainMenu_Button_Selected.png");
		if (gameContext.pInput->IsMouseButtonDown(VK_LBUTTON))
		{
			SoundManager::GetInstance()->GetSystem()->playSound(m_pClickSound, 0, false, &m_pChannel);
			SceneManager::GetInstance()->SetActiveGameScene(L"MainMenuScene");
		}
	}
	else m_pMainMenuButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/MainMenu_Button.png");

	
	// RESTART
	pos = m_pRestartButton->GetTransform()->GetPosition();
	if (xMousePos >= pos.x && xMousePos <= pos.x + 405 && yMousePos >= pos.y && yMousePos <= pos.y + 85)
	{
		m_pRestartButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Restart_Button_Selected.png");
		if (gameContext.pInput->IsMouseButtonDown(VK_LBUTTON))
		{
			SoundManager::GetInstance()->GetSystem()->playSound(m_pClickSound, 0, false, &m_pChannel);
			m_Reset = true;
		}
	}
	else m_pRestartButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Restart_Button.png");
	
	
	// QUIT
	pos = m_pQuitButton->GetTransform()->GetPosition();
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

