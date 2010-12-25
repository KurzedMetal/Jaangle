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
#include "MillionaireButton.h"


// CMillionaireButton

IMPLEMENT_DYNAMIC(CMillionaireButton, CButton)

CMillionaireButton::CMillionaireButton():
m_ps(POL_RoundRect),
m_psxParam(0),
m_currentState(ST_Normal),
m_pFont(NULL),
m_textAlign(DT_CENTER),
m_margin(0)
{
	COLORREF txtColor = RGB(255,255,255);

	m_colors[COL_Frame] = RGB(128,128,128);

	m_colors[COL_NormalBk] = RGB(50,50,100);
	m_colors[COL_OverBk] = RGB(200, 200, 100);
	m_colors[COL_CorrectBk] = RGB(100,200,100);
	m_colors[COL_WrongBk] = RGB(200,100,100);
	m_colors[COL_DisabledBk] = RGB(50,50,50);

	m_colors[COL_NormalText] = txtColor;
	m_colors[COL_OverText] = txtColor;
	m_colors[COL_CorrectText] = txtColor;
	m_colors[COL_WrongText] = txtColor;
	m_colors[COL_DisabledText] = txtColor;
	//m_blinkState	= FALSE;

}

CMillionaireButton::~CMillionaireButton()
{
}


BEGIN_MESSAGE_MAP(CMillionaireButton, CButton)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CMillionaireButton message handlers


void CMillionaireButton::SetPolygonStyle(PolygonStyles ps, INT psxParam)
{
	ASSERT(ps < POL_Last);
	m_ps = ps;
	psxParam = m_psxParam;
	if (m_hWnd != NULL)
	{
		CRect rc;
		GetClientRect(&rc);
		UpdateWindowRegion(rc.Width(), rc.Height());
	}
}



BOOL CMillionaireButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;//CButton::OnEraseBkgnd(pDC);
}

//void CMillionaireButton::OnPaint()
//{
//	CPaintDC dc(this); // device context for painting
//
//	CRect rc;
//	GetClientRect(&rc);
//	CPen* oldPen = (CPen*)dc.SelectStockObject(DC_PEN);
//	CBrush* oldBrush = (CBrush*)dc.SelectStockObject(DC_BRUSH);
//	dc.SetDCPenColor(m_borderColor);
//	if (m_blinkState)
//		dc.SetDCBrushColor(m_colors[ST_Normal]);
//	else
//		dc.SetDCBrushColor(m_colors[m_currentState]);
//	dc.Rectangle(&rc);
//	dc.SetBkMode(TRANSPARENT);
//	dc.SetTextColor(m_textColor);
//	CFont* oldFont = (CFont*) dc.SelectObject(GetFont());
//	GetWindowText(m_windowText);
//	dc.DrawText(m_windowText, rc, DT_CENTER | DT_HIDEPREFIX | DT_WORDBREAK);
//	dc.SelectObject(oldPen);
//	dc.SelectObject(oldBrush);
//	dc.SelectObject(oldFont);
//}

int CMillionaireButton::StateToColor[] =
{
	COL_NormalBk, COL_NormalText,
	COL_OverBk, COL_OverText,
	COL_CorrectBk, COL_CorrectText,
	COL_WrongBk, COL_WrongText,
	COL_DisabledBk, COL_DisabledText
};

void CMillionaireButton::DrawItem (LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC* pDC = CDC::FromHandle (lpDrawItemStruct->hDC);
	ASSERT (pDC != NULL);
	
	//CDC& dc = (CDC&) pDC;

	CRect rc = lpDrawItemStruct->rcItem;
	//GetClientRect(&rc);

	CBrush penBrush(m_colors[COL_Frame]);
	CBrush fillBrush(m_colors[StateToColor[m_currentState * 2]]);
	pDC->SetTextColor(m_colors[StateToColor[m_currentState * 2 + 1]]);
	CBrush* oldBrush = pDC->SelectObject(&fillBrush);
	pDC->PaintRgn(&m_rgn);
	pDC->FrameRgn(&m_rgn, &penBrush, 1, 1);
	pDC->SetBkMode(TRANSPARENT);

	CFont* newFont = m_pFont;
	if (newFont == NULL)
		newFont = GetFont();
	CFont* oldFont = (CFont*) pDC->SelectObject(newFont);
	rc.left += m_margin;
	rc.right -= m_margin;
	GetWindowText(m_windowText);
	pDC->DrawText(m_windowText, rc, m_textAlign | DT_NOPREFIX /*| DT_WORDBREAK*/ | DT_VCENTER | DT_SINGLELINE);
	pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldFont);
}

void CMillionaireButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	// Start tracking the mouse if we're a hot button
	//if (m_bHot && !m_bTracking) {

	if (m_currentState == ST_Normal)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = HOVER_DEFAULT;
		VERIFY(_TrackMouseEvent(&tme));
		m_currentState = ST_Over;

		HWND hWnd = GetSafeHwnd();
		if ((hWnd != NULL) && ::IsWindow (hWnd))
			Invalidate();
	}

	//}

	// Do default action
	CButton::OnMouseMove(nFlags, point);
}

