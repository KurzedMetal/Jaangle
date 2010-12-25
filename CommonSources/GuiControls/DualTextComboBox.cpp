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
#include "DualTextComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDualTextComboBox::CDualTextComboBox()
{
}

CDualTextComboBox::~CDualTextComboBox()
{
}


BEGIN_MESSAGE_MAP(CDualTextComboBox, CComboBox)
	ON_WM_DELETEITEM()
END_MESSAGE_MAP()

int CDualTextComboBox::AddItem(LPCTSTR mainString, LPCTSTR secondString, HICON hIcon)
{
	ASSERT(mainString != NULL);
	DWORD style = GetStyle();
	int ret = 0; 
	if ((style & CBS_OWNERDRAWFIXED) == CBS_OWNERDRAWFIXED)
	{
		ItemData id;
		id.mainString = mainString;
		if (secondString != NULL)
			id.secondString = secondString;
		id.hIcon = hIcon;
		int ret = CComboBox::AddString(_T(""));

		m_items.push_back(id);
		ASSERT(ret == m_items.size() - 1);
	}
	else
		ret = CComboBox::AddString(mainString);


	return ret;
}

void CDualTextComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (lpDrawItemStruct->itemID >= (UINT)GetCount()) 
		return;
	LPCTSTR mainString = m_items[lpDrawItemStruct->itemID].mainString.c_str();
	CDC dc;
	BOOL bSelected = FALSE;
	dc.Attach(lpDrawItemStruct->hDC);

	COLORREF crOldTextColor = dc.GetTextColor();
	COLORREF crOldBkColor = dc.GetBkColor();
	COLORREF textColor = crOldTextColor;

	if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
		(lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		textColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
		dc.SetTextColor(textColor);
		dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
		bSelected = TRUE;
	}
	else
	{
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);
	}

	CRect rect(lpDrawItemStruct->rcItem);
	rect.DeflateRect(4,0);
	dc.SetBkMode(TRANSPARENT);

	if (m_items[lpDrawItemStruct->itemID].hIcon != 0)
	{
		DrawIconEx(lpDrawItemStruct->hDC, rect.left, rect.top + (rect.Height() - 16)/2, m_items[lpDrawItemStruct->itemID].hIcon, 16, 16, 0,NULL, DI_NORMAL);
		rect.left += 16 + 2;
	}
	if (!m_items[lpDrawItemStruct->itemID].secondString.empty())
	{
		LPCTSTR secondString = m_items[lpDrawItemStruct->itemID].secondString.c_str();
		CRect rcCalc(rect);
		dc.DrawText(mainString, -1, &rcCalc, DT_CALCRECT | DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		//CRect trackRC(rcTest);
		if (rcCalc.Width() > rect.Width() + 30)
			dc.DrawText(mainString, -1, &rect, DT_END_ELLIPSIS | DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		else
		{
			dc.DrawText(mainString, -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			CRect secondRC(rect);
			secondRC.left = rcCalc.right + 10;
			COLORREF blueColor = RGB(GetRValue(textColor), GetGValue(textColor), GetBValue(textColor) > 128 ? 0 : 255);
			dc.SetTextColor(blueColor);
			dc.DrawText(secondString, -1, &secondRC, DT_END_ELLIPSIS | DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		}
	}
	else
	{
		dc.DrawText(mainString, -1, &rect, DT_END_ELLIPSIS | DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	}



	// Reset the background color and the text color back to their original values.
	dc.SetTextColor(crOldTextColor);
	dc.SetBkColor(crOldBkColor);

	dc.Detach();
}

void CDualTextComboBox::OnDeleteItem(int nIDCtl, LPDELETEITEMSTRUCT lpDeleteItemStruct)
{
	ASSERT(lpDeleteItemStruct->itemID < m_items.size());
	m_items.erase(m_items.begin() + lpDeleteItemStruct->itemID);
	CComboBox::OnDeleteItem(nIDCtl, lpDeleteItemStruct);
}


void CDualTextComboBox::PreSubclassWindow()
{
	DWORD style = GetStyle();
	//Set OwnerDraw Property to fixed
	ASSERT((style & CBS_OWNERDRAWFIXED) == CBS_OWNERDRAWFIXED);
	CComboBox::PreSubclassWindow();
}

void CDualTextComboBox::ResetContent()
{
	CComboBox::ResetContent();
	m_items.clear();
}

