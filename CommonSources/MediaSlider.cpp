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
#include "MediaSlider.h"

COLORREF CMediaSlider::MixColors(COLORREF col1, COLORREF col2, INT part1/* = 1*/, INT part2/* = 1*/)
{
	return RGB( 
		(part1 * GetRValue(col1) + part2 * GetRValue(col2)) / (part1 + part2),
		(part1 * GetGValue(col1) + part2 * GetGValue(col2)) / (part1 + part2),
		(part1 * GetBValue(col1) + part2 * GetBValue(col2)) / (part1 + part2)
		);
}

// CMediaSlider

IMPLEMENT_DYNAMIC(CMediaSlider, CWnd)

CMediaSlider::CMediaSlider():
m_maxPos(100),
m_curPos(0),
m_cursorWidth(9),
m_bHorizontal(TRUE),
m_bMouseIsDown(FALSE),
m_bUseHandCursor(TRUE),
m_pListener(NULL),
m_tickInterval(0),
m_bTransparentMode(FALSE)
{
	m_colors[COL_Bk] = RGB(57,57,57);
	m_colors[COL_Border] = RGB(28,28,28);
	m_colors[COL_FirstPart] = RGB(255,192,0);
	m_colors[COL_SecondPart] = RGB(255,255,0);
	m_colors[COL_CursorBorder] = RGB(0,0,0);
	m_colors[COL_TickColor] = RGB(128,128,128);
}


CMediaSlider::~CMediaSlider()
{
}


BEGIN_MESSAGE_MAP(CMediaSlider, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()



// CMediaSlider message handlers

BOOL CMediaSlider::Create(MediaSliderListener* pListener, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	SetListener(pListener);
	return CWnd::Create(_T("STATIC"), NULL, dwStyle | SS_NOTIFY | SS_OWNERDRAW, rect, pParentWnd, nID);
}


BOOL CMediaSlider::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}
const INT cMargin = 2;

