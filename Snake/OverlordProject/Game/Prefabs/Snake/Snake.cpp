#include "stdafx.h"
#include "Snake.h"
#include "Components.h"
#include "ContentManager.h"

Snake::Snake()
{

}

void Snake::Initialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	m_pHead = new SnakeHead();
	AddChild(m_pHead);

	for (int i{}; i < 86; ++i)
	{
		SnakeBody* pBody = nullptr;
		if (i == 0)
		{
			pBody = new SnakeBody(false, true);
			m_SnakeSize++;
		}
		else pBody = new SnakeBody(true, false);

		AddChild(pBody);
		m_pBodies.push_back(pBody);
	}
}

void Snake::PostInitialize(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

}

void Snake::Update(const GameContext& gameContext)
{
	if (m_pHead->HitObject())
	{
		m_pHead->SetPause(true);
		for (int i{}; i < m_SnakeSize; ++i)
		{
			m_pBodies[i]->SetActive(false);
		}

	}
	if (!m_Pause)
	{
		if (!m_pHead->HitObject())
		{
			if(m_pBodies[0]->CanGetTail())
			{
				Grow();
				m_pBodies[0]->SetCanGetTail(false);
			}
			
			for (int i{}; i < m_SnakeSize; ++i)
			{
				if (i == 0)
				{
					m_pBodies[i]->AddPosition(m_pHead->GetTransform()->GetPosition());
					m_pBodies[i]->Update(gameContext, m_pHead->GetTransform()->GetPosition());
				}
				else
				{
					m_pBodies[i]->AddPosition(m_pBodies[i - 1]->GetTransform()->GetPosition());
					m_pBodies[i]->Update(gameContext, m_pBodies[i - 1]->GetTransform()->GetPosition());
				}

			}
		}
		if(m_pHead->GetPickedup())
		{
			Grow();
			m_pHead->setPickedUp(false);
			m_PickedUp = true;
		}
	}
}


void Snake::Grow()
{
	if (m_SnakeSize < int(m_pBodies.size()))
	{
		if (m_pBodies[m_SnakeSize - 1]->CanGetTail())
		{
			m_pBodies[m_SnakeSize]->Activate();
			m_pBodies[m_SnakeSize]->GetTransform()->Translate(m_pBodies[m_SnakeSize - 1]->GetTransform()->GetPosition());
			++m_SnakeSize;
			m_pHead->SetSnakeSize(m_SnakeSize + 1);
		}
	}
}

void Snake::SetPause(bool pause)
{
	m_Pause = pause;
	m_pHead->SetPause(pause);
}

bool Snake::IsPaused()
{
	return m_Pause;
}

