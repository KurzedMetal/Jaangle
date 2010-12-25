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
#include "PicWndEx.h"

#define TIMER_FADE_REFRESH 120
#define TIMER_FADE_DELAY 40


IMPLEMENT_DYNAMIC(CPicWndEx, CWnd)

CPicWndEx::CPicWndEx():
m_curFadeStep(-1),
m_totalFadeSteps(0),
m_bTransitioning(FALSE),
m_bMemDCInvalidated(TRUE),
m_pTitleFont(NULL)
{
	m_AlphaDrawer.SetDrawOptions(IAlphaDrawer::DO_DrawNotAlphaPerigram);
}

CPicWndEx::~CPicWndEx()
{
}


BEGIN_MESSAGE_MAP(CPicWndEx, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CPicWndEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rc;
	GetClientRect(&rc);
	m_ff.SetTargetDC(dc);
	if (m_bMemDCInvalidated)
	{
		m_picDrawer.Draw(m_ff.GetMemDC(), rc);
		if (!m_title.empty())
		{
			CDC* pDC = &m_ff.GetMemDC();
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(255,255,255));
			CFont* pOldFont = NULL;
			if (m_pTitleFont != NULL)
				pOldFont = pDC->SelectObject(m_pTitleFont);
		
			CRect titleRC(rc);
			titleRC.DeflateRect(10,10);
			pDC->DrawText(m_title.c_str(), -1, &titleRC, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_EXPANDTABS | DT_CALCRECT);

			int titHeight = titleRC.Height();
			titleRC.top = rc.bottom - titHeight - 10;
			titleRC.bottom = titleRC.top + titHeight;
			CRect borderRC(titleRC);
			borderRC.InflateRect(5,2);
			m_AlphaDrawer.SetColor(RGB(0,0,0));
			m_AlphaDrawer.Draw(*pDC, borderRC, 100);
			pDC->DrawText(m_title.c_str(), -1, &titleRC, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_EXPANDTABS);
			if (pOldFont != NULL)
				pDC->SelectObject(pOldFont);
		}
		m_bMemDCInvalidated = FALSE;
		m_bTransitioning = TRUE;
		m_curFadeStep = 0;
	}

	if (m_bTransitioning)
	{
		if (m_curFadeStep < m_totalFadeSteps)
		{
			//TRACE(_T("CFadePicWnd: Fading: %d/%d\r\n"), m_curFadeStep, m_totalFadeSteps);
			BLENDFUNCTION bf;
			bf.BlendOp=AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = (255 * m_curFadeStep) / ( 2 * m_totalFadeSteps + 1);//v1 Increasing
			bf.AlphaFormat = 0;//AC_SRC_ALPHA ;
			dc.AlphaBlend(rc.left, rc.top, rc.Width(), rc.Height(), &m_ff.GetMemDC(), 0, 0, rc.Width(), rc.Height(), bf);
			SetTimer(TIMER_FADE_REFRESH, TIMER_FADE_DELAY, NULL);
			m_curFadeStep++;
		}
		else
		{
			//TRACE(_T("CFadePicWnd: Fading Finished: Blitting\r\n"));
			m_ff.Draw();
		}
		m_bTransitioning = FALSE;
	}
	else
	{
		m_ff.Draw();
	}
}
BOOL CPicWndEx::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
void CPicWndEx::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	Redraw();
}

void CPicWndEx::SetTransitionTime(UINT ms)
{
	m_totalFadeSteps = ms / TIMER_FADE_DELAY;
}

void CPicWndEx::SetTitle(LPCTSTR title)
{
	m_title = title;
	Redraw();
}

void CPicWndEx::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_FADE_REFRESH)
	{
		KillTimer(TIMER_FADE_REFRESH);
		m_bTransitioning = TRUE;
		Invalidate(FALSE);
	}
}

void CPicWndEx::ClearImage()
{
	m_picDrawer.Destroy();
	Redraw();
}

void CPicWndEx::Redraw()
{
	m_bMemDCInvalidated = TRUE;
	if (m_hWnd) 
		Invalidate(FALSE);
}
BOOL CPicWndEx::LoadFile(LPCTSTR imgFile)
{
	ASSERT(imgFile != NULL);
	BOOL ret = m_picDrawer.LoadFile(imgFile);
	Redraw();
	return ret;
}

BOOL CPicWndEx::LoadResourceStr(LPCTSTR resName, LPCTSTR resType, HMODULE hInst)
{
	ASSERT(resName != NULL);
	ASSERT(resType != NULL);
	BOOL ret = m_picDrawer.LoadResourceStr(resName, resType, hInst);
	Redraw();
	return ret;
}

BOOL CPicWndEx::LoadResourceID(UINT resID, LPCTSTR resType, HMODULE hInst)
{
	return LoadResourceStr(MAKEINTRESOURCE(resID), resType, hInst);
}

CSize CPicWndEx::GetPictureSize()
{
	return m_picDrawer.GetPictureSize();
}

void CPicWndEx::SetBkColor(COLORREF bkColor, BYTE opacity)
{
	m_picDrawer.SetBkColor(bkColor, opacity);
	Redraw();
}
