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
#include "FastSplitter.h"

/////////////////////////////////////////////////////////////////////////////
// CFastSplitter

CFastSplitter::CFastSplitter():
m_pWndParent(NULL),
m_pFPane(NULL),		
m_pSPane(NULL),		
m_nFConstraint(100),		
m_nSConstraint(100),			
m_bVertSplitter(TRUE),	
m_SplitterPos(0),
m_bDragging(FALSE),
m_BarPos(0),
m_DrawedBarPos(0),
m_hwndPrevFocus(0),
m_nSplitterWidth(3),
m_nID(0),
m_bLiveUpdate(FALSE), 
//m_fixedSplitterPos(-1),
m_mode(MOD_Normal),
m_bTracking(FALSE)
{
	SetColor(COL_Normal, GetSysColor(COLOR_BTNFACE));
	SetColor(COL_Disabled, GetSysColor(COLOR_INACTIVECAPTION));
	SetColor(COL_Tracking, GetSysColor(COLOR_BTNHIGHLIGHT));
}

CFastSplitter::~CFastSplitter()
{
}


BEGIN_MESSAGE_MAP(CFastSplitter, CWnd)//CStatic)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFastSplitter message handlers

BOOL CFastSplitter::Create(
			DWORD dwStyle, 
			CWnd* pParentWnd, 
			CWnd* pFPane, 
			CWnd* pSPane,
			UINT nID,
			UINT nFConstraint,
			UINT nSConstraint,
			BOOL bVerticalSplitter,
			UINT SplitterWidth)
{
	ASSERT(pFPane != NULL);
	ASSERT(pSPane != NULL);
	ASSERT(pParentWnd != NULL);
	ASSERT(nID > 0);
	m_nID  = nID;
	m_pFPane = pFPane;
	m_pSPane = pSPane;
	m_nFConstraint = nFConstraint;
	m_nSConstraint = nSConstraint;
	m_pWndParent = pParentWnd;
	m_bVertSplitter = bVerticalSplitter;
	m_nSplitterWidth = SplitterWidth;
	m_SplitterPos = 2 * m_nFConstraint;
	return CWnd::Create(NULL, NULL, dwStyle, CRect(0,0,0,0), pParentWnd, nID);
}


BOOL CFastSplitter::Create(
						   DWORD dwStyle, 
						   CWnd* pParentWnd, 
						   UINT nID)
{
	ASSERT(pParentWnd != NULL);
	ASSERT(nID > 0);
	m_nID  = nID;
	m_pWndParent = pParentWnd;
	return CWnd::Create(NULL, NULL, dwStyle, CRect(0,0,0,0), pParentWnd, nID);
}


void CFastSplitter::OnSize(UINT nType, int cx, int cy) 
{
	if (m_hWnd && cx>0 && cy>0)
		MovePanes();
}

