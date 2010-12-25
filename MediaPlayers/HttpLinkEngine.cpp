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
#include "HttpLinkEngine.h"
#include "cStringUtils.h"
#include "PrgAPI.h"

HttpLinkEngine::HttpLinkEngine()
{
}

HttpLinkEngine::~HttpLinkEngine()
{
}

BOOL HttpLinkEngine::Open(LPCTSTR str)
{
	Close();
	if (CanPlay(str))
	{
		m_ps = PS_Playing;
		m_URL = str;
		return TRUE;
	}
	return FALSE;
}

void HttpLinkEngine::Close()
{
	m_ps = PS_Closed;
	m_URL.clear();
}

BOOL HttpLinkEngine::Start()
{
	if (!m_URL.empty())
	{
		m_ps = PS_Playing;
		//PRGAPI()->SendMessage(SM_ExecuteLink);
		//ShellExecute(NULL, NULL, m_URL.c_str(), 0, 0, SW_SHOWNORMAL);
		return TRUE;
	}
	return FALSE;
}

void HttpLinkEngine::SetVolume(INT volume)
{
	ASSERT(volume <= 100);
	if (volume < 0)		volume = 0;
	if (volume > 100)	volume = 100;
	m_volume = volume;
}

BOOL HttpLinkEngine::CanPlay(LPCTSTR str)
{
	return _tcsistr(str, _T("http://")) != 0;
}
