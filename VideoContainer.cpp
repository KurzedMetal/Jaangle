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
#include "VideoContainer.h"

#define PLAYERSTATUS_TIMER 100


CVideoContainer::~CVideoContainer()
{
}


BOOL CVideoContainer::Init(MediaPlayer* pPlayer)
{
	ASSERT(m_hWnd != NULL);
	ASSERT(pPlayer != NULL);
	if (pPlayer == NULL || m_hWnd == 0)
	{
		ASSERT(0);
		return FALSE;
	}
	HWND hwnd = pPlayer->GetVideoContainerHWND();
	if (hwnd == 0)
	{
		//=== The player does not have anything attached
		//=== DECISION: Attach it
		pPlayer->SetVideoContainerHWND(m_hWnd);
		pPlayer = pPlayer;
		SetTimer(PLAYERSTATUS_TIMER, 1000, NULL);

	}
	else if (hwnd == m_hWnd)
	{
		//=== The player is already attached in this viewer
		//=== DECISION: Do nothing

	}
	else
	{
		//=== The player is already attached to a different viewer
		return FALSE;
	}
	m_pMediaPlayer = pPlayer;
	return TRUE;
}

BEGIN_MESSAGE_MAP(CVideoContainer, CWnd)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


void CVideoContainer::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	if (m_pMediaPlayer != NULL && cx != 0 && cy != 0)
	{
		TRACEST(_T("CVideoContainer::OnSize"));
		if (m_pMediaPlayer->GetVideoContainerHWND() != m_hWnd)
			m_pMediaPlayer->SetVideoContainerHWND(m_hWnd);
		m_pMediaPlayer->SetVideoPosition(0,0,cx,cy);
	}
}

BOOL CVideoContainer::OnEraseBkgnd(CDC* pDC)
{
	if(m_pMediaPlayer != NULL)
	{
		if (m_pMediaPlayer->IsVideo())
			return TRUE;
	}
	CRect rc;
	GetClientRect(&rc);
	pDC->FillSolidRect(&rc, RGB(0,0,0));
	return TRUE;
}

LRESULT CVideoContainer::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_LBUTTONDBLCLK)
	{
		TRACE(_T("@4 CVideoContainer::WindowProc. WM_LBUTTONDBLCLK\r\n"));
		SetFullScreen(!m_bFullScreen);
	}
	return CWnd::WindowProc(message, wParam, lParam);
}

void CVideoContainer::SetFullScreen(BOOL bFullScreen)
{
	if (bFullScreen == m_bFullScreen)
		return;
	m_bFullScreen = bFullScreen;
	if (m_bFullScreen)
	{
		CRect rc;
		GetWindowRect(&rc);
		HMONITOR mon = ::MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		mi.cbSize = sizeof( mi );
		::GetMonitorInfo( mon, &mi );
		rc = mi.rcMonitor;

		GetWindowRect(&m_rcWindow);
		CPoint p(m_rcWindow.left, m_rcWindow.top);
		CSize sz(m_rcWindow.Width(), m_rcWindow.Height());
		::ScreenToClient(::GetParent(m_hWnd), &p);
		m_rcWindow.SetRect(p.x, p.y, p.x + sz.cx, p.y + sz.cy);

		m_windowStyle = GetWindowLong(m_hWnd, GWL_STYLE);
		m_windowExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
		m_windowParent = GetParent();
		DWORD newExStyle = m_windowExStyle & (~WS_EX_CLIENTEDGE);
		::ShowWindow(m_hWnd, SW_HIDE);
		SetWindowLong(m_hWnd, GWL_EXSTYLE, newExStyle);
		SetParent(NULL);
		::SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top, rc.Width(), rc.Height(), 0);
		::UpdateWindow(m_hWnd);
		::ShowWindow(m_hWnd, SW_SHOW);
	}
	else
	{
		SetParent(m_windowParent);
		SetWindowLong(m_hWnd, GWL_EXSTYLE, m_windowExStyle);
		SetWindowLong(m_hWnd, GWL_STYLE, m_windowStyle);
		::MoveWindow(m_hWnd, m_rcWindow.left, m_rcWindow.top, m_rcWindow.Width(), m_rcWindow.Height(), FALSE);
	}
}

void CVideoContainer::OnDestroy()
{
	if (m_pMediaPlayer != NULL)
	{
		if (m_pMediaPlayer->GetVideoContainerHWND() == m_hWnd)
			m_pMediaPlayer->SetVideoContainerHWND(NULL);
	}
	CWnd::OnDestroy();
}
