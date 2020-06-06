#include "stdafx.h"
#include "MainMenuScene.h"
#include "Components.h"
#include "GameObject.h"
#include "OverlordGame.h"
#include "SpriteFont.h"
#include "ContentManager.h"
#include "TextRenderer.h"
#include "SceneManager.h"
#include "SoundManager.h"

MainMenuScene::MainMenuScene() :
	GameScene(L"MainMenuScene")
{}

void MainMenuScene::Initialize()
{
	DirectX::XMFLOAT2 windowSize{ float(OverlordGame::GetGameSettings().Window.Width), float(OverlordGame::GetGameSettings().Window.Height) };

	m_pBackground = new GameObject();
	m_pBackground->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Background.png"));
	m_pBackground->GetTransform()->Translate(0, 0, 0.99f);
	AddChild(m_pBackground);

	auto pBanner = new GameObject();
	pBanner->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Banner.png"));
	pBanner->GetTransform()->Translate(0, windowSize.y-48, 0.98f);
	AddChild(pBanner);

	m_pPlayButton = new GameObject();
	m_pPlayButton->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Play_Button.png"));
	m_pPlayButton->GetTransform()->Translate((windowSize.x - 399 )*0.5f, 250, 0.98f);
	AddChild(m_pPlayButton);

	m_pInfoButton = new GameObject();
	m_pInfoButton->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Info_Button.png"));
	m_pInfoButton->GetTransform()->Translate((windowSize.x - 399) * 0.5f, 350, 0.98f);
	AddChild(m_pInfoButton);

	m_pQuitButton = new GameObject();
	m_pQuitButton->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Quit_Button.png"));
	m_pQuitButton->GetTransform()->Translate((windowSize.x - 399) * 0.5f, 450, 0.98f);
	AddChild(m_pQuitButton);

	//m_pFontButton = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/ChickenPie_72.fnt");
	m_pFontTitle = ContentManager::Load<SpriteFont>(L"./Resources/SpriteFonts/ChickenPie_120.fnt");

	// SOUND
	SoundManager::GetInstance()->GetSystem()->createSound("./Resources/Sounds/Click.wav", FMOD_2D, 0, &m_pClickSound);
	m_pClickSound->setMode(FMOD_LOOP_OFF);

	SoundManager::GetInstance()->GetSystem()->createSound("./Resources/Sounds/MainMenu.mp3", FMOD_2D, 0, &m_pMusic);
	m_pMusic->setMode(FMOD_LOOP_NORMAL);
	SoundManager::GetInstance()->GetSystem()->playSound(m_pMusic, 0, false, &m_pChannel2);
}

void MainMenuScene::Update()
{

	m_AccumulatedSec += GetGameContext().pGameTime->GetElapsed();
	if (m_AccumulatedSec < 0.5f) return;
	
	float xMousePos = float(GetGameContext().pInput->GetMousePosition().x);
	float yMousePos = float(GetGameContext().pInput->GetMousePosition().y);
	DirectX::XMFLOAT2 windowSize{ float(OverlordGame::GetGameSettings().Window.Width), float(OverlordGame::GetGameSettings().Window.Height) };

	DirectX::XMFLOAT2 pos = { m_pPlayButton->GetTransform()->GetPosition().x , m_pPlayButton->GetTransform()->GetPosition().y };
	if (xMousePos >= pos.x && xMousePos <= pos.x + 399 && yMousePos >= pos.y && yMousePos <= pos.y + 78)
	{
		m_pPlayButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Play_Button_Selected.png");
		if (GetGameContext().pInput->IsMouseButtonDown(VK_LBUTTON))
		{
			m_pChannel2->setPaused(true);
			SoundManager::GetInstance()->GetSystem()->playSound(m_pClickSound, 0, false, &m_pChannel);
			SceneManager::GetInstance()->SetActiveGameScene(L"SnakeGameScene");
		}
	}
	else m_pPlayButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Play_Button.png");

	pos = { m_pInfoButton->GetTransform()->GetPosition().x , m_pInfoButton->GetTransform()->GetPosition().y };
	if (xMousePos >= pos.x && xMousePos <= pos.x + 399 && yMousePos >= pos.y && yMousePos <= pos.y + 78)
	{
		m_pInfoButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Info_Button_Selected.png");
		if (GetGameContext().pInput->IsMouseButtonDown(VK_LBUTTON))
		{
			//m_pChannel->setPaused(false);
			SoundManager::GetInstance()->GetSystem()->playSound(m_pClickSound, 0, false, &m_pChannel);
			SceneManager::GetInstance()->SetActiveGameScene(L"InfoScene");
		}
	}
	else m_pInfoButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Info_Button.png");

	pos = { m_pQuitButton->GetTransform()->GetPosition().x , m_pQuitButton->GetTransform()->GetPosition().y };
	if (xMousePos >= pos.x && xMousePos <= pos.x + 399 && yMousePos >= pos.y && yMousePos <= pos.y + 78)
	{
		m_pQuitButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Quit_Button_Selected.png");
		if (GetGameContext().pInput->IsMouseButtonDown(VK_LBUTTON))
		{
			SoundManager::GetInstance()->GetSystem()->playSound(m_pClickSound, 0, false, &m_pChannel);
			PostQuitMessage(0);
		}
	}
	else m_pQuitButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/Quit_Button.png");
}

void MainMenuScene::Draw()
{
	TextRenderer::GetInstance()->DrawText(m_pFontTitle, L"SNAKE", DirectX::XMFLOAT2(500, 50), DirectX::XMFLOAT4(0, 0, 0, 1));
}

void MainMenuScene::SceneActivated()
{
	m_pChannel2->setPaused(false);
}

void MainMenuScene::SceneDeactivated()
{
	m_AccumulatedSec = 0;
}