void CFastSplitter::MovePanes()
{
	if (m_hWnd == 0)
		return;
	ASSERT(m_mode < MOD_Last);

	CRect rect;
	GetClientRect(&rect);//GetRelativeRect());
	m_RealSplitterPos = m_SplitterPos;
	ApplyConstraints(m_RealSplitterPos, rect.Width(), rect.Height());
	CRect firstRc(rect);
	CRect secondRc(rect);
	m_SplitterRect.SetRect(0,0,0,0);
	if (!m_bVertSplitter)
	{
		switch (m_mode)
		{
		case MOD_Normal:
			firstRc.bottom = firstRc.top + m_RealSplitterPos;
			secondRc.top = firstRc.bottom + m_nSplitterWidth;
			m_SplitterRect.SetRect(rect.left, firstRc.bottom, rect.right, secondRc.top - 1);
			break;
		case MOD_FirstMaximized:
			firstRc = rect;
			secondRc.top = secondRc.bottom;
			break;
		case MOD_SecondMaximized:
			firstRc.bottom = firstRc.top;
			secondRc = rect;
			break;
		}
	}
	else
	{
		switch (m_mode)
		{
		case MOD_Normal:
			firstRc.right = firstRc.left + m_RealSplitterPos;
			secondRc.left = firstRc.right + m_nSplitterWidth;
			m_SplitterRect.SetRect(firstRc.right, rect.top, secondRc.left - 1, rect.bottom);
			break;
		case MOD_FirstMaximized:
			firstRc = rect;
			secondRc.left = secondRc.right;
			break;
		case MOD_SecondMaximized:
			firstRc.right = firstRc.left;
			secondRc = rect;
			break;
		}
	}
	if (m_pFPane != NULL)
	{
		m_pFPane->SetParent(this);
		if (firstRc.Width() > 0 && firstRc.Height() > 0)
		{
			CRect rc;
			m_pFPane->GetWindowRect(&rc);
			ScreenToClient(&rc);
			if (rc != firstRc || !m_pFPane->IsWindowVisible())
				m_pFPane->SetWindowPos(0, firstRc.left, firstRc.top, firstRc.Width(), firstRc.Height(), SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
		}
		else
			m_pFPane->ShowWindow(SW_HIDE);
	}
	if (m_pFPane != NULL)
	{
		m_pSPane->SetParent(this);
		if (secondRc.Width() > 0 && secondRc.Height() > 0)
		{
			CRect rc;
			m_pSPane->GetWindowRect(&rc);
			ScreenToClient(&rc);
			if (rc != secondRc || !m_pSPane->IsWindowVisible())
				m_pSPane->SetWindowPos(0, secondRc.left, secondRc.top, secondRc.Width(), secondRc.Height(), SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW);
		}
		else
			m_pSPane->ShowWindow(SW_HIDE);
	}
}

void CFastSplitter::ApplyConstraints(UINT& SplitterPos, UINT Width, UINT Height)
{
	if (SplitterPos < m_nFConstraint)
		SplitterPos = m_nFConstraint;
	if (m_bVertSplitter)
	{
		if (SplitterPos > Width - m_nSConstraint)
			SplitterPos = Width - m_nSConstraint;
	}
	else
	{
		if (SplitterPos > Height - m_nSConstraint)
			SplitterPos = Height - m_nSConstraint;
	}
}



BOOL CFastSplitter::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= SS_NOTIFY; // to notify its parent of mouse events	
	return CWnd::PreCreateWindow(cs);
}

CRect CFastSplitter::GetRelativeRect()
{
	CRect parentRC, thisRC;
	m_pWndParent->GetClientRect(&parentRC);
	m_pWndParent->ClientToScreen(&parentRC);
	GetClientRect(&thisRC);
	ClientToScreen(&thisRC);
	CRect rect;
	rect.left = thisRC.left - parentRC.left;
	rect.top = thisRC.top - parentRC.top;
	rect.right = rect.left + thisRC.Width();
	rect.bottom = rect.top + thisRC.Height();
	return rect;

}

CRect CFastSplitter::GetSplitterRect(UINT SplitterPos, BOOL bWindowRelative/* = FALSE*/)
{
	CRect ret(m_SplitterRect);
	INT offset = SplitterPos - m_RealSplitterPos;
	if (offset != 0)
	{
		if (m_bVertSplitter)
			ret.OffsetRect(offset, 0);
		else
			ret.OffsetRect(0, offset);
	}

	if (!bWindowRelative)
	{
		CRect parentRC, thisRC;
		m_pWndParent->GetClientRect(&parentRC);
		m_pWndParent->ClientToScreen(&parentRC);
		GetClientRect(&thisRC);
		ClientToScreen(&thisRC);
		int xOff = thisRC.left - parentRC.left;
		int yOff = thisRC.top - parentRC.top;
		ret.OffsetRect(-xOff, -yOff);
	}
	return ret;
}

