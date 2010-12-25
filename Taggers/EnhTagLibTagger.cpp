//	/*
// 	*
// 	* Copyright (C) 2003-2010 Alexandros Economou
//	*
//	* This file is part of Jaangle (http://www.jaangle.com)
// 	*
// 	* This Program is free software; you can redistribute it and/or modify
// 	* it under the terms of the GNU General Public License as published by
// 	* the Free Software Foundation; either version 2, or (at your option)
// 	* any later version.
// 	*
// 	* This Program is distributed in the hope that it will be useful,
// 	* but WITHOUT ANY WARRANTY; without even the implied warranty of
// 	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// 	* GNU General Public License for more details.
// 	*
// 	* You should have received a copy of the GNU General Public License
// 	* along with GNU Make; see the file COPYING. If not, write to
// 	* the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
// 	* http://www.gnu.org/copyleft/gpl.html
// 	*
//	*/ 
#include "stdafx.h"
#include "EnhTagLibTagger.h"
#include "TeenSpiritEnums.h"
#include "PrgAPI.h"


UINT EnhTagLibTagger::Read(LPCTSTR fileName, TagInfo& tagInfo)
{
	ASSERT(fileName != NULL);
	TRACEST(_T("EnhTagLibTagger::Read"), fileName);
	tagInfo = TagInfo();//This is the only tagger
	tagInfo.Size = (UINT) GetFileSize(fileName) / 1024;
	if (tagInfo.Size == 0)
		return 0;
	UINT requestedTiFlags = tagInfo.validFields;
	UINT aquiredTiFlags = TagInfo_Size;

	if (m_readOption > 20)
	{
		TrackTypesEnum tt = GetTrackType(fileName);
		BOOL bReadTags = TRUE;
		PrgAPI* pAPI = PRGAPI();
		switch (tt)
		{
		case TTYPE_mp3:
			bReadTags = (pAPI->GetOption(OPT_TAG_MP3) & 1) == 1;
			break;
		case TTYPE_ogg:
			bReadTags = (pAPI->GetOption(OPT_TAG_OGG) & 1) == 1;
			break;
		case TTYPE_oga:
			bReadTags = (pAPI->GetOption(OPT_TAG_OGA) & 1) == 1;
			break;
		case TTYPE_flac:
			bReadTags = (pAPI->GetOption(OPT_TAG_FLAC) & 1) == 1;
			break;
		case TTYPE_mpc:
			bReadTags = (pAPI->GetOption(OPT_TAG_MPC) & 1) == 1;
			break;
		case TTYPE_wv:
			bReadTags = (pAPI->GetOption(OPT_TAG_WV) & 1) == 1;
			break;
		case TTYPE_spx:
			bReadTags = (pAPI->GetOption(OPT_TAG_SPX) & 1) == 1;
			break;
		case TTYPE_tta:
			bReadTags = (pAPI->GetOption(OPT_TAG_TTA) & 1) == 1;
			break;
		case TTYPE_m4a:
		case TTYPE_m4b:
		case TTYPE_m4p:
		case TTYPE_3g2:
			//=== Crashes on >2 users on v866
			bReadTags = (pAPI->GetOption(OPT_TAG_M4A) & 1) == 1;
			break;
		case TTYPE_wma:
			bReadTags = (pAPI->GetOption(OPT_TAG_WMA) & 1) == 1;
			break;
		case TTYPE_aiff:
		case TTYPE_aif:
			bReadTags = (pAPI->GetOption(OPT_TAG_AIF) & 1) == 1;
			break;
		case TTYPE_wav:
			bReadTags = (pAPI->GetOption(OPT_TAG_WAV) & 1) == 1;
			break;
		default:
			break;
		}
		if (bReadTags)
			aquiredTiFlags |= m_taglib.Read(fileName, tagInfo);
	}

	if (tagInfo.Title.empty() && tagInfo.Artist.empty())
		aquiredTiFlags |= m_fntag.Read(fileName, tagInfo);
//		|| tagInfo.Title.empty())
	if (m_readOption > 50)
	{
		//Use BassLib
		aquiredTiFlags |= m_bass.Read(fileName, tagInfo);
	}
	if (m_readOption > 80 && ((aquiredTiFlags & TagInfo_Length) == 0))
	{
		m_dshow.SetConfig(CONF_ReadDepth, m_readOption >= 90 ? 80 : 20);
		aquiredTiFlags |= m_dshow.Read(fileName, tagInfo);
	}

	//=== This is specially for wav files which i consider safe
	//if (m_readOption > 20 && ((aquiredTiFlags & TagInfo_Length) == 0))
	//{
	//	INT len = _tcslen(fileName);
	//	if (len > 5)
	//	{
	//		if (_tcsicmp(&fileName[len - 4], _T(".wav")) == 0)
	//			aquiredTiFlags |= m_bass.Read(fileName, tagInfo);
	//	}
	//}

#ifdef _DEBUG
	if (tagInfo.Title.size() == 0)
	{
		TCHAR bf[MAX_PATH];
		_tcsncpy(bf, fileName, MAX_PATH);
		LPTSTR pos = _tcsrchr(bf, _T('.'));
		pos[0] = 0;
		pos = _tcsrchr(bf, _T('\\'));
		pos++;
		tagInfo.Title = pos;
		aquiredTiFlags |= TagInfo_Title;
	}
#endif
	if (tagInfo.Length > 0 && tagInfo.Bitrate == 0)
	{
		tagInfo.Bitrate = (8 * tagInfo.Size) / tagInfo.Length;
		aquiredTiFlags |= TagInfo_Bitrate;
	}

	return aquiredTiFlags;
}

