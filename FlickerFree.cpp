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
	memset(&m_bitmapRect, 0, sizeof(CRect));
}
CFlickerFree::~CFlickerFree()
{
	if (m_dc.m_hDC != 0)
	{
		if (m_pOldBitmap != NULL)
			m_dc.SelectObject(m_pOldBitmap);
	}
		
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
		TRACE(_T("CFlickerFree::GetMemDC Creating Bitmap cx:%d, cy:%d\r\n"), m_bitmapRect.right, m_bitmapRect.bottom);
		CRect rc(m_bitmapRect);
		m_targetDC->LPtoDP(&rc);
		m_bitmap.CreateCompatibleBitmap(m_targetDC, rc.Width(), rc.Height());
		m_pOldBitmap = m_dc.SelectObject(&m_bitmap);

		m_dc.SetMapMode(m_targetDC->GetMapMode());

		m_dc.SetWindowExt(m_targetDC->GetWindowExt());
		m_dc.SetViewportExt(m_targetDC->GetViewportExt());

		m_dc.SetWindowOrg(0, 0);
	}	
	return m_dc;
}
void CFlickerFree::SetTargetDC(CDC& targetDC)
{
	ASSERT(targetDC.m_hDC);
	m_targetDC = &targetDC;
	targetDC.GetWindow()->GetClientRect(&m_targetRC);
	//=== Make it at least the size of targer DC
	SetSize(m_targetRC.Width(), m_targetRC.Height());
}

void CFlickerFree::SetSize(INT cx, INT cy)
{
	if (cx > m_bitmapRect.right || cy > m_bitmapRect.bottom)
	{
		TRACE(_T("CFlickerFree::SetSize (Setting) cx:%d, cy:%d\r\n"), cx, cy);
		m_bitmapRect.left = m_bitmapRect.top = 0;
		m_bitmapRect.right = max(m_bitmapRect.right, cx);
		m_bitmapRect.bottom = max(m_bitmapRect.bottom, cy);
		if (m_dc.m_hDC && m_bitmap.m_hObject)
		{
			TRACE(_T("CFlickerFree::SetSize Deleting old bitmap\r\n"));
			m_dc.SelectObject(m_pOldBitmap);
			m_bitmap.DeleteObject();
		}
	}
}

void CFlickerFree::Draw()
{
	if (m_dc.m_hDC != 0)
	{
		m_targetDC->BitBlt(m_targetRC.left, m_targetRC.top, m_targetRC.Width(), m_targetRC.Height(),
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
