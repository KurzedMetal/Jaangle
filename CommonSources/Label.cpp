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
#include "Label.h"

IMPLEMENT_DYNAMIC(CLabel, CWnd)

CLabel::CLabel():
m_drawStringParams(DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS)
{
	m_colors[COL_Bk] = GetSysColor(COLOR_BTNFACE);
	m_colors[COL_Text] = GetSysColor(COLOR_MENUTEXT);
}

CLabel::~CLabel()
{
}


BEGIN_MESSAGE_MAP(CLabel, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(WM_GETTEXT, OnGetText)
END_MESSAGE_MAP()



BOOL CLabel::Create(LPCTSTR caption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create(_T("STATIC"), caption, dwStyle, rect, pParentWnd, nID);
}


BOOL CLabel::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}


void CLabel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rc;
	GetClientRect(&rc);
	dc.SetTextColor(m_colors[COL_Text]);
	dc.SetBkMode(TRANSPARENT);
	CFont* pOldFont = dc.SelectObject(GetFont());
	dc.FillSolidRect(&rc, m_colors[COL_Bk]);
	dc.DrawText(m_text.c_str(), m_text.size(), rc, m_drawStringParams);
	dc.SelectObject(pOldFont);
}


afx_msg LRESULT CLabel::OnSetText(WPARAM wParam, LPARAM lParam)
{
	if (lParam != 0)
		m_text = ((LPCTSTR)lParam);
	else
		m_text.clear();
	if (m_hWnd)
		Invalidate();
	return 0;
}

afx_msg LRESULT CLabel::OnGetText(WPARAM wParam, LPARAM lParam)
{
	_tcsncpy((LPTSTR)lParam, m_text.c_str(), wParam);
	return 1;
}

void CLabel::SetColor(COLORS idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
	{
		m_colors[idx] = value;
		if (m_hWnd)		
			Invalidate();
	}
}
COLORREF CLabel::GetColor(COLORS idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}

void CLabel::CalculateSize(LPCTSTR text, int textLen, OUT SIZE& size)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(GetFont());
	GetTextExtentPoint(dc.m_hDC, text, textLen, &size);
	dc.SelectObject(pOldFont);
}


