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
#include "TeenSpirit.h"
#include "VideoPane.h"
#include "MediaPlayer.h"
#include "PrgAPI.h"
#include "TSBrowserEnh.h"


VideoPane::VideoPane()
{
}

VideoPane::~VideoPane()
{
	m_visCtrl.DestroyWindow();
}

BOOL VideoPane::Init(HWND hwndParent)
{
	if (m_visCtrl.GetSafeHwnd())
		return TRUE;
	MediaPlayer* pPlayer = PRGAPI()->GetMediaPlayer();
	if (pPlayer->GetVideoContainerHWND() != NULL)
		return FALSE;
	if (m_visCtrl.Create( NULL, NULL, WS_VISIBLE | WS_CHILD, CRect(0,0,0,0), CWnd::FromHandle(hwndParent), 910, NULL))
	{
		return m_visCtrl.Init(pPlayer);
	}
	return FALSE;
}
void VideoPane::UnInit()
{
	if (m_visCtrl.GetSafeHwnd())
		m_visCtrl.DestroyWindow();
}

BOOL VideoPane::Show()
{
	if (m_visCtrl.GetSafeHwnd())
	{
		m_visCtrl.ShowWindow(SW_SHOW);
		return TRUE;
	}
	return FALSE;
}
void VideoPane::Hide()
{
	if (m_visCtrl.GetSafeHwnd())
		m_visCtrl.ShowWindow(SW_HIDE);
}
void VideoPane::MovePane(INT x, INT y, INT cx, INT cy)
{
	m_visCtrl.MoveWindow(x, y, cx, cy);
}

LPCTSTR VideoPane::GetTitle(UINT idx)
{
	return PRGAPI()->GetString(IDS_VIDEO);
	//switch (idx % 1)
	//{
	//case 0:
	//	return PRGAPI()->GetString(IDS_VIDEO);
	//}
	//return NULL;
}

BOOL VideoPane::LoadState(IPaneState& paneState)
{
	return TRUE;
}

BOOL VideoPane::SaveState(IPaneState& paneState)
{
	return TRUE;
}



BOOL VideoPane::OnStateChanged(UINT stateMessages)
{
	if (stateMessages == SM_MediaChanged)
	{
		MediaPlayer* pPlayer = PRGAPI()->GetMediaPlayer();
		if (pPlayer)
		{
			if (pPlayer->IsVideo() == FALSE)
			{
				if (m_visCtrl.IsFullScreen())
					m_visCtrl.SetFullScreen(FALSE);
			}
		}
		return TRUE;
	}
	return FALSE;
}


BOOL VideoPane::IsCreatable()
{
	MediaPlayer* pPlayer = PRGAPI()->GetMediaPlayer();
	return pPlayer->GetVideoContainerHWND() == NULL;
}




