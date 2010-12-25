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
#include "OptionsSheet.h"
#include "PrgApi.h"
#include "TSPropertyPage.h"


void CButtonX::OnPaint()
{
	CButton::OnPaint();
	{
		CPoint p;
		GetCursorPos(&p);
		if (::WindowFromPoint(p) == m_hWnd)
		{
			CRect rc;
			GetClientRect(&rc);
			CClientDC dc(this);
			DrawIconEx(dc.m_hDC, 3, (rc.Height() - 16)/2, PRGAPI()->GetIcon(ICO_Internet16), 16, 16, 0, 0, DI_NORMAL);
		}
	}
}

// COptionsSheet

COptionsSheet::COptionsSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CTreePropSheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_nIDCaption = nIDCaption;
	SetTreeViewMode(TRUE, TRUE);//, TRUE);
	SetTreeWidth(200);

}

COptionsSheet::~COptionsSheet()
{
}
// COptionsSheet message handlers
void COptionsSheet::ApplyTranslation(ITranslation& translation)
{
	if (m_hWnd)
	{
		TCITEM it;
		it.mask = TCIF_TEXT;
		CTabCtrl* pTab = GetTabControl();
		ASSERT(pTab->GetSafeHwnd());
		if (pTab->GetSafeHwnd() == NULL)
			return;
		int curSel = pTab->GetCurSel();
		PrgAPI* pAPI = PRGAPI();
		for (UINT i = 0; i < m_pages.size(); i++)
		{
			it.pszText = (LPTSTR)m_pages[i]->GetTitle();
			pTab->SetItem(i, &it);
			//m_pages[i]->ApplyTranslation();
		}
		RefillPageTree();
		SelectPageTreeItem(curSel);
		SetWindowText(pAPI->GetString(m_nIDCaption));

		SetDlgItemText(IDOK, pAPI->GetString(IDS_OK));
		SetDlgItemText(IDCANCEL, pAPI->GetString(IDS_CANCEL_S));
		SetDlgItemText(IDHELP, pAPI->GetString(IDS_HELP_S));
		//=== Code to get the 12321
		//HWND hwnd = ::FindWindowEx(m_hWnd, NULL, _T("BUTTON"), _T("&Apply"));
		//INT id = ::GetDlgCtrlID(hwnd);
		SetDlgItemText(12321, pAPI->GetString(IDS_APPLY));
	}
}


BOOL COptionsSheet::OnInitDialog()
{
	//EnableStackedTabs(FALSE);
	BOOL bResult = CTreePropSheet::OnInitDialog();
	CTreeCtrl* pTree = GetPageTreeControl();
	m_psh.dwFlags |= PSH_HASHELP;

	pTree->SetBkColor(RGB(248,248,248));
	pTree->SetTextColor(RGB(0,0,0));
	pTree->SetIndent(0);
	//pTree->ModifyStyle(TVS_HASLINES | TVS_LINESATROOT, 0);
	
	PrgAPI* pAPI = PRGAPI();
	ApplyTranslation(*pAPI->GetTranslationManager());
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM) pAPI->GetFont(FONT_Dialogs)->m_hObject);

	m_header.SetDraggable(TRUE);
	m_header.SetIconSize(48, 48);
	//m_header.SetHeaderHeight(56);
	m_header.SetTitleText(pAPI->GetString(IDS_OPTIONS));
	m_header.SetTitleFont(pAPI->GetFont(FONT_DialogsBigBold));
	m_header.SetDescFont(pAPI->GetFont(FONT_Dialogs));
	m_header.Init(this);
	m_header.MoveCtrls(this);


	CWnd* pWndHelp = GetDlgItem(IDHELP);
	if (pWndHelp != NULL)
	{
		m_helpButton.SubclassWindow(pWndHelp->m_hWnd);
		m_helpButton.SetButtonStyle(BS_RIGHT);
	//	//pWndHelp->SendMessage(BM_SETIMAGE, IMAGE_ICON, (LPARAM)PRGAPI()->GetIcon(ICO_Internet16));
	//	pWndHelp->SetFont(PRGAPI()->GetFont(FONT_DialogsItalics));
	}


	//std::tstring subTitle;
	//for (size_t i = 0; i < m_pages.size(); i++)
	//{
	//	subTitle += m_pages[i]->GetTitle();
	//	if (i < m_pages.size() - 1)
	//		subTitle += _T(", ");
	//}
	//m_header.SetDescText(subTitle.c_str());
	if (m_header.GetIconHandle() == NULL)
		m_header.SetIconHandle(pAPI->GetIcon(ICO_Main));

	return bResult;
}

void COptionsSheet::AddTSPropertyPage(CTSPropertyPage* page)
{
	m_pages.push_back(page);
	if (page->GetHelpID() != NULL)
		page->m_psp.dwFlags |= PSP_HASHELP;
	CTreePropSheet::AddPage(page);

	//page->m_psp.dwFlags |= PSH_HASHELP;
	//page->m_psp.dwFlags |= PSP_USEHICON;
	//page->m_psp.hIcon = PRGAPI()->GetIcon(ICO_Add16);
}

void COptionsSheet::OnHelp()
{
	((CTSPropertyPage*)GetActivePage())->OnHelp();
}


BEGIN_MESSAGE_MAP(CButtonX, CButton)
	ON_WM_PAINT()
END_MESSAGE_MAP()


BEGIN_MESSAGE_MAP(COptionsSheet, TreePropSheet::CTreePropSheet)
	ON_BN_CLICKED(IDHELP, &COptionsSheet::OnHelp)
	//ON_WM_PAINT()
END_MESSAGE_MAP()


