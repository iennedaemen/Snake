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
	UNREFERENCED_PARAMETER(clipNumber);
	//TODO: complete
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if(clipNumber > m_pMeshFilter->m_AnimationClips.size())
	{
	//If not,
		//	Call Reset
		Reset(true);
		//	Log a warning with an appropriate message
		Logger::LogWarning(L"clipNumber is higher than the amount of existing clips -> SetAnimation(UINT clipNumber), ModelAnimator.cpp");
		//	return
		return;
	}
	//else
		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		//	Call SetAnimation(AnimationClip clip)
	else SetAnimation(m_pMeshFilter->m_AnimationClips[clipNumber]);
}

void ModelAnimator::SetAnimation(std::wstring clipName)
{
	UNREFERENCED_PARAMETER(clipName);
	//TODO: complete
	//Set m_ClipSet to false
	m_ClipSet = false;
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	auto it = std::find_if(m_pMeshFilter->m_AnimationClips.begin(), m_pMeshFilter->m_AnimationClips.end(), [clipName](AnimationClip clip)
	{
		return clip.Name == clipName;
	});

	//If found,
	if(it != m_pMeshFilter->m_AnimationClips.end())
	{
		//	Call SetAnimation(Animation Clip) with the found clip
		SetAnimation(*it);
	}
	//Else
	else
	{
		//	Call Reset
		Reset(true);
		//	Log a warning with an appropriate message
		Logger::LogWarning(L"clip with given name does not exist -> SetAnimation(std::wstring clipName), ModelAnimator.cpp");
	}
}

void ModelAnimator::SetAnimation(AnimationClip clip)
{
	UNREFERENCED_PARAMETER(clip);
	//TODO: complete
	//Set m_ClipSet to true
	m_ClipSet = true;
	//Set m_CurrentClip
	m_CurrentClip = clip;
	//Call Reset(false)
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	UNREFERENCED_PARAMETER(pause);
	//TODO: complete
	//If pause is true, set m_IsPlaying to false
	if (pause)
		m_IsPlaying = false;

	//Set m_TickCount to zero
	m_TickCount = 0;
	//Set m_AnimationSpeed to 1.0f
	m_AnimationSpeed = 1.0f;
	
	//If m_ClipSet is true
	if(m_ClipSet)
	{
		//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
		std::vector<DirectX::XMFLOAT4X4> boneTransforms = m_CurrentClip.Keys.front().BoneTransforms;
		//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
		m_Transforms.assign(boneTransforms.begin(), boneTransforms.end());
	}
	//Else
	else
	{
		//	Create an IdentityMatrix
		DirectX::XMFLOAT4X4 identityMatrix;
		DirectX::XMStoreFloat4x4(&identityMatrix, DirectX::XMMatrixIdentity());
		//	Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
		m_Transforms.assign(m_CurrentClip.Keys.front().BoneTransforms.size(),identityMatrix);
	}
}

void ModelAnimator::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);
	//TODO: complete
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		float elapsedSec = gameContext.pGameTime->GetElapsed();
		float passedTicks = elapsedSec * m_CurrentClip.TicksPerSecond * m_AnimationSpeed;
		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		passedTicks = fmod(passedTicks, m_CurrentClip.Duration);

		//2.
		//IF m_Reversed is true
		if (m_Reversed)
		{
			//	Subtract passedTicks from m_TickCount
			m_TickCount -= passedTicks;
			//	If m_TickCount is smaller than zero, add m_CurrentClip.Duration to m_TickCount
			if (m_TickCount < 0)
				m_TickCount += m_CurrentClip.Duration;
		}
		//ELSE
		else
		{
			//	Add passedTicks to m_TickCount
			m_TickCount += passedTicks;
			//	if m_TickCount is bigger than the clip duration, subtract the duration from m_TickCount
			if (m_TickCount > m_CurrentClip.Duration)
				m_TickCount -= m_CurrentClip.Duration;
		}

		//3.
		//Find the enclosing keys
		AnimationKey keyA, keyB;
		//Iterate all the keys of the clip and find the following keys:
		//keyA > Closest Key with Tick before/smaller than m_TickCount
		//keyB > Closest Key with Tick after/bigger than m_TickCount
		for (size_t i{}; i < m_CurrentClip.Keys.size(); ++i)
		{
			if (m_CurrentClip.Keys[i].Tick < m_TickCount)
			{
				keyA = m_CurrentClip.Keys[i];
				keyB = m_CurrentClip.Keys[(i + 1) % m_CurrentClip.Keys.size()];
			}
		}

		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		float keyLength = m_CurrentClip.Duration / m_CurrentClip.Keys.size();
		float blendFactor = (keyB.Tick - m_TickCount) / keyLength;
		if (blendFactor < 0) 
			blendFactor = m_TickCount;
		//Clear the m_Transforms vector
		m_Transforms.clear();
		//FOR every boneTransform in a key (So for every bone)
		for (size_t i{}; i < m_CurrentClip.Keys.front().BoneTransforms.size(); ++i)
		{
			//	Retrieve the transform from keyA (transformA)
			auto transformA = keyA.BoneTransforms[i];
			// 	Retrieve the transform from keyB (transformB)
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
