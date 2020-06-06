#include "stdafx.h"
#include "UI.h"
#include "Components.h"
#include "OverlordGame.h"
#include "../Snake/Snake.h"
#include "ContentManager.h"
#include <iomanip>
#include "TextRenderer.h"
#include "GameOverMenu.h"
#include "SoundManager.h"

void UI::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	DirectX::XMFLOAT2 windowSize{ float(OverlordGame::GetGameSettings().Window.Width), float(OverlordGame::GetGameSettings().Window.Height) };

	m_pPauseButton = new GameObject();
	m_pPauseButton->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Pause_Button.png"));
	m_pPauseButton->GetTransform()->Translate(windowSize.x -70 , 10, 0.99f);
	AddChild(m_pPauseButton);
	m_pPauseButton->SetFinalDraw(true);

	m_pFont = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/ChickenPie_256.fnt");

	SetFinalDraw(true);

	// SOUND
	SoundManager::GetInstance()->GetSystem()->createSound("./Resources/Sounds/Click.wav", FMOD_2D, 0, &m_pClickSound);
	m_pClickSound->setMode(FMOD_LOOP_OFF);
}

void UI::Update(const GameContext& gameContext, Snake* snake)
{
	
	if (m_PauseGame)
	{
		m_pPauseMenu->Update(gameContext, snake);
		if (m_pPauseMenu->GetClose())
		{
			RemoveChild(m_pPauseMenu);
			m_pPauseMenu->SetActive(false);
			delete(m_pPauseMenu);
			m_PauseGame = false;
		}
	}
	
	float xMousePos = float(gameContext.pInput->GetMousePosition().x);
	float yMousePos = float(gameContext.pInput->GetMousePosition().y);
	DirectX::XMFLOAT2 windowSize{ float(OverlordGame::GetGameSettings().Window.Width), float(OverlordGame::GetGameSettings().Window.Height) };

	if (xMousePos >= windowSize.x - 70 && xMousePos <= windowSize.x - 10 && yMousePos >= 10 && yMousePos <= 70)
	{
		m_pPauseButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Pause_Button_Selected.png");
		if (gameContext.pInput->IsMouseButtonDown(VK_LBUTTON) && !m_PauseGame)
		{
			SoundManager::GetInstance()->GetSystem()->playSound(m_pClickSound, 0, false, &m_pChannel);
			
			if (!m_pGameOverMenu)
			{
				snake->SetPause(true);
				m_pPauseMenu = new PauseMenu();
				AddChild(m_pPauseMenu);
				m_PauseGame = true;
				m_pPauseMenu->SetFinalDraw(true);
			}
		}
	}
	else m_pPauseButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Pause_Button.png");

	if(snake->GetChild<SnakeHead>()->HitObject() || m_Won)
	{
		if (!m_pGameOverMenu)
		{
			m_pGameOverMenu = new GameOverMenu(m_Points, m_GameTime, m_Won);
			AddChild(m_pGameOverMenu);
			m_pGameOverMenu->SetFinalDraw(true);
		}
	}
}

void UI::Draw(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	DirectX::XMFLOAT2 windowSize{ float(OverlordGame::GetGameSettings().Window.Width), float(OverlordGame::GetGameSettings().Window.Height) };
	
	std::wstringstream ss;
	ss << std::floor(m_GameTime);
	TextRenderer::GetInstance()->DrawText(m_pFont, ss.str(), DirectX::XMFLOAT2(windowSize.x*0.5f, 0), DirectX::XMFLOAT4(1,1,1,1));

	std::wstringstream ss2;
	ss2 << "Score: " << std::setfill(L'0') << std::setw(4) << m_Points;
	TextRenderer::GetInstance()->DrawText(m_pFont, ss2.str(), DirectX::XMFLOAT2(90, 0), DirectX::XMFLOAT4(1, 1, 1, 1));
}

