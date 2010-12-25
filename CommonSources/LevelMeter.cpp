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
#include "LevelMeter.h"

#define TIMER_RESETMAXLEVEL 100
#define TIMER_RESETMAXLEVEL_DELAY 1000

//#define COLORS_3PART_MODE

// CLevelMeter

IMPLEMENT_DYNAMIC(CLevelMeter, CWnd)

CLevelMeter::CLevelMeter():
m_minVal(0),
m_maxVal(100),
m_curVal(0),
m_bShowLines(TRUE),
m_bShowBorder(TRUE)
{
	m_colors[COL_Bk] = RGB(0,0,0);
	m_colors[COL_ActiveLow] = RGB(0,200,0);
	m_colors[COL_ActiveMid] = RGB(200,200,0);
	m_colors[COL_ActiveHigh] = RGB(200,0,0);
	m_colors[COL_Border] = RGB(128,128,128);
	m_colors[COL_Lines] = RGB(128,128,128);

}


CLevelMeter::~CLevelMeter()
{
}


BEGIN_MESSAGE_MAP(CLevelMeter, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CLevelMeter::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::Create(_T("STATIC"), NULL, dwStyle, rect, pParentWnd, nID);
}


// CLevelMeter message handlers



BOOL CLevelMeter::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}

void CLevelMeter::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rc;
	GetClientRect(&rc);

	CPen* oldPen = (CPen*)dc.SelectStockObject(DC_PEN);
	CBrush* oldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	if (m_bShowBorder)
	{
		dc.SetDCPenColor(m_colors[COL_Border]);
		dc.Rectangle(&rc);
		rc.InflateRect(-1,-1,-1,-1);
	}

	INT levPoint = ((m_curVal - m_minVal) * rc.Height() / (m_maxVal - m_minVal));
	INT maxLevPoint = ((m_maxLevel - m_minVal) * rc.Height() / (m_maxVal - m_minVal));
	//if (levPoint != rc.Height())
	//{
	//	CRect rcDraw(rc);
	//	rcDraw.bottom -= levPoint;
	//	dc.FillSolidRect(&rcDraw, m_colors[COL_Bk]);
	//}
	dc.FillSolidRect(&rc, m_colors[COL_Bk]);
	if (m_bShowLines)
	{
		dc.SetDCPenColor(m_colors[COL_Lines]);
		for (INT i = 1; i < 7; i++)
		{
			dc.MoveTo(rc.left + 2, i * rc.Height() / 7 + 1);	
			dc.LineTo(rc.left + rc.Width() - 2, i * rc.Height() / 7 + 1);	
		}
	}
	INT pLow = 0;
	INT pLowMid = 4 * rc.Height() / 7;
	INT pMidHigh = 6 * rc.Height() / 7;
	INT pHigh = rc.Height();
#ifdef COLORS_3PART_MODE


	if (levPoint > pLow)//Draw Low Bar
	{
		CRect rcDraw(rc);
		rcDraw.top += pHigh - min(pLowMid,levPoint);
		dc.FillSolidRect(&rcDraw, m_colors[COL_ActiveLow]);
	}
	if (levPoint > pLowMid)//Draw Mid Bar
	{
		CRect rcDraw(rc);
		rcDraw.top += pHigh - min(pMidHigh,levPoint);
		rcDraw.bottom -= pLowMid;
		dc.FillSolidRect(&rcDraw, m_colors[COL_ActiveMid]);
	}
	if (levPoint > pMidHigh)//Draw Mid Bar
	{
		CRect rcDraw(rc);
		rcDraw.top += pHigh - min(pHigh, levPoint);
		rcDraw.bottom -= pMidHigh;
		dc.FillSolidRect(&rcDraw, m_colors[COL_ActiveHigh]);
	}


#else
	dc.SetDCPenColor(m_colors[COL_ActiveLow]);
	for (INT i = 0; i < levPoint; i+=2)
	{
		INT pos = rc.Height() - i - 1;
		dc.MoveTo(rc.left, pos);
		dc.LineTo(rc.right, pos);
	}

#endif
	if (maxLevPoint > 0)
	{
		INT color = COL_ActiveLow;
		if (maxLevPoint > pMidHigh)
			color = COL_ActiveHigh;
		else if (maxLevPoint > pLowMid)
			color = COL_ActiveMid;
		CRect rcDraw(rc);
		rcDraw.top += pHigh - maxLevPoint;
		rcDraw.bottom = rcDraw.top + 1;
		dc.FillSolidRect(&rcDraw, m_colors[color]);
	}


	dc.SelectObject(oldPen);
	dc.SelectObject(oldBrush);


	//rcDraw.bottom = rc.Height() - levPoint - 1;
	//dc.FillSolidRect(&rcDraw, m_colors[COL_Bk]);
	//
	//if (rc.bottom != levPoint)
	//{
	//	if (levPoint < rc.Width()/5)
	//	{
	//		CRect rcDraw(rc);
	//		rc.top = levPoint;
	//		rc.bottom = rc.Width()/5;
	//		dc.FillSolidRect(&rc, m_colors[COL_ActiveHigh]);
	//	}
	//	if (levPoint < 2*rc.Width()/5)
	//	{
	//		CRect rcDraw(rc);
	//		rc.top = max(;
	//		rc.bottom = rc.Width()/5;
	//		dc.FillSolidRect(&rc, m_colors[COL_ActiveHigh]);
	//	}


	//	rc.top = levPoint;
	//	dc.FillSolidRect(&rc, m_colors[COL_Active]);
	//}
}

void CLevelMeter::SetMaxMin(INT maxVal, INT minVal)	
{
	ASSERT(maxVal > minVal);
	if (maxVal > minVal)
	{
		m_maxVal = maxVal; 
		m_minVal = minVal;
		SetLevel(GetLevel());	//Re-evaluates the value
	}
}
void CLevelMeter::SetLevel(INT value)				
{
	ASSERT(value >= m_minVal && value <= m_maxVal);
	INT oldValue = m_curVal;
	m_curVal = value;
	if (value < m_minVal)
		m_curVal = m_minVal;
	else if (value > m_maxVal)
		m_curVal = m_maxVal;
	if (m_curVal > m_maxLevel)
	{
		KillTimer(TIMER_RESETMAXLEVEL);
		SetTimer(TIMER_RESETMAXLEVEL, TIMER_RESETMAXLEVEL_DELAY, NULL);
		m_maxLevel = m_curVal;
	}
	if (m_curVal != oldValue && m_hWnd)
		Invalidate(FALSE);
}


void CLevelMeter::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == TIMER_RESETMAXLEVEL)
	{
		m_maxLevel = m_curVal;
		Invalidate(FALSE);
	}

	CWnd::OnTimer(nIDEvent);
}

void CLevelMeter::SetColor(COLORS col, COLORREF value)
{
	ASSERT(col < COL_Last);
	if (col < COL_Last)
	{
		m_colors[col] = value;
		Invalidate();
	}
}
COLORREF CLevelMeter::GetColor(COLORS col)
{
	ASSERT(col < COL_Last);
	if (col < COL_Last)
		return m_colors[col];
	return 0;
}

