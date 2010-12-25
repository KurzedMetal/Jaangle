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
#include "StdAfx.h"
#include "FlickerFree.h"

CFlickerFree::CFlickerFree():
m_targetDC(NULL),
m_pOldBitmap(NULL)
{
}
CFlickerFree::~CFlickerFree()
{
}

CDC& CFlickerFree::GetMemDC()
{
	ASSERT(m_targetDC->m_hDC);
	if (m_dc.m_hDC == 0)
	{
		m_dc.CreateCompatibleDC(m_targetDC);

	}
	if (m_bitmap.m_hObject == 0)
	{
		m_targetDC->LPtoDP(&m_BitmapRect);
		m_bitmap.CreateCompatibleBitmap(m_targetDC, m_BitmapRect.Width(), m_BitmapRect.Height());
		m_targetDC->DPtoLP(&m_BitmapRect);
		m_pOldBitmap = m_dc.SelectObject(&m_bitmap);

		m_dc.SetMapMode(m_targetDC->GetMapMode());

		m_dc.SetWindowExt(m_targetDC->GetWindowExt());
		m_dc.SetViewportExt(m_targetDC->GetViewportExt());

		m_dc.SetWindowOrg(m_rect.left, m_rect.top);
	}	
	return m_dc;
}
void CFlickerFree::SetTargetDC(CDC& targetDC)
{
	ASSERT(targetDC.m_hDC);
	m_targetDC = &targetDC;
	CRect rc;
	targetDC.GetWindow()->GetClientRect(&rc);
	SetSize(rc.Width(), rc.Height());
}

void CFlickerFree::SetSize(INT cx, INT cy)
{
	m_rect.left = 0;
	m_rect.top = 0;
	m_rect.right = cx;
	m_rect.bottom = cy;
	if (m_dc.m_hDC && m_bitmap.m_hObject)
	{
		if (cx > m_BitmapRect.Width() || cy > m_BitmapRect.Height())
		{
			TRACE(_T("Resizing memory bitmap.\r\n\tOld: %dx%d\r\n\tNew: %dx%d\r\n"), 
				m_BitmapRect.Width(), m_BitmapRect.Height(), cx, cy);
			m_BitmapRect.right = max(m_BitmapRect.right, cx);
			m_BitmapRect.bottom = max(m_BitmapRect.bottom, cy);
			m_dc.SelectObject(m_pOldBitmap);
			m_bitmap.DeleteObject();
		}
	}
	else
		m_BitmapRect = m_rect;
}

void CFlickerFree::Draw()
{
	if (m_dc.m_hDC != 0)
	{
		m_targetDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
			&m_dc, 0, 0, SRCCOPY);
	}
}



CFlickerFreeDC::CFlickerFreeDC(CFlickerFree& ff, CDC& dc)
{
	m_ff = &ff;
	m_ff->SetTargetDC(dc);
	HDC hdc = m_ff->GetMemDC().m_hDC;
	if (hdc)
		Attach(hdc);
}

CFlickerFreeDC::~CFlickerFreeDC()
{
	m_ff->Draw();
	Detach();
}
