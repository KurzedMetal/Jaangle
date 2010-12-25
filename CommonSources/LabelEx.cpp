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
#include "LabelEx.h"

// CLabelEx

IMPLEMENT_DYNAMIC(CLabelEx, CWnd)

CLabelEx::CLabelEx():
m_bt(CLabelEx::B_NoBorder),
m_xParam1(0),
m_xParam2(0),
m_xMargin(2),
m_yMargin(2),
m_bEraseBk(FALSE),
m_cxIcon(0),
m_cyIcon(0),
m_pSpecialFont(NULL),
m_bAutoWidth(FALSE)
{
	m_colors[COL_Erase] = GetSysColor(COLOR_WINDOW);
	m_colors[COL_Bk] = GetSysColor(COLOR_BTNFACE);
	m_colors[COL_Text] = GetSysColor(COLOR_MENUTEXT);
	m_colors[COL_Fore] = GetSysColor(COLOR_MENUTEXT);
}

CLabelEx::~CLabelEx()
{
}


BEGIN_MESSAGE_MAP(CLabelEx, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(WM_GETTEXT, OnGetText)
END_MESSAGE_MAP()



// CLabelEx message handlers

void CLabelEx::SetMargins(INT xMargin, INT yMargin)
{
	m_xMargin = xMargin;
	m_yMargin = yMargin;
}


BOOL CLabelEx::OnEraseBkgnd(CDC* pDC)
{
	if (m_bEraseBk)
	{
		CRect rc;
		GetClientRect(&rc);
		pDC->FillSolidRect(&rc, m_colors[COL_Erase]);
	}
	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}

void CLabelEx::SetIconProperties(INT cxIcon, INT cyIcon)
{
	m_cxIcon = cxIcon;
	m_cyIcon = cyIcon;
}
void CLabelEx::ClearIcons()
{
	m_hIcons.clear();
}


void CLabelEx::AddIcon(HICON hIcon)
{
	ASSERT(hIcon != NULL);
	m_hIcons.push_back(hIcon);
}

INT CLabelEx::GetIconCount()
{
	return m_hIcons.size();
}

INT CLabelEx::CalculateWidth()
{
	CClientDC dc(this);
	return CalculateWidthInternal(dc);
}


INT CLabelEx::CalculateWidthInternal(CDC& dc)
{
	UINT len = 3 * m_xMargin;
	len += (1 + m_cxIcon) * m_hIcons.size();
	if (!m_hIcons.empty())
		len += 3;//Extra distance between Icon & Text
	CRect testRC(0,0,0,0);
	DrawString(dc, testRC, TRUE);
	len += testRC.Width();
	if (m_bt == B_LeftLine)
		len += 4;
	return len;
}

void CLabelEx::DrawString(CDC& dc, CRect& rc, BOOL bCalcOnly)
{
	dc.SetTextColor(m_colors[COL_Text]);
	dc.SetBkMode(TRANSPARENT);
	int drawOptions = DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER;
	if (!bCalcOnly)
		drawOptions |= DT_END_ELLIPSIS;
	INT offset = 0, startIdx = 0, curLeft = 0;
	CRect rcResult(0,0,0,0);
	CFont* pOldFont = dc.SelectObject(GetFont());
	while (TRUE)
	{
		startIdx = m_text.find(_T("<s>"), offset);
		if (startIdx == std::tstring::npos || startIdx > offset)
		{
			INT textLen = (startIdx == std::tstring::npos) ? -1 : startIdx - offset;
			//Calculate & Draw the normal string
			CRect rcTest(rc);
			dc.SelectObject(GetFont());
			dc.DrawText(&m_text[offset], textLen, rcTest, drawOptions | DT_CALCRECT);
			if (!bCalcOnly)
			{
				CRect rcDraw(rc);
				rcDraw.left += curLeft;
				dc.DrawText(&m_text[offset], textLen, rcDraw, drawOptions);
			}
			rcTest.OffsetRect(curLeft, 0);
			rcResult.UnionRect(&rcResult, rcTest);
			curLeft += rcTest.Width();
		}
		if (startIdx == std::tstring::npos)
			break;

		//Calculate & Draw the special string
		startIdx += 3;
		INT endIdx = m_text.find(_T("</s>"), startIdx);
		if (endIdx == std::tstring::npos || endIdx > startIdx)
		{
			INT textLen = (endIdx == std::tstring::npos) ? -1 : endIdx - startIdx;
			//Calculate & Draw the normal string
			LPCTSTR dbg = &m_text[startIdx];
			CRect rcTest(rc);
			dc.SelectObject(m_pSpecialFont ? m_pSpecialFont : GetFont());
			dc.DrawText(&m_text[startIdx], textLen, rcTest, drawOptions | DT_CALCRECT);
			if (!bCalcOnly)
			{
				CRect rcDraw(rc);
				rcDraw.left += curLeft;
				dc.DrawText(&m_text[startIdx], textLen, rcDraw, drawOptions);
			}
			rcTest.OffsetRect(curLeft, 0);
			rcResult.UnionRect(&rcResult, rcTest);
			curLeft += rcTest.Width();
		}
		offset = endIdx + 4;
	
	}
	//VERIFY(dc.DrawText(m_text.c_str(), m_text.size(), rc, drawOptions) != 0);
	dc.SelectObject(pOldFont);
	if (bCalcOnly)
		rc = rcResult;
}

INT CLabelEx::GetIconIndexFromPoint(CPoint& pt)
{
	if (m_hIcons.size() == 0)
		return -1;

	CRect rc;
	GetClientRect(&rc);
	rc.left += m_xMargin;
	rc.right -= m_xMargin;
	rc.top += m_yMargin;
	rc.bottom -= m_yMargin;
	CPoint local(pt);
	GetParent()->ClientToScreen(&local);
	ScreenToClient(&local);

	for (UINT i=0; i<m_hIcons.size(); i++)
	{
		CRect rcIcon(CPoint(rc.left, rc.top + (rc.Height() - m_cyIcon) / 2), CSize(m_cxIcon, m_cyIcon));
		if (rcIcon.PtInRect(local))
			return i;
		rc.left += m_cxIcon+1;
	}
	return -1;
}


void CLabelEx::OnPaint()
{
	{
		CPaintDC _dc(this); // device context for painting
	}
	CClientDC dc(this); // There are artifacts while resizing if i don't use this
	CRect rc;
	GetClientRect(&rc);


	CPen* pOldPen = NULL;
	if (m_Pen.m_hObject != NULL)
		pOldPen = dc.SelectObject(&m_Pen);
	CBrush* pOldBrush = NULL;
	if (m_bkBrush.m_hObject != NULL)
		pOldBrush = dc.SelectObject(&m_bkBrush);

	if (m_bAutoWidth)
	{
		INT len = CalculateWidthInternal(dc);
		if (len < rc.Width())
			rc.right = len + rc.left;
	}
	switch(m_bt)
	{
	case B_NoBorder:
		dc.FillSolidRect(&rc, m_colors[COL_Bk]);
		break;
	case B_Rect:
		dc.Rectangle(&rc);
		break;
	case B_RoundRect:
		dc.RoundRect(&rc, CPoint(m_xParam1, m_xParam2));
		break;
	case B_BottomLine:
		dc.FillSolidRect(&rc, m_colors[COL_Bk]);
		dc.MoveTo(rc.left, rc.bottom);
		dc.LineTo(rc.right, rc.bottom);
		break;
	case B_LeftLine:
		dc.FillSolidRect(&rc, m_colors[COL_Bk]);
		dc.MoveTo(rc.left, rc.top);
		dc.LineTo(rc.left, rc.bottom);
		break;
	default:
		ASSERT(0);//?
		break;
	}
	rc.left += m_xMargin;
	rc.right -= m_xMargin;
	rc.top += m_yMargin;
	rc.bottom -= m_yMargin;

	if (m_bt == B_LeftLine)
		rc.left += 4;
	for (UINT i=0; i<m_hIcons.size(); i++)
	{
		DrawIconEx(dc.m_hDC, rc.left, rc.top + (rc.Height() - m_cyIcon) / 2, m_hIcons[i], m_cxIcon, m_cyIcon, 0, 0, DI_NORMAL);
		rc.left += 1 + m_cxIcon;
	}

	if (!m_hIcons.empty())
		rc.left += 3;//Extra distance between Icon & Text



	DrawString(dc, rc, FALSE);
	//dc.DrawText(m_text.c_str(), m_text.size(), rc, drawOptions);
	//dc.SelectObject(pOldFont);
	if (pOldPen != NULL)
		dc.SelectObject(pOldPen);
	if (pOldBrush != NULL)
		dc.SelectObject(pOldBrush);
}

void CLabelEx::SetBorderType(BORDER_TYPE bt, INT xParam1/* = 0*/, INT xParam2/* = 0*/)
{
	ASSERT(bt < B_Last);
	m_bt = bt;
	m_xParam1 = xParam1;
	m_xParam2 = xParam2;
	if (m_hWnd)
		Invalidate();
}

afx_msg LRESULT CLabelEx::OnSetText(WPARAM wParam, LPARAM lParam)
{
	if (lParam != 0)
		m_text = ((LPCTSTR)lParam);
	else
		m_text.clear();
	if (m_hWnd)
		Invalidate();
	return 0;
} //End of OnSetFont

afx_msg LRESULT CLabelEx::OnGetText(WPARAM wParam, LPARAM lParam)
{
	_tcsncpy((LPTSTR)lParam, m_text.c_str(), wParam);
	return 1;
	//return (LRESULT)(LPCTSTR)m_sText;
} //End of OnSetFont

void CLabelEx::SetColor(COLORS idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
	{
		m_colors[idx] = value;
		if (idx == COL_Bk)
		{
			if (m_bkBrush.GetSafeHandle() != NULL)
				m_bkBrush.DeleteObject();
			m_bkBrush.CreateSolidBrush(value);
		}
		else if (idx == COL_Fore)
		{
			if (m_Pen.m_hObject != NULL)
				m_Pen.DeleteObject();
			m_Pen.CreatePen(PS_SOLID, 1, value);
		}
		else if (idx == COL_Erase)
		{
			m_bEraseBk = TRUE;
		}
		if (m_hWnd)		
			Invalidate();

	}
}
COLORREF CLabelEx::GetColor(COLORS idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}

