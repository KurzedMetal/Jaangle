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
#include "BassTagger.h"
#include "BassInstance.h"
#include <bass.h>

BassTagger::BassTagger()
{

}
BassTagger::~BassTagger()
{
}

UINT BassTagger::Read(LPCTSTR fileName, TagInfo& tagInfo)
{
	TRACEST(_T("BassTagger::Read"));
	if (!m_bassInstance.IsValid() || !m_bassInstance.IsFileSupported(fileName))
		return TagInfo_None;//Not supported
	HSTREAM channel = BASS_StreamCreateFile(FALSE, (LPCWSTR)CT2CW(fileName), 0, 0, BASS_UNICODE | BASS_STREAM_PRESCAN);
	UINT ret = TagInfo_None;
	if (channel != 0)
	{
		QWORD bytes = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
		DOUBLE time = BASS_ChannelBytes2Seconds(channel, bytes); // the time length 
		if (time > 0)
		{
			tagInfo.Length = (UINT)time;
			ret |= TagInfo_Length;
		}
		//DWORD freq;
		//if (BASS_ChannelGetAttributes(channel, &freq, NULL, NULL))
		//{
		//	TCHAR bf[30];
		//	_itot(freq, bf, 10);
		//	tagInfo.FormatInfo = bf; 
		//	ret |= TagInfo_FormatInfo;
		//}
		
		BASS_StreamFree(channel);
	}
	return ret;
}
