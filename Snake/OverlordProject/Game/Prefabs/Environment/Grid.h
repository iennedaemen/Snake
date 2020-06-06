#pragma once
#include "GameObject.h"

class Grid : public GameObject
{
public:
	Grid(){};
	virtual ~Grid() = default;

	void setIsDark(bool isDark);
	void SetIsFree(bool isFree)
	{
		m_IsFree = isFree;
	}

	bool GetIsFree()
	{
		return m_IsFree;
	}

	Grid(const Grid& other) = delete;
	Grid(Grid&& other) noexcept = delete;
	Grid& operator=(const Grid& other) = delete;
	Grid& operator=(Grid&& other) noexcept = delete;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override { UNREFERENCED_PARAMETER(gameContext); };
	void Update(const GameContext& gameContext) override{ UNREFERENCED_PARAMETER(gameContext); };

private:
	bool m_IsDark = true;
	bool m_IsFree = true;
};