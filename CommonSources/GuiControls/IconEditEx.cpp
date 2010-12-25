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
#include "IconEditEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int cMargin = 2;
/////////////////////////////////////////////////////////////////////////////
// CIconEdit

CIconEditEx::CIconEditEx():
m_hIcon(NULL),
m_hIconInternal(NULL),
m_hClearIcon(NULL),
m_cx(16),
m_cy(16),
m_progressValue(0.0f)
{
	SetColor(COL_Bk, ::GetSysColor(COLOR_WINDOW));// Initializing background color to the system face color.
	SetColor(COL_Text, ::GetSysColor(COLOR_WINDOWTEXT));// Initializing text color to window text color
	SetColor(COL_Progress, RGB(128,128,128));
	SetColor(COL_EmptyMessage, RGB(192,192,192));
}

CIconEditEx::~CIconEditEx()
{
	ClearIcon();
}

BEGIN_MESSAGE_MAP(CIconEditEx, CEdit)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void CIconEditEx::OnPaint()
{
	CEdit::OnPaint();
	CClientDC dc(this);
	CRect rcClient;
	GetClientRect(&rcClient);
	INT left = rcClient.left + cMargin;
	INT top = (rcClient.Height() - m_cy) / 2;
	INT right = rcClient.right - cMargin;

	//CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	//CPen* pOldPen = (CPen*)dc.SelectStockObject(DC_PEN);
	//dc.SetDCPenColor(RGB(128,128,128));
	//dc.Rectangle(&rcClient);
	//dc.SelectObject(pOldBrush);
	//dc.SelectObject(pOldPen);

	if (m_hIcon != NULL)
	{
		//CRgn rgn;
		//rgn.CreateRectRgn(cMargin, cMargin, cMargin + m_cx, cMargin + m_cy);
		//pDC->SelectClipRgn(&rgn, RGN_OR);
		::DrawIconEx(dc.m_hDC, left, top, m_hIcon, m_cx, m_cy, 0, NULL, DI_NORMAL );
		left += cMargin + m_cx;
	}
	if (m_hClearIcon != NULL)
	{
		right -= m_cx - cMargin;
		::DrawIconEx(dc.m_hDC, right, top, m_hClearIcon, m_cx, m_cy, 0, NULL, DI_NORMAL );
	}

	if (GetWindowTextLength() == 0 && !m_textIfEmpty.IsEmpty())
	{
		//CClientDC dc(this);
		CRect rc(left, top, right- cMargin, rcClient.Height() - cMargin);
		//GetRect(&rc);
		CFont* oldFont = dc.SelectObject(GetFont());
		dc.SetTextColor(m_colors[COL_EmptyMessage]);
		dc.DrawText(m_textIfEmpty, &rc, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		dc.SelectObject(oldFont);
	}
	dc.DrawEdge(&rcClient, EDGE_BUMP, BF_RECT | BF_FLAT);
}


void CIconEditEx::SetEmptyMessage(LPCTSTR txt)
{
	m_textIfEmpty = txt;
	if (m_hWnd)
		Invalidate();
}

void CIconEditEx::PreSubclassWindow() 
{
	ASSERT( GetStyle() & ES_MULTILINE );
}

/////////////////////////////////////////////////////////////////////////////
// CIconEdit implementation

void CIconEditEx::SetIconDimentions(INT cx, INT cy)
{
	m_cx = cx;
	m_cy = cy;
}

void CIconEditEx::SetIcon(HICON icon) 
{
	m_hIcon = icon;
	//ReSize();
}
void CIconEditEx::SetClearIcon(HICON icon)
{
	m_hClearIcon = icon;
	//ReSize();
}

void CIconEditEx::SetIcon(UINT iconres) 
{
	ClearIcon();
	if (iconres > 0)
	{
		m_hIconInternal = ( HICON ) ::LoadImage( AfxGetResourceHandle(),
			MAKEINTRESOURCE( iconres ),
			IMAGE_ICON,
			m_cx,
			m_cy,
			LR_DEFAULTCOLOR );
		SetIcon(m_hIconInternal);
	}
}

void CIconEditEx::ClearIcon()
{
	if(m_hIconInternal != NULL)
	{
		::DestroyIcon(m_hIconInternal);
		m_hIconInternal = NULL;
	}
}

LRESULT CIconEditEx::OnSetFont( WPARAM wParam, LPARAM lParam )
{
	LRESULT res = DefWindowProc( WM_SETFONT, wParam, lParam );
	ReSize();
	return res;
}

void CIconEditEx::OnSize( UINT nType, int cx, int cy ) 
{
	CEdit::OnSize( nType, cx, cy );
	if (cx > 0 && cy >0)
		ReSize(cx, cy);
}

void CIconEditEx::ReSize(int cx, int cy )
{
	if (cx == 0 && cy == 0)
	{
		CRect rc;
		GetClientRect(&rc);
		cx = rc.Width();
		cy = rc.Height();
	}
	LOGFONT lf;
	CFont* pFont = GetFont();
	int yMarg = cMargin;
	if (pFont != NULL)
	{
		pFont->GetLogFont(&lf);
		yMarg = (cy - 2 * cMargin - abs(lf.lfHeight)) / 2 - 1;
		if (yMarg < 0)
			yMarg = 0;
	}
	INT left = cMargin;
	INT top = cMargin + yMarg;
	if (m_hIcon != NULL)
	{
		left += cMargin + m_cx;
	}
	INT right = cx - cMargin;
	if (m_hClearIcon != NULL)
	{
		right -= (cMargin + m_cx);
	}
	SetRect(&CRect(left, top, right, cy - top));
}

HBRUSH CIconEditEx::CtlColor(CDC* pDC, UINT nCtlColor)
{
	HBRUSH hbr;
	hbr = (HBRUSH)m_brBkgnd; // Passing a Handle to the Brush
	CRect rcClient;
	GetClientRect(&rcClient);
	if (m_progressValue > 0.0)
	{
		CRect rc;
		GetRect(&rc);
		pDC->FillSolidRect(&rc, m_colors[COL_Bk]);
		GetClientRect(&rc);
		CRgn rgn;
		rgn.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
		pDC->SelectClipRgn(&rgn, RGN_COPY);
		pDC->FillSolidRect(0, 0, INT(rc.Width() * m_progressValue), rc.Height(), m_colors[COL_Progress]);
		pDC->SetBkMode(TRANSPARENT);
	}
	//if (m_hIcon != NULL)
	//{
	//	CRgn rgn;
	//	rgn.CreateRectRgn(cMargin, cMargin, cMargin + m_cx, cMargin + m_cy);
	//	pDC->SelectClipRgn(&rgn, RGN_OR);
	//	INT top = (rcClient.Height() - m_cy) / 2;
	//	::DrawIconEx(pDC->m_hDC, cMargin, top, m_hIcon, m_cx, m_cy, 0, NULL, DI_NORMAL );
	//}
	//if (m_hClearIcon != NULL)
	//{
	//	INT top = (rcClient.Height() - m_cy) / 2;
	//	::DrawIconEx(pDC->m_hDC, cMargin, top, m_hIcon, m_cx, m_cy, 0, NULL, DI_NORMAL );
	//}
	pDC->SetBkColor(m_colors[COL_Bk]); // Setting the Color of the Text Background to the one passed by the Dialog
	pDC->SetTextColor(m_colors[COL_Text]); // Setting the Text Color to the one Passed by the Dialog
	return hbr;
}

void CIconEditEx::SetProgressValue(DOUBLE value)
{
	//TRACE(_T("@1CIconEditEx::SetProgressValue: %d\n"), value);
	if (value < 0.0)
		value = 0.0;
	if (value > 1.0)
		value = 1.0;
	if (m_progressValue != value)
	{
		m_progressValue = value;
		Invalidate(TRUE);
	}
}

void CIconEditEx::SetColor(Colors col, COLORREF crColor)
{
	ASSERT(col < COL_Last);
	m_colors[col] = crColor;
	if (col == COL_Bk)
	{
		m_brBkgnd.DeleteObject();
		m_brBkgnd.CreateSolidBrush(crColor);
	}
	if (m_hWnd)
		RedrawWindow();
}

void CIconEditEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (PointOverClearIcon(point))
	{
		SetWindowText(_T(""));
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), EN_CHANGE), (LPARAM)m_hWnd);
	}
	return CWnd::OnLButtonDown(nFlags, point);
}

BOOL CIconEditEx::PointOverClearIcon(CPoint& point)
{
	if (m_hClearIcon != NULL)
	{
		CRect rc;
		GetClientRect(&rc);
		rc.left = rc.right - m_cx - cMargin;
		if (rc.PtInRect(point))
			return TRUE;
	}
	return FALSE;
}


BOOL CIconEditEx::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint p;
	GetCursorPos(&p);
	ScreenToClient(&p);
	if (PointOverClearIcon(p))
	{
		HCURSOR hCursor = ::LoadCursor (NULL, IDC_HAND);
		if (NULL != hCursor)
		{
			::SetCursor(hCursor);
			return TRUE;
		}
	}
	else if (m_hIcon != NULL)
	{
		if (p.x < m_cx + cMargin)
		{
			HCURSOR hCursor = ::LoadCursor (NULL, IDC_ARROW);
			if (NULL != hCursor)
			{
				::SetCursor(hCursor);
				return TRUE;
			}
		}
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}