void CFastSplitter::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_mode == MOD_Normal)
	{
		//CRect splRc = GetSplitterRect(m_SplitterPos);
		if (m_SplitterRect.PtInRect(point))
		{
			m_bDragging = TRUE;
			m_DragStartPoint = point;
			m_BarPos = m_RealSplitterPos;
			if ( !m_bLiveUpdate)
				DrawBar();
			SetCapture();
			m_hwndPrevFocus = ::SetFocus(m_hWnd);  
			::SetCursor( AfxGetApp()->LoadStandardCursor (LPCTSTR (m_bVertSplitter ? IDC_SIZEWE : IDC_SIZENS)));
		}
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CFastSplitter::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_mode == MOD_Normal)
	{
		if (m_bDragging == TRUE)
		{
			m_bDragging = FALSE;
			if ( !m_bLiveUpdate )
			{
				DrawBar(TRUE);	 
				if (m_bVertSplitter)
					m_SplitterPos = m_RealSplitterPos + point.x - m_DragStartPoint.x;
				else
					m_SplitterPos = m_RealSplitterPos + point.y - m_DragStartPoint.y;
				MovePanes();
			}
			ReleaseCapture();
			::SetFocus(m_hwndPrevFocus);
		}
	}
	CWnd::OnLButtonUp(nFlags, point);
}

