#include "stdafx.h"
#include "InfoScene.h"
#include "OverlordGame.h"
#include "Components.h"
#include "GameObject.h"
#include "ContentManager.h"
#include "SceneManager.h"
#include "SoundManager.h"

InfoScene::InfoScene() :
	GameScene(L"InfoScene")
{}

void InfoScene::Initialize()
{
	DirectX::XMFLOAT2 windowSize{ float(OverlordGame::GetGameSettings().Window.Width), float(OverlordGame::GetGameSettings().Window.Height) };

	GameObject* pBackground = new GameObject();
	pBackground->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Background_Info.png"));
	pBackground->GetTransform()->Translate(0, 0, 0.99f);
	AddChild(pBackground);

	auto pBanner = new GameObject();
	pBanner->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/Banner.png"));
	pBanner->GetTransform()->Translate(0, windowSize.y - 48, 0.98f);
	AddChild(pBanner);

	m_pOkButton = new GameObject();
	m_pOkButton->AddComponent(new SpriteComponent(L"./Resources/Textures/Menu/GotIt_Button.png"));
	m_pOkButton->GetTransform()->Translate((windowSize.x - 399) * 0.5f, 450, 0.98f);
	AddChild(m_pOkButton);

	SoundManager::GetInstance()->GetSystem()->createSound("./Resources/Sounds/Click.wav", FMOD_2D, 0, &m_pClickSound);
	m_pClickSound->setMode(FMOD_LOOP_OFF);
}

void InfoScene::Update()
{
	float xMousePos = float(GetGameContext().pInput->GetMousePosition().x);
	float yMousePos = float(GetGameContext().pInput->GetMousePosition().y);
	DirectX::XMFLOAT2 windowSize{ float(OverlordGame::GetGameSettings().Window.Width), float(OverlordGame::GetGameSettings().Window.Height) };

	DirectX::XMFLOAT2 pos = { m_pOkButton->GetTransform()->GetPosition().x , m_pOkButton->GetTransform()->GetPosition().y };
	if (xMousePos >= pos.x && xMousePos <= pos.x + 399 && yMousePos >= pos.y && yMousePos <= pos.y + 78)
	{
		m_pOkButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/GotIt_Button_Selected.png");
		if (GetGameContext().pInput->IsMouseButtonDown(VK_LBUTTON))
		{
			SoundManager::GetInstance()->GetSystem()->playSound(m_pClickSound, 0, false, &m_pChannel);
			SceneManager::GetInstance()->SetActiveGameScene(L"MainMenuScene");
		}
	}
	else m_pOkButton->GetComponent<SpriteComponent>()->SetTexture(L"./Resources/Textures/Menu/GotIt_Button.png");

}

void InfoScene::Draw()
{
	
}
