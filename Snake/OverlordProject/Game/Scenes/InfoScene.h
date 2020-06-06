#pragma once
#include "GameScene.h"
#include "SpriteFont.h"

class InfoScene final : public GameScene
{
public:
	InfoScene();
	virtual ~InfoScene() = default;

	InfoScene(const InfoScene& other) = delete;
	InfoScene(InfoScene&& other) noexcept = delete;
	InfoScene& operator=(const InfoScene& other) = delete;
	InfoScene& operator=(InfoScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	GameObject* m_pOkButton = nullptr;

	SpriteFont* m_pFontButton = nullptr;
	SpriteFont* m_pFontText = nullptr;

	FMOD::Sound* m_pClickSound = nullptr;
	FMOD::Channel* m_pChannel = nullptr;
};