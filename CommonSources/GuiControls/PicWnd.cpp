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
#include "PicWnd.h"


IMPLEMENT_DYNAMIC(CPicWnd, CWnd)

CPicWnd::CPicWnd()
{
	SetBkColor(GetSysColor(COLOR_BTNFACE));
}

CPicWnd::~CPicWnd()
{
	ClearImage();
}


BEGIN_MESSAGE_MAP(CPicWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CPicWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rc;
	GetClientRect(&rc);
	m_picDrawer.Draw(dc, rc);
}
void CPicWnd::Redraw()
{
	if (m_hWnd) 
		Invalidate(FALSE);
}


void CPicWnd::SetBkColor(COLORREF bkColor, BYTE opacity)
{
	m_picDrawer.SetBkColor(bkColor, opacity);
	Redraw();
}

BOOL CPicWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}

void CPicWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	Redraw();
}

BOOL CPicWnd::LoadFile(LPCTSTR imgFile)
{
	ASSERT(imgFile != NULL);
	BOOL ret = m_picDrawer.LoadFile(imgFile);
	Redraw();
	return ret;
}

BOOL CPicWnd::LoadResourceStr(LPCTSTR resName, LPCTSTR resType, HMODULE hInst)
{
	ASSERT(resName != NULL);
	ASSERT(resType != NULL);
	BOOL ret = m_picDrawer.LoadResourceStr(resName, resType, hInst);
	Redraw();
	return ret;
}

BOOL CPicWnd::LoadResourceID(UINT resID, LPCTSTR resType, HMODULE hInst)
{
	return LoadResourceStr(MAKEINTRESOURCE(resID), resType, hInst);
}


void CPicWnd::ClearImage()
{
	m_picDrawer.Destroy();
	Redraw();
}

CSize CPicWnd::GetPictureSize()
{
	return m_picDrawer.GetPictureSize();
}
