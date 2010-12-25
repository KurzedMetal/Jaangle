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
#include "TickerWnd.h"


// CLabelEx
#define OPTIONAL_TIMER	100
#define NORMAL_TIMER	110
#define PERMANENT_TIMER	120

IMPLEMENT_DYNAMIC(CTickerWnd, CWnd)

CTickerWnd::CTickerWnd():
m_cxIcon(16),
m_cyIcon(16),
m_pFont(0),
m_curPermamentMsg(0)
{
	m_delays[MT_Permanent] = 10000;	//Time to switch when in idle
	m_delays[MT_Normal] = 2000;		//Time that it WILL be shown (FIFO implementation)
	m_delays[MT_Optional] = 3000;	//Time before returning to IDLE (FIFO implementation)

	m_colors[COL_BkPerm] = m_colors[COL_BkNorm] = m_colors[COL_BkOpti] = GetSysColor(COLOR_BTNFACE);
	m_colors[COL_TextPerm] = m_colors[COL_TextNorm] = m_colors[COL_TextOpti] = GetSysColor(COLOR_MENUTEXT);
}

CTickerWnd::~CTickerWnd()
{
}


BEGIN_MESSAGE_MAP(CTickerWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL CTickerWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}

void CTickerWnd::DrawSurface(CDC& dc, HICON hIcon, LPCTSTR msg, COLORREF textColor, COLORREF bkColor)
{
	CRect rc;
	GetClientRect(&rc);
	dc.FillSolidRect(&rc, bkColor);
	const int xMargin=3;
	CRect textRC(rc);
	if (hIcon != 0)
	{
		DrawIconEx(dc.m_hDC, xMargin, (rc.Height() - m_cyIcon) / 2, hIcon, m_cxIcon, m_cyIcon, 0,0,DI_NORMAL);
		textRC.left = xMargin * 2 + m_cxIcon;
	}
	else
		textRC.left = xMargin;
	if (msg != NULL)
	{
		if (m_pFont == NULL)
			m_pFont	= GetFont();
		CFont* pOldFont = dc.SelectObject(m_pFont);
		dc.SetTextColor(textColor);
		dc.SetBkMode(TRANSPARENT);
		textRC.right = rc.Width() - xMargin;
		dc.DrawText(msg, textRC, DT_LEFT | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER);
		dc.SelectObject(pOldFont);
	}

}


void CTickerWnd::OnPaint()
{
#ifdef THREAD_SAFE_TICKERWND
	CSingleLock lock(&m_cs, TRUE);
#endif
	CPaintDC dc(this); // device context for painting
	if (!m_normalMessages.empty())
	{
		DrawSurface(dc, m_normalMessages.front().hIcon, 
			m_normalMessages.front().msg.c_str(), m_colors[COL_TextNorm], m_colors[COL_BkNorm]);		
	}
	else if (!m_permanantMessages.empty())
	{
		ASSERT(m_curPermamentMsg < INT(m_permanantMessages.size()));
		if (m_curPermamentMsg >= INT(m_permanantMessages.size()))
			m_curPermamentMsg = 0;
		DrawSurface(dc, m_permanantMessages[m_curPermamentMsg].hIcon, 
			m_permanantMessages[m_curPermamentMsg].msg.c_str(), m_colors[COL_TextPerm], m_colors[COL_BkPerm]);		
	}
	else
		DrawSurface(dc, 0, 0, m_colors[COL_TextPerm], m_colors[COL_BkPerm]);

}

void CTickerWnd::SetIconSize(INT cxIcon, INT cyIcon)
{
	m_cxIcon = cxIcon;
	m_cyIcon = cyIcon;
}
void CTickerWnd::SetMessageDelay(MESSAGE_TYPE mt, UINT delay)
{
	ASSERT(mt < MT_Last);
	if (mt < MT_Last)
		m_delays[mt] = delay;
}

void CTickerWnd::AddMessage(MESSAGE_TYPE mt, LPCTSTR msg, HICON hIcon)
{
#ifdef THREAD_SAFE_TICKERWND
	CSingleLock lock(&m_cs, TRUE);
#endif
	ASSERT(mt < MT_Last);
	switch (mt)
	{
	case MT_Permanent:
		if (m_permanantMessages.empty())
			SetTimer(PERMANENT_TIMER, m_delays[MT_Permanent], NULL);
		m_permanantMessages.push_back(MsgStruct(msg, hIcon));
		break;
	case MT_Normal:
		if (m_normalMessages.empty())
		{
			KillTimer(OPTIONAL_TIMER);
			SetTimer(NORMAL_TIMER, m_delays[MT_Normal], NULL);
			Invalidate();
		}
		m_normalMessages.push(MsgStruct(msg, hIcon));
		break;
	case MT_Optional:
		if (m_normalMessages.empty())
		{
			KillTimer(OPTIONAL_TIMER);
			SetTimer(OPTIONAL_TIMER, m_delays[MT_Optional], NULL);
			CClientDC dc(this);
			DrawSurface(dc, hIcon, msg, 
				m_colors[COL_TextOpti], m_colors[COL_BkOpti]);
		}
		break;
	}
}

void CTickerWnd::ClearMessages(MESSAGE_TYPE mt)
{
#ifdef THREAD_SAFE_TICKERWND
	CSingleLock lock(&m_cs, TRUE);
#endif
	ASSERT(mt == MT_Normal || mt == MT_Permanent);
	switch (mt)
	{
	case MT_Normal:
		while (!m_normalMessages.empty())
			m_normalMessages.pop();
		break;
	case MT_Permanent:
		m_permanantMessages.clear();
		break;
	}
}
void CTickerWnd::SetFont(CFont& m_font)
{
	m_pFont = &m_font;
}



//---------------------------------------------------
//ICtrlColors ---------------------------------------
//---------------------------------------------------
void CTickerWnd::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
	{
		m_colors[idx] = value;
		if (m_hWnd)		Invalidate();
	}
}
COLORREF CTickerWnd::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}
const TCHAR* const cColorNames[] = {
	_T("BkPerm"),
	_T("TextPerm"),
	_T("BkNorm"),
	_T("TextNorm"),
	_T("BkOpti"),
	_T("TextOpti"),
	NULL
};


LPCTSTR CTickerWnd::GetColorName(UINT idx)
{
	ASSERT(idx <= COL_Last);
	if (idx < COL_Last)
		return cColorNames[idx];
	return NULL;
}
//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------

void CTickerWnd::OnTimer(UINT_PTR nIDEvent)
{
#ifdef THREAD_SAFE_TICKERWND
	CSingleLock lock(&m_cs, TRUE);
#endif
	switch (nIDEvent)
	{
	case NORMAL_TIMER:
		m_normalMessages.pop();
		if (m_normalMessages.empty())
			KillTimer(NORMAL_TIMER);
		Invalidate();
		break;
	case OPTIONAL_TIMER:
		KillTimer(OPTIONAL_TIMER);
		Invalidate();
		break;
	case PERMANENT_TIMER:
		if (m_normalMessages.empty() && !m_permanantMessages.empty())
		{
			INT sz = INT(m_permanantMessages.size());
			m_curPermamentMsg++;
			if (m_curPermamentMsg >= sz)
				m_curPermamentMsg = 0;
			if (sz>1)
				Invalidate();
		};
		break;
	}

	__super::OnTimer(nIDEvent);
}
