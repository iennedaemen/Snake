#pragma once

class Score
{
public:
	Score() {};
	virtual ~Score() = default;

	void SetGameTime(float time)
	{
		m_GameTime = time;
	}
	float GetGameTime()
	{
		return m_GameTime;
	}

	void SetScore(int score)
	{
		m_Points = score;
	}
	int GetScore()
	{
		return m_Points;
	}
	
	Score(const Score& other) = delete;
	Score(Score&& other) noexcept = delete;
	Score& operator=(const Score& other) = delete;
	Score& operator=(Score&& other) noexcept = delete;


private:
	float m_GameTime = 0.0f;
	int m_Points = 0;;
};
