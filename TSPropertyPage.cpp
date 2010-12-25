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
#include "TSPropertyPage.h"
#include "PrgAPI.h"


IMPLEMENT_DYNAMIC(CTSPropertyPage, CPropertyPage)

static const COLORREF bkColor = RGB(255,220,150);

CTSPropertyPage::CTSPropertyPage(UINT nIDTemplate, UINT nIDCaption):
CPropertyPage(nIDTemplate),
m_pPPParent(NULL),
m_resourceID(nIDCaption)
{
	m_resourceID = nIDCaption;
	if (m_resourceID != 0)
		m_Caption = PRGAPI()->GetString(m_resourceID);
	m_psp.pszTitle = m_Caption;
	m_psp.dwFlags |= PSP_USETITLE;
	m_EditBrush = CreateSolidBrush(bkColor);
}

HBRUSH CTSPropertyPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = NULL;//CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	if (nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_LISTBOX)
	{
		hbr = m_EditBrush;//(HBRUSH)GetStockObject(GRAY_BRUSH);
		//pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(bkColor);
		pDC->SetTextColor(RGB(0,0,50));
		//if (!m_bLocationEnabled)
		//{
		//	if (::GetWindowLong(pWnd->m_hWnd, GWL_STYLE) & ES_READONLY)
		//	{
		//		pDC->SetTextColor(RGB(200,0,0));
		//		pDC->SetBkColor(RGB(255,200,200));
		//		hbr = (HBRUSH)GetStockObject(GRAY_BRUSH);
		//	}
		//}
	}
	else
		hbr = CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}


CTSPropertyPage::~CTSPropertyPage()
{
	DeleteObject(m_EditBrush);
}

LPCTSTR CTSPropertyPage::GetTitle()
{
	ASSERT(m_resourceID != NULL);
	if (m_resourceID != 0)
		return PRGAPI()->GetString(m_resourceID);
	return _T("");
}

BOOL CTSPropertyPage::PreTranslateMessage(MSG* pMsg)
{
	//TRIPLE CLICK SUPPORT FOR ALL EDITS IN ALL PROPERTY PAGES
	if (IsDoubleClick(pMsg->message)) 
	{
		m_uWhichButton = IsDoubleClick(pMsg->message);	// save which button
		m_uTimeLastDblClk = clock();				// ..and current time
		// update double-click time. This is inefficient, but expedient because
		// don't have to worry about WM_SETTINGCHANGE.
		m_uClocksPerDblClk = GetDoubleClickTime() * CLOCKS_PER_SEC / 1000;

	} 
	else if (IsButtonDown(pMsg->message) && IsButtonDown(pMsg->message)==m_uWhichButton) 
	{
		CPropertyPage::WindowProc(pMsg->message, pMsg->wParam, pMsg->lParam);
		if ((clock() - m_uTimeLastDblClk) < m_uClocksPerDblClk) 
		{
			TCHAR bf[5];
			GetClassName(pMsg->hwnd, bf, 5);
			if (_tcsncmp(bf, _T("Edit"), 4)==0)
				::PostMessage(pMsg->hwnd, EM_SETSEL, 0, -1);
		}
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CTSPropertyPage::OnHelp()
{
	LPCTSTR ret = GetHelpID();
	if (ret != NULL)
		PRGAPI()->GoToSiteURL(PrgAPI::SA_Help, _T("propertypage"), ret);
}