void CFastSplitter::OnMouseMove(UINT nFlags, CPoint point) 
{

	if (m_mode == MOD_Normal)
	{
		if (m_bDragging || m_SplitterRect.PtInRect(point))
		{
			::SetCursor( AfxGetApp()->LoadStandardCursor(LPCTSTR (m_bVertSplitter ? IDC_SIZEWE : IDC_SIZENS)));
			if (m_bDragging)
			{
				if (m_bLiveUpdate)
				{
					if (m_bVertSplitter)
						m_SplitterPos = m_BarPos + point.x - m_DragStartPoint.x;
					else
						m_SplitterPos = m_BarPos + point.y - m_DragStartPoint.y;
					MovePanes();
				}
				else
				{
					if (m_bVertSplitter)
						m_BarPos = m_RealSplitterPos + point.x - m_DragStartPoint.x;
					else
						m_BarPos = m_RealSplitterPos + point.y - m_DragStartPoint.y;
					DrawBar();	
				}
			}
			else if (!m_bTracking)
			{
				//TRACE(_T("CFastSplitter::OnMouseMove !m_bTracking\r\n"));
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(tme);
				tme.hwndTrack = m_hWnd;
				tme.dwFlags = TME_LEAVE;
				tme.dwHoverTime = HOVER_DEFAULT;
				m_bTracking = _TrackMouseEvent(&tme);

				HWND hWnd = GetSafeHwnd();
				if ((hWnd != NULL) && ::IsWindow (hWnd))
					InvalidateRect(&m_SplitterRect);
			}
		}
		else
		{
			//TRACE(_T("CFastSplitter::OnMouseMove !m_bDragging !m_bTracking\r\n"));

		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CFastSplitter::DrawBar(BOOL bEraseOnly)
{
	CClientDC dc(this);
	CBrush* pOldBrush = dc.SelectObject(&m_Brushes[m_mode == MOD_Normal ? COL_Normal : COL_Disabled]);
	//TRACE(_T("@1Split: %d Bar: %d oldBarPos: %d"), m_SplitterPos, m_BarPos, m_DrawedBarPos);

	if (m_DrawedBarPos > 0)//Erase the previous
	{
		//TRACE(_T("@1Erasing %d"), m_DrawedBarPos);

		CRect rc = GetSplitterRect(m_DrawedBarPos, TRUE);
		//ClientToScreen(&rc);
		//ClientToWindowRect(m_pWndParent->m_hWnd, &rc);
		//rc = GetSplitterRect(m_DrawedBarPos, TRUE);
		dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATINVERT);
	}
	if (bEraseOnly)
		m_DrawedBarPos = 0;
	else
	{
		//TRACE(_T("@1Drawing %d"), m_BarPos);
		//CRect rc = GetSplitterRect(m_BarPos, TRUE);
		CRect rc = GetSplitterRect(m_BarPos, TRUE);
		//ClientToScreen(&rc);
		//ClientToWindowRect(m_pWndParent->m_hWnd, &rc);
		dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATINVERT);
		m_DrawedBarPos = m_BarPos;
	}
	dc.SelectObject(pOldBrush);	
}

void CFastSplitter::CancelDrag()
{
	DrawBar(TRUE);								
	ReleaseCapture();						
	::SetFocus(m_hwndPrevFocus);		
	m_bDragging = FALSE;
}

BOOL CFastSplitter::PreTranslateMessage(MSG* pMsg) 
{
	if ( (pMsg->message == WM_KEYDOWN)&&( pMsg->wParam == VK_ESCAPE ))
		CancelDrag();
	return CWnd::PreTranslateMessage(pMsg);
}

void CFastSplitter::OnDestroy() 
{
	CWnd::OnDestroy();
}


void CFastSplitter::SetMode(MODE mode)
{
	if (mode != m_mode)
	{
		m_mode = mode;
		MovePanes();
	}
}

void CFastSplitter::SetSplitterPos(UINT nPos)
{
	m_SplitterPos = nPos;
	m_RealSplitterPos = nPos;
	MovePanes();
}
 
BOOL CFastSplitter::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


void CFastSplitter::OnPaint()
{
	{
		CPaintDC _dc(this); // device context for painting
	}
	CClientDC dc(this);
	COLORREF color = m_colors[COL_Disabled];
	if (m_mode == MOD_Normal)
		color = m_colors[m_bTracking ? COL_Tracking : COL_Normal];
	//===Paint #1
	//HBRUSH br = CreateSolidBrush(color);
	//HBRUSH oldBr = (HBRUSH)::SelectObject(dc.m_hDC, br);
	//dc.Rectangle(&GetSplitterRect(m_SplitterPos));
	//::SelectObject(dc.m_hDC, oldBr);
	//===Paint #2
	CRect rc = m_SplitterRect;//GetSplitterRect(m_SplitterPos);
	dc.FillSolidRect(&rc, color);
	HPEN oldPen = (HPEN)::SelectObject(dc.m_hDC, GetStockObject(DC_PEN));
	::SetDCPenColor(dc.m_hDC, RGB(128,128,128));
	if (m_bVertSplitter)
	{
		dc.MoveTo(rc.left, rc.top);
		dc.LineTo(rc.left, rc.bottom);
		dc.MoveTo(rc.right, rc.top);
		dc.LineTo(rc.right, rc.bottom);
	}
	else
	{
		dc.MoveTo(rc.left, rc.top);
		dc.LineTo(rc.right, rc.top);
		dc.MoveTo(rc.left, rc.bottom);
		dc.LineTo(rc.right, rc.bottom);
	}
	::SelectObject(dc.m_hDC, oldPen);

	//dc.FillSolidRect(&GetSplitterRect(m_SplitterPos), color);

}

void CFastSplitter::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);
	//MovePanes();
}

//---------------------------------------------------
//ICtrlColors ---------------------------------------
//---------------------------------------------------
void CFastSplitter::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
	{
		m_colors[idx] = value;
		if (m_Brushes[idx].m_hObject)
			m_Brushes[idx].DeleteObject();
		m_Brushes[idx].CreateSolidBrush(value);
		if (m_hWnd)		Invalidate();
	}
}
COLORREF CFastSplitter::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}
//const TCHAR* const cColorNames[] = {
//	_T("Normal"),
//	_T("Disabled"),
//	_T("Tracking"),
//	_T("Text"),
//	NULL
//};
//
//
//LPCTSTR CFastSplitter::GetColorName(UINT idx)
//{
//	ASSERT(idx <= COL_Last);
//	if (idx < COL_Last)
//		return cColorNames[idx];
//	return NULL;
//}
//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------


LRESULT CFastSplitter::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	// To ignore "unreferenced formal parameter" warning
	wParam = wParam;
	lParam = lParam;

	m_bTracking = false;

	HWND hWnd = GetSafeHwnd();
	if ((hWnd != NULL) && ::IsWindow (hWnd))
		InvalidateRect(&m_SplitterRect);
	return 0;
}
