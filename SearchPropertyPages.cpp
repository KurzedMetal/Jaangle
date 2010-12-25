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
#include "SearchDlg.h"
#include "SearchPropertyPages.h"
#include "PrgAPI.h"


CSearchPropertyPages::CSearchPropertyPages(UINT nID):
CPropertyPage(nID),
m_pParent(NULL)
{
}

CSearchPropertyPages::~CSearchPropertyPages()
{
}

BOOL CSearchPropertyPages::OnInitDialog()
{
	CDialog::OnInitDialog();
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM)PRGAPI()->GetFont(FONT_Dialogs)->m_hObject);
	ApplyTranslation(*PRGAPI()->GetTranslationManager());

	//m_Info.SetBkMode(TRANSPARENT);
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CSearchPropertyPages::OnChange()
{
	m_pParent->SendMessage(WM_PAGEUPDATED);
}
BOOL CSearchPropertyPages::OnSetActive()
{
	OnChange();
	return CPropertyPage::OnSetActive();
}

BOOL CSearchPropertyPages::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_RETURN)
		{
			OnChange();
			return TRUE;
		}
		else if (pMsg->wParam == VK_ESCAPE)
		{
			m_pParent->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}
		break;
	}
	return CPropertyPage::PreTranslateMessage(pMsg);
}