LRESULT CMillionaireButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	if (m_currentState == ST_Over)
	{
		m_currentState = ST_Normal;
		HWND hWnd = GetSafeHwnd();
		if ((hWnd != NULL) && ::IsWindow (hWnd))
			Invalidate();
	}
	return 0;
}

void CMillionaireButton::UpdateWindowRegion(INT cx, INT cy)
{
	SetWindowRgn(NULL, FALSE);
	m_rgnWnd.DeleteObject();
	switch (m_ps)
	{
	case POL_Rect:
		m_rgnWnd.CreateRectRgn(0, 0, cx, cy);
		break;
	case POL_RoundRect:
		{
			INT psxParam = m_psxParam;
			if (psxParam == 0)
				psxParam = min(cx, cy) / 4;
			m_rgnWnd.CreateRoundRectRgn(0, 0, cx, cy, psxParam, psxParam);
		}
		break;
	case POL_Ellipse:
		m_rgnWnd.CreateEllipticRgn(0, 0, cx, cy);
		break;
	case POL_Diamond:
		{
			INT psxParam = m_psxParam;
			if (psxParam == 0)
				psxParam = min(cx, cy) / 2;
			POINT p[] = {
				{psxParam, 0},
				{cx - psxParam, 0},
				{cx, cy/2},
				{cx - psxParam, cy},
				{psxParam, cy},
				{0, cy/2}
			};
			m_rgnWnd.CreatePolygonRgn(p, sizeof(p)/sizeof(POINT), ALTERNATE);
		}
		break;
	default:
		ASSERT(0);
		break;
	}
	m_rgn.DeleteObject();
	m_rgn.CreateRectRgn(0,0,0,0);
	m_rgn.CopyRgn(&m_rgnWnd);
	SetWindowRgn(m_rgnWnd, FALSE);

}
void CMillionaireButton::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW);
	CRect rc;
	GetClientRect(&rc);
	UpdateWindowRegion(rc.Width(), rc.Height());

	//CRect rc;
	//GetClientRect(&rc);
	//m_rgn.CreateRoundRectRgn(rc.left,rc.top,rc.right, rc.bottom, 




	//	m_rgn.DeleteObject(); 
	//	SetWindowRgn(NULL, FALSE);

	//	m_pPoints = new POINT[m_edges.size()];
	//	for (int i=0;i<m_edges.size();i++)
	//	{
	//		m_pPoints[i] = m_edges.at(i).start;

	//		if (i>0)
	//		{
	//			ASSERT(m_edges.at(i-1).end == m_edges.at(i).start);
	//		}
	//	}

	//	m_rgnWnd.CreateRectRgn(0,0,0,0);

	//	// couldn't find a way to expand a region. so, we'll
	//	// offset the region in 8 directions and combine the
	//	// results. yes, this is a hack. 

	//	CRgn temp;
	//	temp.CreatePolygonRgn(m_pPoints, m_edges.size(), ALTERNATE);

	//	temp.OffsetRgn(-1,0);
	//	m_rgnWnd.CombineRgn(&temp, &m_rgnWnd, SIMPLEREGION);
	//	temp.OffsetRgn(0,1);
	//	m_rgnWnd.CombineRgn(&temp, &m_rgnWnd, SIMPLEREGION);
	//	temp.OffsetRgn(1,0);
	//	m_rgnWnd.CombineRgn(&temp, &m_rgnWnd, SIMPLEREGION);
	//	temp.OffsetRgn(1,0);
	//	m_rgnWnd.CombineRgn(&temp, &m_rgnWnd, SIMPLEREGION);
	//	temp.OffsetRgn(0,-1);
	//	m_rgnWnd.CombineRgn(&temp, &m_rgnWnd, SIMPLEREGION);
	//	temp.OffsetRgn(0,-1);
	//	m_rgnWnd.CombineRgn(&temp, &m_rgnWnd, SIMPLEREGION);
	//	temp.OffsetRgn(-1,0);
	//	m_rgnWnd.CombineRgn(&temp, &m_rgnWnd, SIMPLEREGION);
	//	temp.OffsetRgn(-1,0);
	//	m_rgnWnd.CombineRgn(&temp, &m_rgnWnd, SIMPLEREGION);

	//	// once you use a region in SetWindowRgn, you're not allowed to do *anything* else with it
	//	// so, we'll make a copy.
	//	m_rgn.CreateRectRgn(0,0,0,0);
	//	m_rgn.CopyRgn(&m_rgnWnd);

	//	SetWindowRgn(m_rgnWnd, TRUE);

	//	if (m_bmpID!=0)
	//	{
	//		if (!m_bmp.LoadBitmap(m_bmpID))
	//		{
	//			ASSERT(0);
	//		}
	//	}

	//	if (m_bmpIDD!=0)
	//	{
	//		if (!m_bmpD.LoadBitmap(m_bmpIDD))
	//		{
	//			ASSERT(0);
	//		}
	//	}
	//}


	CButton::PreSubclassWindow();
}

void CMillionaireButton::OnSize(UINT nType, int cx, int cy)
{
	CButton::OnSize(nType, cx, cy);
	UpdateWindowRegion(cx, cy);
}
