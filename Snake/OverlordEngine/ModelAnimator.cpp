#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
m_pMeshFilter(pMeshFilter),
m_Transforms(std::vector<DirectX::XMFLOAT4X4>()),
m_IsPlaying(false), 
m_Reversed(false),
m_ClipSet(false),
m_TickCount(0),
m_AnimationSpeed(1.0f)
{
	SetAnimation(0);
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	m_ClipSet = false;

	if(clipNumber > m_pMeshFilter->m_AnimationClips.size())
	{
		Reset(true);
		Logger::LogWarning(L"clipNumber is higher than the amount of existing clips -> SetAnimation(UINT clipNumber), ModelAnimator.cpp");
		return;
	}
	else SetAnimation(m_pMeshFilter->m_AnimationClips[clipNumber]);
}

void ModelAnimator::SetAnimation(std::wstring clipName)
{
	m_ClipSet = false;
	// Search for an AnimationClip with the given name
	auto it = std::find_if(m_pMeshFilter->m_AnimationClips.begin(), m_pMeshFilter->m_AnimationClips.end(), [clipName](AnimationClip clip)
	{
		return clip.Name == clipName;
	});

	if (it != m_pMeshFilter->m_AnimationClips.end())
	{
		SetAnimation(*it);
	}
	else
	{
		Reset(true);
		Logger::LogWarning(L"clip with given name does not exist -> SetAnimation(std::wstring clipName), ModelAnimator.cpp");
	}
}

void ModelAnimator::SetAnimation(AnimationClip clip)
{
	m_ClipSet = true;
	m_CurrentClip = clip;
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	if (pause)
		m_IsPlaying = false;

	m_TickCount = 0;
	m_AnimationSpeed = 1.0f;
	
	if(m_ClipSet)
	{
		//	Retrieve the BoneTransform from the first Key from the current clip
		std::vector<DirectX::XMFLOAT4X4> boneTransforms = m_CurrentClip.Keys.front().BoneTransforms;
		//	Refill the m_Transforms vector with the new BoneTransforms
		m_Transforms.assign(boneTransforms.begin(), boneTransforms.end());
	}
	else
	{
		//	Create an IdentityMatrix
		DirectX::XMFLOAT4X4 identityMatrix;
		DirectX::XMStoreFloat4x4(&identityMatrix, DirectX::XMMatrixIdentity());
		//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount)
		m_Transforms.assign(m_CurrentClip.Keys.front().BoneTransforms.size(),identityMatrix);
	}
}

void ModelAnimator::Update(const GameContext& gameContext)
{

	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//Calculate the passedTicks
		float elapsedSec = gameContext.pGameTime->GetElapsed();
		float passedTicks = elapsedSec * m_CurrentClip.TicksPerSecond * m_AnimationSpeed;
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds
		passedTicks = fmod(passedTicks, m_CurrentClip.Duration);

		
		if (m_Reversed)
		{
			//	Subtract
			m_TickCount -= passedTicks;
			//	If m_TickCount is smaller than zero, "reset"
			if (m_TickCount < 0)
				m_TickCount += m_CurrentClip.Duration;
		}
		//ELSE
		else
		{
			//	Add 
			m_TickCount += passedTicks;
			//	if m_TickCount is bigger than the clip duration, "reset"
			if (m_TickCount > m_CurrentClip.Duration)
				m_TickCount -= m_CurrentClip.Duration;
		}

	
		//Find the enclosing keys
		AnimationKey keyA, keyB;
		for (size_t i{}; i < m_CurrentClip.Keys.size(); ++i)
		{
			if (m_CurrentClip.Keys[i].Tick < m_TickCount)
			{
				keyA = m_CurrentClip.Keys[i];
				keyB = m_CurrentClip.Keys[(i + 1) % m_CurrentClip.Keys.size()];
			}
		}

		//Interpolate between keys
		float keyLength = m_CurrentClip.Duration / m_CurrentClip.Keys.size();
		float blendFactor = (keyB.Tick - m_TickCount) / keyLength;
		if (blendFactor < 0) 
			blendFactor = m_TickCount;
		//Clear the m_Transforms vector
		m_Transforms.clear();
		
		// For every bone
		for (size_t i{}; i < m_CurrentClip.Keys.front().BoneTransforms.size(); ++i)
		{
			//	Retrieve the transform from keyA 
			auto transformA = keyA.BoneTransforms[i];
			// 	Retrieve the transform from keyB
			auto transformB = keyB.BoneTransforms[i];
			//	Decompose both transforms
			DirectX::XMVECTOR scaleA;
			DirectX::XMVECTOR translationA;
			DirectX::XMVECTOR quatRotationA;
			DirectX::XMMATRIX matrixA = DirectX::XMLoadFloat4x4(&transformA);
			if (!DirectX::XMMatrixDecompose(&scaleA, &quatRotationA, &translationA, matrixA))
				Logger::LogWarning(L"Matrix of keyA not decomposed -> :Update(const GameContext& gameContext), ModelAnimator.cpp");

			DirectX::XMVECTOR scaleB;
			DirectX::XMVECTOR translationB;
			DirectX::XMVECTOR quatRotationB;
			DirectX::XMMATRIX matrixB = DirectX::XMLoadFloat4x4(&transformB);
			if (!DirectX::XMMatrixDecompose(&scaleB, &quatRotationB, &translationB, matrixB))
				Logger::LogWarning(L"Matrix of keyB not decomposed -> :Update(const GameContext& gameContext), ModelAnimator.cpp");

			//	Lerp between all the transformations (Position, Scale, Rotation)
			DirectX::XMVECTOR translation, rotation, scale;
			translation = DirectX::XMVectorLerp(translationB, translationA, blendFactor);
			scale = DirectX::XMVectorLerp(scaleB, scaleA, blendFactor);
			rotation = DirectX::XMQuaternionSlerp(quatRotationB, quatRotationA, blendFactor);

			//	Compose a transformation matrix with the lerp-results
			DirectX::XMMATRIX transformation = 
				DirectX::XMMatrixScalingFromVector(scale) * 
				DirectX::XMMatrixRotationQuaternion(rotation) *
				DirectX::XMMatrixTranslationFromVector(translation);
			
			//	Add the resulting matrix to the m_Transforms vector
			DirectX::XMFLOAT4X4 finalTransformation;
			DirectX::XMStoreFloat4x4(&finalTransformation, transformation);
			m_Transforms.push_back(finalTransformation);
		}
	}
}