void CMediaSlider::OnPaint()
{
	CPaintDC _dc(this); // device context for painting
	CClientDC dc(this); // There are artifacts while resizing if i don't use this
	CRect rc;
	GetClientRect(&rc);
	if (!m_bTransparentMode)
		dc.FillSolidRect(&rc, m_colors[COL_Bk]);
	CPen* oldPen = (CPen*)dc.SelectStockObject(DC_PEN);
	INT distFromCenter = (m_cursorWidth - 1) / 2;
	COLORREF midColor = MixColors(m_colors[COL_Border], m_colors[COL_FirstPart]);

	if (m_bHorizontal)
	{

		//=== Draw the border
		INT centerLineY = rc.Height() / 2;
		dc.SetDCPenColor(m_colors[COL_Border]);
		POINT pBorder[] = {
			{cMargin + 1,				centerLineY - 2},
			{rc.right - cMargin - 2,	centerLineY - 2},
			{rc.right - cMargin - 1,	centerLineY - 1},
			{rc.right - cMargin - 1,	centerLineY + 1},
			{rc.right - cMargin - 2,	centerLineY + 2},
			{cMargin + 1,				centerLineY + 2},
			{cMargin,					centerLineY + 1},
			{cMargin,					centerLineY - 1},
			{cMargin - 1,				centerLineY - 2}
		};
		dc.Polyline(pBorder, 9);



		dc.SetDCPenColor(midColor);
		//Draw Up MidLine
		dc.MoveTo(cMargin + 1,				centerLineY - 1);
		dc.LineTo(rc.right - cMargin - 1,	centerLineY - 1);
		//Draw Down MidLine
		dc.MoveTo(cMargin + 1,				centerLineY + 1);
		dc.LineTo(rc.right - cMargin - 1,	centerLineY + 1);


		INT startX = cMargin + 1 + distFromCenter;
		INT endX = rc.right - startX ;
		INT availablePixelsForSlidinig = endX - startX + 1;

		INT cursorX = startX + INT(availablePixelsForSlidinig * (DOUBLE)(m_curPos / (DOUBLE)m_maxPos));

		//Draw First Part
		if (cursorX > cMargin + distFromCenter + 1)
		{
			dc.SetDCPenColor(m_colors[COL_FirstPart]);
			dc.MoveTo(cMargin + 1,				centerLineY);
			dc.LineTo(cursorX - distFromCenter,	centerLineY);
		}

		//Draw Second Part
		if (rc.right - cMargin - 1 > cursorX + m_cursorWidth)
		{
			dc.SetDCPenColor(m_colors[COL_SecondPart]);
			dc.MoveTo(cursorX + distFromCenter + 1,	centerLineY);
			dc.LineTo(rc.right - cMargin - 1,		centerLineY);
		}

		//Draw Ticks
		if (m_tickInterval > 0)
		{
			dc.SetDCPenColor(m_colors[COL_TickColor]);
			for (INT i = m_tickInterval; i < m_maxPos; i += m_tickInterval)
			{
				INT xPos = cMargin + 1 + (i * (rc.Width() - 2 * cMargin - 2)) / m_maxPos;
				dc.MoveTo(xPos,	centerLineY - 0);
				dc.LineTo(xPos, centerLineY + 3);
			}
		}


		//DrawCursor Border
		dc.SetDCPenColor(m_colors[COL_CursorBorder]);
		POINT pNormalCursorBorder[] = {
			{cursorX - distFromCenter - 1,	centerLineY - 3},
			{cursorX + distFromCenter + 1,	centerLineY - 3},
			{cursorX + distFromCenter + 3,	centerLineY - 1},
			{cursorX + distFromCenter + 3,	centerLineY + 1},
			{cursorX + distFromCenter + 1,	centerLineY + 3},
			{cursorX - distFromCenter - 1,	centerLineY + 3},
			{cursorX - distFromCenter - 3,	centerLineY + 1},
			{cursorX - distFromCenter - 3,	centerLineY - 1},
			{cursorX - distFromCenter - 1,	centerLineY - 3}
		};

		POINT pReducedCursorBorder[] = {
			{cursorX - distFromCenter - 0,	centerLineY - 2},
			{cursorX + distFromCenter + 0,	centerLineY - 2},
			{cursorX + distFromCenter + 2,	centerLineY - 1},
			{cursorX + distFromCenter + 2,	centerLineY + 1},
			{cursorX + distFromCenter + 0,	centerLineY + 2},
			{cursorX - distFromCenter - 0,	centerLineY + 2},
			{cursorX - distFromCenter - 2,	centerLineY + 1},
			{cursorX - distFromCenter - 2,	centerLineY - 1},
			{cursorX - distFromCenter - 0,	centerLineY - 2}
		};


		LPPOINT pCursorBorder = m_bTransparentMode ? pReducedCursorBorder : pNormalCursorBorder;


		dc.Polyline(pCursorBorder, 9);

		COLORREF tmpColor = MixColors(m_colors[COL_FirstPart], RGB(255,255,255));
		dc.FillSolidRect(cursorX - distFromCenter - 2, centerLineY - 1, m_cursorWidth + 4, 3, tmpColor);
		tmpColor = MixColors(m_colors[COL_FirstPart], RGB(255,255,255), 1, 5);
		dc.SetDCPenColor(tmpColor);
		dc.MoveTo(cursorX - distFromCenter - 1, centerLineY - 2);
		dc.LineTo(cursorX + distFromCenter + 2, centerLineY - 2);
		//tmpColor = MixColors(m_colors[COL_FirstPart], RGB(0,0,0), 1, 1);
		dc.SetDCPenColor(m_colors[COL_FirstPart]);
		dc.MoveTo(cursorX - distFromCenter - 1, centerLineY + 2);
		dc.LineTo(cursorX + distFromCenter + 2, centerLineY + 2);

		tmpColor = MixColors(m_colors[COL_FirstPart], RGB(255,0,0), 2, 3);
		dc.SetDCPenColor(tmpColor);
		dc.MoveTo(cursorX - distFromCenter, centerLineY);
		dc.LineTo(cursorX + distFromCenter + 1, centerLineY);

	}
	else
	{

		INT centerLineX = rc.Width() / 2;
		dc.SetDCPenColor(m_colors[COL_Border]);
		POINT pBorder[] = {
			{centerLineX - 1,			cMargin + 0},
			{centerLineX + 1,			cMargin + 0},
			{centerLineX + 2,			cMargin + 1},
			{centerLineX + 2,			rc.bottom - cMargin - 2},
			{centerLineX + 1,			rc.bottom - cMargin - 1},
			{centerLineX - 1,			rc.bottom - cMargin - 1},
			{centerLineX - 2,			rc.bottom - cMargin - 2},
			{centerLineX - 2,			cMargin + 1},
			{centerLineX - 1,			cMargin + 0}
		};
		dc.Polyline(pBorder, 9);



		dc.SetDCPenColor(midColor);
		//Draw Up MidLine
		dc.MoveTo(centerLineX - 1,			cMargin + 1);
		dc.LineTo(centerLineX - 1,			rc.bottom - cMargin - 1);
		//Draw Down MidLine
		dc.MoveTo(centerLineX + 1,			cMargin + 1);
		dc.LineTo(centerLineX + 1,			rc.bottom - cMargin - 1);


		INT startY = cMargin + 1 + distFromCenter;
		INT endY = rc.bottom - startY;
		INT availablePixelsForSlidinig = endY - startY + 1;

		INT cursorY = startY + (m_curPos * availablePixelsForSlidinig) / m_maxPos;

		//Draw First Part
		if (cursorY - distFromCenter > cMargin + 1)
		{
			dc.SetDCPenColor(m_colors[COL_FirstPart]);
			dc.MoveTo(centerLineX, cMargin + 1);
			dc.LineTo(centerLineX, cursorY - distFromCenter);
		}

		//m_tickInterval

		//Draw Second Part
		if (rc.bottom - cMargin - 1 > cursorY + distFromCenter + 1)
		{
			dc.SetDCPenColor(m_colors[COL_SecondPart]);
			dc.MoveTo(centerLineX, cursorY + distFromCenter + 1);
			dc.LineTo(centerLineX, rc.bottom - cMargin - 1);
		}

		//DrawCursor Border
		dc.SetDCPenColor(m_colors[COL_CursorBorder]);
		POINT pCursorBorder[] = {
			{centerLineX - 1, cursorY - distFromCenter - 3	},
			{centerLineX + 1, cursorY - distFromCenter - 3	},
			{centerLineX + 3, cursorY - distFromCenter - 1	},
			{centerLineX + 3, cursorY + distFromCenter + 1	},
			{centerLineX + 1, cursorY + distFromCenter + 3	},
			{centerLineX - 1, cursorY + distFromCenter + 3	},
			{centerLineX - 3, cursorY + distFromCenter + 1	},
			{centerLineX - 3, cursorY - distFromCenter - 1	},
			{centerLineX - 1, cursorY - distFromCenter - 3	}
		};
		dc.Polyline(pCursorBorder, 9);

		COLORREF tmpColor = MixColors(m_colors[COL_FirstPart], RGB(255,255,255));
		dc.FillSolidRect(centerLineX - 2,  cursorY - distFromCenter - 1, 5, m_cursorWidth + 2, tmpColor);
		tmpColor = MixColors(m_colors[COL_FirstPart], RGB(255,255,255), 1, 5);
		dc.SetDCPenColor(tmpColor);
		dc.MoveTo(centerLineX - 1, cursorY - distFromCenter - 2);
		dc.LineTo(centerLineX + 2, cursorY - distFromCenter - 2);
		//tmpColor = MixColors(m_colors[COL_FirstPart], RGB(0,0,0), 1, 1);
		dc.SetDCPenColor(m_colors[COL_FirstPart]);
		dc.MoveTo(centerLineX - 1, cursorY + distFromCenter + 2);
		dc.LineTo(centerLineX + 2, cursorY + distFromCenter + 2);

		tmpColor = MixColors(m_colors[COL_FirstPart], RGB(255,0,0), 2, 3);
		dc.SetDCPenColor(tmpColor);
		dc.MoveTo(centerLineX, cursorY - distFromCenter);
		dc.LineTo(centerLineX, cursorY + distFromCenter + 1);



	}
	dc.SelectObject(oldPen);
}



