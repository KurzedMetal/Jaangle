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
#include "RectAlphaDrawer.h"

CRect RectAlphaDrawer::s_InternalRect(0,0,256,256);

RectAlphaDrawer::RectAlphaDrawer():
m_bkColor(RGB(255,0,0)),
m_pDC(NULL),
m_pBMP(NULL),
m_pOldBMP(NULL),
m_drawOptions(NULL)
{
}

RectAlphaDrawer::~RectAlphaDrawer(void)	
{
	if (m_pDC != NULL)
	{
		m_pDC->SelectObject(m_pOldBMP);
		delete m_pBMP;
		delete m_pDC;
	}
}

void RectAlphaDrawer::SetColor(COLORREF col)
{
	m_bkColor = col;
	if (m_pDC != NULL)
		m_pDC->FillSolidRect(&s_InternalRect, m_bkColor);
}
void RectAlphaDrawer::Draw(CDC& dc, CRect& rc, BYTE Alpha)
{
	if (m_pDC == NULL)
		CreateInternalDC(&dc);
	BLENDFUNCTION blFunc = {AC_SRC_OVER, 0, Alpha, 0}; 
	dc.AlphaBlend(rc.left, rc.top, rc.Width(), rc.Height(), GetInternalDC(), 0, 0, s_InternalRect.Width(), s_InternalRect.Height(), blFunc);
	if (m_drawOptions & DO_DrawNotAlphaPerigram)
		dc.Draw3dRect(&rc, m_bkColor, m_bkColor);
}

CDC* RectAlphaDrawer::GetInternalDC()
{
	if (m_pDC == NULL)
		CreateInternalDC();
	ASSERT(m_pDC != NULL);
	return m_pDC;
}

void RectAlphaDrawer::CreateInternalDC(CDC* pParentDC)
{
	m_pDC = new CDC;
	m_pBMP = new CBitmap;
	if (pParentDC == NULL)
	{
		HDC screenDC = ::GetDC(0);
		m_pDC->CreateCompatibleDC(CDC::FromHandle(screenDC));
		m_pBMP->CreateCompatibleBitmap(CDC::FromHandle(screenDC), s_InternalRect.Width(), s_InternalRect.Height());
		::ReleaseDC(0, screenDC);
	}
	else
	{
		m_pDC->CreateCompatibleDC(pParentDC);
		m_pBMP->CreateCompatibleBitmap(pParentDC, s_InternalRect.Width(), s_InternalRect.Height());
	}
	m_pOldBMP = m_pDC->SelectObject(m_pBMP);
	SetColor(m_bkColor);
}

CRect* RectAlphaDrawer::GetInternalRect()
{
	return &s_InternalRect;
}

void RectAlphaDrawer::SetDrawOptions(UINT drawOptions)
{
	m_drawOptions = drawOptions;
}

