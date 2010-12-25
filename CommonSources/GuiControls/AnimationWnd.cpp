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
#include "AnimationWnd.h"

#define ID_TIMER_REFRESH 100


// CAnimationWnd

IMPLEMENT_DYNAMIC(CAnimationWnd, CWnd)

CAnimationWnd::CAnimationWnd():
m_cx(0),
m_cy(0),
m_delayMS(40),
m_curFrame(0),
m_TotalFrames(0),
m_bStarted(FALSE)
{
	SetBkColor(GetSysColor(COLOR_BTNFACE));
	m_picDrawer.GetDrawParams().drawMode = GdiPlusPicDrawer::DM_Stretch;
	m_picDrawer.GetDrawParams().bTransparent = TRUE;
	m_picLoader.GetDrawParams().bTransparent = TRUE;
	m_bkColor = Color(255, 0, 0, 0);

}

CAnimationWnd::~CAnimationWnd()
{
	ClearImage();
}


BEGIN_MESSAGE_MAP(CAnimationWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CAnimationWnd message handlers



void CAnimationWnd::OnPaint()
{

	CPaintDC dc(this); // device context for painting
	CRect rc;
	GetClientRect(&rc);
	DrawAnimation(dc, rc);
}

void CAnimationWnd::DrawAnimation(CDC& dc, CRect& rc)
{
	if (m_cx && m_cy && m_picDrawer.GetPictureSize() != CSize(0,0))
	{
		INT imgPerLine = m_picDrawer.GetPictureSize().cx / m_cx;
		if (imgPerLine == 0)
		{
			TRACE(_T("FrameSize Larger than picture width"));
			imgPerLine = 1;
		}
		INT curLine = m_curFrame / imgPerLine;
		INT curColumn = m_curFrame % imgPerLine;
		INT srcX = curColumn * m_cx;
		INT srcY = curLine * m_cy;
		m_picDrawer.Draw(dc, rc, &CRect(srcX, srcY, srcX + m_cx, srcY + m_cy));
	}
	else
		m_picDrawer.Draw(dc, rc);

}

void CAnimationWnd::SetBkColor(COLORREF bkColor, BYTE opacity)
{
	m_bkColor = Color(opacity, GetRValue(bkColor), GetGValue(bkColor), GetBValue(bkColor));
	InitializeDrawer();
}




void CAnimationWnd::Start()
{
	Stop();
	m_bStarted = TRUE;
	if (m_TotalFrames > 0)
		m_curFrame = 1;
	SetTimer(ID_TIMER_REFRESH, m_delayMS, NULL);
}
void CAnimationWnd::Stop()
{
	m_bStarted = FALSE;
	KillTimer(ID_TIMER_REFRESH);
	m_curFrame = 0;
	Invalidate(FALSE);
}


BOOL CAnimationWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CAnimationWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_TIMER_REFRESH)
	{
		if (m_TotalFrames > 0)
		{
			m_curFrame++;
			if (m_curFrame >= m_TotalFrames)
				m_curFrame = 1;
			Invalidate(FALSE);
		}
	}
	CWnd::OnTimer(nIDEvent);
}

void CAnimationWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}
BOOL CAnimationWnd::LoadFile(LPCTSTR imgFile)
{
	ASSERT(imgFile != NULL);
	BOOL ret = m_picDrawer.LoadFile(imgFile);
	if (ret)
		CalculateFrames();
	return ret;
}

BOOL CAnimationWnd::LoadResourceStr(LPCTSTR resName, LPCTSTR resType, HMODULE hInst)
{
	ASSERT(resName != NULL);
	ASSERT(resType != NULL);
	BOOL ret = m_picLoader.LoadResourceStr(resName, resType, hInst);
	if (ret)
		CalculateFrames();
	return ret;
}

BOOL CAnimationWnd::LoadResourceID(UINT resID, LPCTSTR resType, HMODULE hInst)
{
	return LoadResourceStr(MAKEINTRESOURCE(resID), resType, hInst);
}


void CAnimationWnd::ClearImage()
{
	m_picDrawer.Destroy();
}
void CAnimationWnd::SetAnimationDelay(UINT delayMS)
{
	m_delayMS = delayMS;
	if (m_bStarted)
		Start();
}

void CAnimationWnd::SetAutoSize(BOOL bAutoSize)
{
	m_picDrawer.GetDrawParams().zoomLockMode = bAutoSize ? GdiPlusPicDrawer::ZLM_FullImage : GdiPlusPicDrawer::ZLM_NoLock;
}
BOOL CAnimationWnd::GetAutoSize()									
{
	return m_picDrawer.GetDrawParams().zoomLockMode != GdiPlusPicDrawer::ZLM_NoLock;
}


void CAnimationWnd::CalculateFrames()
{
	CSize sz = m_picLoader.GetPictureSize();
	if (sz.cx > 0 && sz.cy > 0 && m_cx > 0 && m_cy > 0)
	{
		if (m_cx > sz.cx)	m_cx = sz.cx;
		if (m_cy > sz.cy)	m_cy = sz.cy;
		m_TotalFrames = (sz.cx / m_cx) * (sz.cy / m_cy);
		InitializeDrawer();
	}
}


void CAnimationWnd::InitializeDrawer()
{
	if (m_picLoader.HasImage())
	{
		if (m_picDrawer.HasImage() == TRUE)
		{
			CSize l = m_picLoader.GetPictureSize();
			CSize d = m_picDrawer.GetPictureSize();
			if (l != d)
				m_picDrawer.Destroy();
			
		}
		if (m_picDrawer.HasImage() == FALSE)
		{
			m_picDrawer.Create(m_picLoader.GetPictureSize().cx, m_picLoader.GetPictureSize().cy);
		}
		// Clear the screen with the color
		Graphics g(m_picDrawer.GetInternalBitmap());
		g.Clear(m_bkColor);
		m_picLoader.Draw(g, Rect(0,0,m_picLoader.GetPictureSize().cx - 1, m_picLoader.GetPictureSize().cy - 1));
	}

}
