#include "stdafx.h"
#include "SpriteFontLoader.h"
#include "BinaryReader.h"
#include "ContentManager.h"
#include "TextureData.h"

SpriteFont* SpriteFontLoader::LoadContent(const std::wstring& assetFile)
{
	auto pBinReader = new BinaryReader();
	pBinReader->Open(assetFile);

	if (!pBinReader->Exists())
	{
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Failed to read the assetFile!\nPath: \'%s\'", assetFile.c_str());
		return nullptr;
	}

	//See BMFont Documentation for Binary Layout

	//Parse the Identification bytes (B,M,F)
	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr
	if ((pBinReader->Read<char>() != 'B') || (pBinReader->Read<char>() != 'M') || (pBinReader->Read<char>() != 'F'))
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid .fnt font");
		return nullptr;
	}

	//Parse the version (version 3 required)
	//If version is < 3,
	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
	//return nullptr
	if (pBinReader->Read<byte>() < 3)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported");
		return nullptr;
	}

	//Valid .fnt file
	auto pSpriteFont = new SpriteFont();
	//SpriteFontLoader is a friend class of SpriteFont
	//That means you have access to its privates (pSpriteFont->m_FontName = ... is valid)

	//**********
	// BLOCK 0 *
	//**********
	//Retrieve the blockId and blockSize
	byte blockId = pBinReader->Read<byte>();
	int blockSize = pBinReader->Read<int>();
	//Retrieve the FontSize (will be -25, BMF bug) [SpriteFont::m_FontSize]
	pSpriteFont->m_FontSize = pBinReader->Read<short>();
	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	pBinReader->MoveBufferPosition(12);
	//Retrieve the FontName [SpriteFont::m_FontName]
	pSpriteFont->m_FontName = pBinReader->ReadNullString();

	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pBinReader->Read<byte>();
	blockSize = pBinReader->Read<int>();
	//Retrieve Texture Width & Height [SpriteFont::m_TextureWidth/m_TextureHeight]
	pBinReader->MoveBufferPosition(4);
	pSpriteFont->m_TextureWidth = pBinReader->Read<USHORT>();
	pSpriteFont->m_TextureHeight = pBinReader->Read<USHORT>();
	//Retrieve PageCount
	//> if pagecount > 1
	//> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed)
	unsigned int pageCount = pBinReader->Read<USHORT>();
	if (pageCount > 1)
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed");
	}
	//Advance to Block2 (Move Reader)
	pBinReader->MoveBufferPosition(5);


	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pBinReader->Read<byte>();
	blockSize = pBinReader->Read<int>();
	//Retrieve the PageName (store Local)
	//	> If PageName is empty
	//	> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty])
	std::wstring pageName = pBinReader->ReadNullString();
	if(pageName.empty())
	{
		Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty]");
	}
	//>Retrieve texture filepath from the assetFile path
	//> (ex. c:/Example/somefont.fnt => c:/Example/) [Have a look at: wstring::rfind()]
	std::wstring filePath = assetFile.substr(0, assetFile.find_last_of('/') + 1);
	//>Use path and PageName to load the texture using the ContentManager [SpriteFont::m_pTexture]
	//> (ex. c:/Example/ + 'PageName' => c:/Example/somefont_0.png)
	filePath.append(pageName);
	pSpriteFont->m_pTexture = ContentManager::Load<TextureData>(filePath);

	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	blockId = pBinReader->Read<byte>();
	blockSize = pBinReader->Read<int>();
	//Retrieve Character Count (see documentation)
	int charCount = blockSize / 20;
	//Retrieve Every Character, For every Character:
	for (int i = 0; i < charCount; ++i)
	{
		//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
		wchar_t characterId = static_cast<wchar_t>(pBinReader->Read<UINT>());
		//> Check if CharacterId is valid (SpriteFont::IsCharValid), Log Warning and advance to next character if not valid
		if (!pSpriteFont->IsCharValid(characterId))
		{
			Logger::LogWarning(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Character ID");
			continue;
		}

		//> Retrieve the corresponding FontMetric (SpriteFont::GetMetric) [REFERENCE!!!]
		FontMetric& fm = pSpriteFont->GetMetric(characterId);
		//> Set IsValid to true [FontMetric::IsValid]
		fm.IsValid = true;
		//> Set Character (CharacterId) [FontMetric::Character]
		fm.Character = characterId;
		//> Retrieve Xposition (store Local)
		unsigned int xPosition = pBinReader->Read<USHORT>();
		//> Retrieve Yposition (store Local)
		unsigned int yPosition = pBinReader->Read<USHORT>();
		//> Retrieve & Set Width [FontMetric::Width]
		fm.Width = pBinReader->Read<USHORT>();
		//> Retrieve & Set Height [FontMetric::Height]
		fm.Height = pBinReader->Read<USHORT>();
		//> Retrieve & Set OffsetX [FontMetric::OffsetX]
		fm.OffsetX = pBinReader->Read<short>();
		//> Retrieve & Set OffsetY [FontMetric::OffsetY]
		fm.OffsetY = pBinReader->Read<short>();
		//> Retrieve & Set AdvanceX [FontMetric::AdvanceX]
		fm.AdvanceX = pBinReader->Read<short>();
		//> Retrieve & Set Page [FontMetric::Page]
		fm.Page = pBinReader->Read<byte>();
		//> Retrieve Channel (BITFIELD!!!) 
		//	> See documentation for BitField meaning [FontMetrix::Channel]
		byte channel = pBinReader->Read<byte>();
		switch (channel)
		{
		case 1:
			fm.Channel = 2;
			break;
		case 2:
			fm.Channel = 1;
			break;
		case 4:
			fm.Channel = 0;
			break;
		case 8:
			fm.Channel = 3;
			break;
		default:
			fm.Channel = 0;
			break;
		}
		//> Calculate Texture Coordinates using Xposition, Yposition, TextureWidth & TextureHeight [FontMetric::TexCoord]
		fm.TexCoord = DirectX::XMFLOAT2(xPosition / float(pSpriteFont->m_TextureWidth), yPosition / float(pSpriteFont->m_TextureHeight));
	}
	//DONE :)

	delete pBinReader;
	return pSpriteFont;
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}
