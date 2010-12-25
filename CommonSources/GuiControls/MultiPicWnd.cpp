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
#include "MultiPicWnd.h"

#define TIMER_CHANGE		130
#define TIMER_FADE_REFRESH 120
#define TIMER_FADE_DELAY 40


IMPLEMENT_DYNAMIC(CMultiPicWnd, CWnd)

CMultiPicWnd::CMultiPicWnd():
m_curFadeStep(-1),
m_totalFadeSteps(0),
m_bTransitioning(FALSE),
m_bMemDCInvalidated(TRUE),
m_pTitleFont(NULL),
m_curPic(-1),
m_ChangeDelayMS(5000),
m_bStarted(FALSE),
m_bTemporaryDisableTransitioning(TRUE),
m_mode(MOD_Normal)
{
	m_AlphaDrawer.SetDrawOptions(IAlphaDrawer::DO_DrawNotAlphaPerigram);
}

CMultiPicWnd::~CMultiPicWnd()
{
}

BEGIN_MESSAGE_MAP(CMultiPicWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CMultiPicWnd::AddImage(LPCTSTR imgFile, LPCTSTR imgName/* = _T("")*/)
{
	m_frames.push_back(Frame(imgFile, imgName));
	if (m_frames.size() == 1)
	{
		m_bMemDCInvalidated = TRUE;
		m_picDrawer.LoadFile(imgFile);
		m_curPic = 0;
		Redraw();
		Start();
	}
}

void CMultiPicWnd::ClearImages()
{
	m_frames.clear();
	m_picDrawer.Destroy();
	m_curPic = -1;
	Stop();
	Redraw();
}

UINT CMultiPicWnd::GetCount()
{
	return UINT(m_frames.size());
}


void CMultiPicWnd::Start()
{
	Stop();
	SetTimer(TIMER_CHANGE, m_ChangeDelayMS, NULL);
}
void CMultiPicWnd::Stop()
{
	KillTimer(TIMER_CHANGE);
	KillTimer(TIMER_FADE_REFRESH);
	m_bTransitioning = FALSE;
	m_curPic = 0;
}
void CMultiPicWnd::SetPictureDelay(UINT ms)
{
	if (ms<50)
		ms = 50;
	m_ChangeDelayMS = ms;
	if (m_bStarted)
		Start();
}

void CMultiPicWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_FADE_REFRESH)
	{
		KillTimer(TIMER_FADE_REFRESH);
		m_bTransitioning = TRUE;
		Redraw();
	} 
	else if (nIDEvent == TIMER_CHANGE)
	{
		if (m_mode == MOD_Normal)
		{
			if (m_frames.size() > 1)
			{
				if (IsWindowVisible())
				{
					m_curPic++;
					if (m_curPic >= INT(m_frames.size()))
						m_curPic = 0;
					m_picDrawer.LoadFile(m_frames[m_curPic].path.c_str());
					m_picDrawer.GetDrawParams().zoomLockMode = GdiPlusPicDrawer::ZLM_FullImage;
					m_bMemDCInvalidated = TRUE;
					Redraw();
				}
			}
		}
		else if (m_mode == MOD_ShowParts)
		{
			if (m_frames.size() > 1)
			{
				m_curPic++;
				if (m_curPic >= INT(m_frames.size()))
					m_curPic = 0;
				m_picDrawer.LoadFile(m_frames[m_curPic].path.c_str());
			}
			CRect rc;
			GetClientRect(&rc);
			if (m_picDrawer.GetZoomForLockMode(Rect(rc.left, rc.top, rc.Width(), rc.Height()), GdiPlusPicDrawer::ZLM_FullImage) < 1.0)
			{
				GdiPlusPicDrawer::DrawParams& params = m_picDrawer.GetDrawParams();
				params.zoomLockMode = GdiPlusPicDrawer::ZLM_NoLock;
				params.reqZoom = 1.0;
				params.positionX = rand() * 100 / RAND_MAX;
				params.positionY = rand() * 100 / RAND_MAX;
				m_bMemDCInvalidated = TRUE;
				Redraw();
			}
			else
			{
				if (m_picDrawer.GetDrawParams().zoomLockMode != GdiPlusPicDrawer::ZLM_FullImage)
				{
					m_picDrawer.GetDrawParams().zoomLockMode = GdiPlusPicDrawer::ZLM_FullImage;
					m_bMemDCInvalidated = TRUE;
					Redraw();
				}
			}
		}

	}
	CWnd::OnTimer(nIDEvent);
}

void CMultiPicWnd::Redraw()
{
	if (m_hWnd) 
		Invalidate(FALSE);
}

void CMultiPicWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rc;
	GetClientRect(&rc);
	m_ff.SetTargetDC(dc);
	if (m_bMemDCInvalidated)
	{
		m_picDrawer.Draw(m_ff.GetMemDC(), rc);
		if (m_curPic>=0 && m_frames[m_curPic].title.size() > 0)
		{
			CDC* pDC = &m_ff.GetMemDC();
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(255,255,255));
			CFont* pOldFont = NULL;
			if (m_pTitleFont != NULL)
				pOldFont = pDC->SelectObject(m_pTitleFont);

			CRect titleRC(rc);
			titleRC.DeflateRect(10,10);
			pDC->DrawText(m_frames[m_curPic].title.c_str(), -1, &titleRC, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_EXPANDTABS | DT_CALCRECT);

			int titHeight = titleRC.Height();
			titleRC.top = rc.bottom - titHeight - 10;
			titleRC.bottom = titleRC.top + titHeight;
			CRect borderRC(titleRC);
			borderRC.InflateRect(5,2);
			m_AlphaDrawer.SetColor(RGB(0,0,0));
			m_AlphaDrawer.Draw(*pDC, borderRC, 100);
			pDC->DrawText(m_frames[m_curPic].title.c_str(), -1, &titleRC, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_EXPANDTABS);
			if (pOldFont != NULL)
				pDC->SelectObject(pOldFont);
		}
		m_bMemDCInvalidated = FALSE;
		m_bTransitioning = TRUE;
		m_curFadeStep = 0;
	}
	if (m_bTemporaryDisableTransitioning)//Do not transition the first time
	{
		m_bTransitioning = FALSE;
		m_bTemporaryDisableTransitioning = FALSE;
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
BOOL CMultiPicWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
void CMultiPicWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	m_bMemDCInvalidated = TRUE;
	m_bTemporaryDisableTransitioning = TRUE;
	Redraw();
}

void CMultiPicWnd::SetTransitionTime(UINT ms)
{
	m_totalFadeSteps = ms / TIMER_FADE_DELAY;
	if (m_totalFadeSteps > 1000)
		m_totalFadeSteps = 1000;
}

void CMultiPicWnd::SetBkColor(COLORREF bkColor, BYTE opacity)
{
	m_picDrawer.SetBkColor(bkColor, opacity);
	m_bMemDCInvalidated = TRUE;
	Redraw();
}

LPCTSTR CMultiPicWnd::GetCurrentImagePath()
{
	if (m_curPic >= 0 && m_curPic < (INT)m_frames.size())
		return m_frames[m_curPic].path.c_str();
	return NULL;
}