UINT EnhTagLibTagger::Write(LPCTSTR fileName, TagInfo& info)
{
	TrackTypesEnum tt = GetTrackType(fileName);
	PrgAPI* pAPI = PRGAPI();
	switch (tt)
	{
	case TTYPE_mp3:
		if ((pAPI->GetOption(OPT_TAG_MP3) & 2) == 0)
			return TagInfo_None;
		break;
	case TTYPE_ogg:
		if ((pAPI->GetOption(OPT_TAG_OGG) & 2) == 0)
			return TagInfo_None;
		break;
	case TTYPE_oga:
		if ((pAPI->GetOption(OPT_TAG_OGA) & 2) == 0)
			return TagInfo_None;
		break;
	case TTYPE_flac:
		if ((pAPI->GetOption(OPT_TAG_FLAC) & 2) == 0)
			return TagInfo_None;
		break;
	case TTYPE_mpc:
		if ((pAPI->GetOption(OPT_TAG_MPC) & 2) == 0)
			return TagInfo_None;
		break;
	case TTYPE_wv:
		if ((pAPI->GetOption(OPT_TAG_WV) & 2) == 0)
			return TagInfo_None;
		break;
	case TTYPE_spx:
		if ((pAPI->GetOption(OPT_TAG_SPX) & 2) == 0)
			return TagInfo_None;
		break;
	case TTYPE_tta:
		if ((pAPI->GetOption(OPT_TAG_TTA) & 2) == 0)
			return TagInfo_None;
		break;
	case TTYPE_m4a:
	case TTYPE_m4b:
	case TTYPE_m4p:
	case TTYPE_3g2:
		//=== Crashes on >2 users on v866
		if ((pAPI->GetOption(OPT_TAG_M4A) & 2) == 0)
			return TagInfo_None;
		break;
	case TTYPE_wma:
		return TagInfo_None;
		//=== Remove the tagging support from wma's. Can destroy files. Last Check TagLib 1.6 (Jaangle .97b) 6-10-2009
		//if ((pAPI->GetOption(OPT_TAG_WMA) & 2) == 0)
		//	return TagInfo_None;
		break;
	case TTYPE_aiff:
	case TTYPE_aif:
		if ((pAPI->GetOption(OPT_TAG_AIF) & 2) == 0)
			return TagInfo_None;
		break;
	case TTYPE_wav:
		if ((pAPI->GetOption(OPT_TAG_WAV) & 2) == 0)
			return TagInfo_None;
		break;
	default:
		return TagInfo_None;
	}

	return m_taglib.Write(fileName, info);
}

BOOL EnhTagLibTagger::ClearTags(LPCTSTR fileName)
{
	return m_taglib.ClearTags(fileName);
}

LONGLONG EnhTagLibTagger::GetFileSize(LPCTSTR fileName)
{
	HANDLE hFile = CreateFile(fileName, 
		FILE_READ_ATTRIBUTES,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("@1 EnhTagLibTagger::GetFileSize. Can't Open File\r\n"));
		return 0;
	}

	LARGE_INTEGER size;
	::GetFileSizeEx(hFile, &size);
	CloseHandle(hFile);
	return (LONGLONG)size.QuadPart;
}

void EnhTagLibTagger::SetConfig(Config conf, INT val)
{
	if (conf == CONF_ReadDepth)
		m_readOption = val;
}
INT EnhTagLibTagger::GetConfig(Config conf)
{
	if (conf = CONF_ReadDepth)
		return m_readOption;
	return -1;
}
