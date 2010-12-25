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
#include "CountDownCtrl.h"


// CCountDownCtrl

IMPLEMENT_DYNAMIC(CCountDownCtrl, CProgressCtrl)

CCountDownCtrl::CCountDownCtrl():
m_bReverseBar(TRUE),
m_unit(UN_Unknown)
{
	m_colors[COL_Bk] = RGB(0,0,0);
	m_colors[COL_Text] = RGB(255,255,255);
	m_colors[COL_Fg1] = RGB(158, 194, 14);
	m_colors[COL_Fg2] = RGB(255, 147, 8);
}

CCountDownCtrl::~CCountDownCtrl()
{
}


BEGIN_MESSAGE_MAP(CCountDownCtrl, CProgressCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CCountDownCtrl message handlers



BOOL CCountDownCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;//CProgressCtrl::OnEraseBkgnd(pDC);
}

void CCountDownCtrl::OnPaint()
{
	CPaintDC _dc(this); // device context for painting
	CFlickerFreeDC dc(m_ff, _dc);
	CRect rc;
	GetClientRect(&rc);
	INT minVal, maxVal, pos;
	GetRange(minVal, maxVal);
	pos = GetPos();
	CRect rc1(rc);
	CRect rc2(rc);
	DOUBLE relPos = (pos - minVal) / DOUBLE(maxVal - minVal);
	if (m_bReverseBar)
	{
		rc2.right = LONG(rc.Width() * relPos);
		rc1.left = rc2.right + 1;
		pos = minVal + maxVal - pos;
	}
	else
	{
		rc1.right = LONG(rc.Width() * relPos);
		rc2.left = rc1.right + 1;
	}
	dc.FillSolidRect(&rc1, GetGradientColor(relPos));
	dc.FillSolidRect(&rc2, m_colors[COL_Bk]);
	CFont* oldFont = dc.SelectObject(GetFont());
	TCHAR bf[100];
	if (m_unit == UN_ms)
		_sntprintf(bf, 100, _T("%d.%0.3d ms"), INT(pos / 1000), pos % 1000);
	else
		_sntprintf(bf, 100, _T("%d"), pos);
	bf[99] = 0;
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(m_colors[COL_Text]);
	dc.DrawText(bf, -1, &rc, DT_SINGLELINE|DT_CENTER|DT_VCENTER | DT_NOPREFIX);
	dc.SelectObject(oldFont);
	dc.SetPixel(rc.left, rc.top, m_colors[COL_Bk]);
	dc.SetPixel(rc.right, rc.top, m_colors[COL_Bk]);
	dc.SetPixel(rc.left, rc.bottom, m_colors[COL_Bk]);
	dc.SetPixel(rc.right, rc.bottom, m_colors[COL_Bk]);
}

COLORREF CCountDownCtrl::GetGradientColor(DOUBLE relPos)
{
	//INT r1 = GetRValue(m_colors[COL_Fg1]);
	//INT g1 = GetGValue(m_colors[COL_Fg1]);
	//INT b1 = GetBValue(m_colors[COL_Fg1]);
	//INT r2 = GetRValue(m_colors[COL_Fg2]);
	//INT g2 = GetGValue(m_colors[COL_Fg2]);
	//INT b2 = GetBValue(m_colors[COL_Fg2]);
	//INT r = r1 * (1.0 - relPos) + r2 * relPos;
	//INT g = g1 * (1.0 - relPos) + g2 * relPos;
	//INT b = b1 * (1.0 - relPos) + b2 * relPos;
	INT r = INT(GetRValue(m_colors[COL_Fg1]) * (1.0 - relPos) + GetRValue(m_colors[COL_Fg2]) * relPos);
	INT g = INT(GetGValue(m_colors[COL_Fg1]) * (1.0 - relPos) + GetGValue(m_colors[COL_Fg2]) * relPos);
	INT b = INT(GetBValue(m_colors[COL_Fg1]) * (1.0 - relPos) + GetBValue(m_colors[COL_Fg2]) * relPos);
	return RGB(r,g,b);
}

void CCountDownCtrl::SetColor(COLORS colID, COLORREF value)
{
	ASSERT(colID>=0 && colID < COL_Last);
	m_colors[colID] = value;
}

int CCountDownCtrl::SetPos(int nPos)
{
	Invalidate(FALSE);
	return CProgressCtrl::SetPos(nPos);
}