void CMediaSlider::SetMaxPos(INT maxPos)	
{
	if (maxPos < 1)
		maxPos = 1;
	m_maxPos = maxPos;
	SetPos(GetPos());
}
void CMediaSlider::SetPos(INT value)				
{
	ASSERT(value >= 0);// && value <= m_maxPos);
	INT oldValue = m_curPos;
	m_curPos = value;
	if (value < 0)
		m_curPos = 0;
	else if (value > m_maxPos)
		m_curPos = m_maxPos;
	if (m_curPos != oldValue && m_hWnd)
		Invalidate(FALSE);
}


void CMediaSlider::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (m_bMouseIsDown)
	{
		INT newPos = GetPosByPoint(point.x, point.y);
		if (newPos != -1)
		{
			SetPos(GetPosByPoint(point.x, point.y));
			if (m_pListener != NULL)
				m_pListener->OnMediaSliderClick(this);
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

INT CMediaSlider::GetPosByPoint(INT x, INT y)
{
	CRect rc;
	GetClientRect(&rc);
	if (m_bHorizontal)
	{
		INT startX = cMargin + 1 + (m_cursorWidth - 1) / 2;
		INT endX = rc.right - cMargin - 1 - (m_cursorWidth - 1) / 2;
		if (x < startX)
			x = startX;
		if (x > endX)
			x = endX;
		INT availablePixelsForSlidinig = endX - startX + 1;
		return INT((x - startX) * DOUBLE(m_maxPos / (DOUBLE)availablePixelsForSlidinig));
	}
	//Vertical
	INT startY = cMargin + 1 + (m_cursorWidth - 1) / 2;
	INT endY = rc.bottom - cMargin - 1 - (m_cursorWidth - 1) / 2;
	if (y < startY)
		y = startY;
	if (y > endY)
		y = endY;
	INT availablePixelsForSlidinig = endY - startY + 1;
	return INT((y - startY) * DOUBLE(m_maxPos / (DOUBLE)availablePixelsForSlidinig));


}

void CMediaSlider::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bMouseIsDown = TRUE;
	if (::GetCapture() != m_hWnd)
	{
		SetCapture();
		//TRACE(_T("SetCapture\r\n"));
	}
	INT newPos = GetPosByPoint(point.x, point.y);
	if (newPos != -1)
	{
		SetPos(GetPosByPoint(point.x, point.y));
		if (m_pListener != NULL)
			m_pListener->OnMediaSliderClick(this);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CMediaSlider::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bMouseIsDown = FALSE;
	if (::GetCapture() == m_hWnd)
	{
		ReleaseCapture();
		//TRACE(_T("ReleaseCapture\r\n"));
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CMediaSlider::SetColor(CMediaSlider::COLORS idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
	{
		m_colors[idx] = value;
		Invalidate();
	}
}
COLORREF CMediaSlider::GetColor(CMediaSlider::COLORS idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}

BOOL CMediaSlider::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_bUseHandCursor)
	{
		SetCursor(AfxGetApp()->LoadStandardCursor (IDC_HAND));
		return TRUE;//__super::OnSetCursor(pWnd, nHitTest, message);
	}
	return __super::OnSetCursor(pWnd, nHitTest, message);
}
