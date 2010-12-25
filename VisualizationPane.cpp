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
#include "VisualizationPane.h"
#include "MediaPlayer.h"
#include "PrgAPI.h"

INT VisualizationPane::m_instances = 0;

VisualizationPane::VisualizationPane()
{
	m_instances++;
}

VisualizationPane::~VisualizationPane()
{
	m_visCtrl.DestroyWindow();
	m_instances--;
}

BOOL VisualizationPane::Init(HWND hwndParent)
{
	if (m_visCtrl.GetSafeHwnd())
		return TRUE;
	if (m_visCtrl.Create(_T("STATIC"), NULL, WS_VISIBLE | WS_CHILD, CRect(0,0,0,0), CWnd::FromHandle(hwndParent), 910, NULL))
	{
		m_visCtrl.SetMediaPlayer(PRGAPI()->GetMediaPlayer());
		return TRUE;
	}
	return FALSE;
}

void VisualizationPane::UnInit()
{
	if (m_visCtrl.GetSafeHwnd())
		m_visCtrl.DestroyWindow();
}

BOOL VisualizationPane::Show()
{
	if (m_visCtrl.GetSafeHwnd())
	{
		m_visCtrl.ShowWindow(SW_SHOW);
		return TRUE;
	}
	return FALSE;
}

void VisualizationPane::Hide()
{
	if (m_visCtrl.GetSafeHwnd())
		m_visCtrl.ShowWindow(SW_HIDE);
}

void VisualizationPane::MovePane(INT x, INT y, INT cx, INT cy)
{
	m_visCtrl.MoveWindow(x, y, cx, cy);
}

LPCTSTR VisualizationPane::GetTitle(UINT idx)
{
	switch (idx % 1)
	{
	case 0:
		return PRGAPI()->GetString(IDS_VISUALIZATIONS);
	}
	return NULL;
}

BOOL VisualizationPane::LoadState(IPaneState& paneState)
{
	return TRUE;
}

BOOL VisualizationPane::SaveState(IPaneState& paneState)
{
	return TRUE;
}

BOOL VisualizationPane::OnStateChanged(UINT stateMessages)
{
	BOOL bHandled = FALSE;
	return bHandled;
}

