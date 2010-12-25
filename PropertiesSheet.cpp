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
#include "PropertiesSheet.h"
#include "PrgAPI.h"

// CPropertiesSheet

IMPLEMENT_DYNAMIC(CPropertiesSheet, CTreePropSheet)

CPropertiesSheet::CPropertiesSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CTreePropSheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CPropertiesSheet::CPropertiesSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CTreePropSheet(pszCaption, pParentWnd, iSelectPage)
{
}


CPropertiesSheet::~CPropertiesSheet()
{
}


BEGIN_MESSAGE_MAP(CPropertiesSheet, CTreePropSheet)
END_MESSAGE_MAP()

BOOL CPropertiesSheet::OnInitDialog ()
{
	BOOL bResult = CTreePropSheet::OnInitDialog ();

	CTreeCtrl* pTree = GetPageTreeControl();
	pTree->SetBkColor(RGB(248,248,248));
	pTree->SetTextColor(RGB(0,0,0));
	pTree->SetIndent(0);


	PrgAPI* pAPI = PRGAPI();
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM)pAPI->GetFont(FONT_Dialogs)->m_hObject);
	m_header.SetDraggable(TRUE);
	m_header.SetIconSize(48, 48);
	m_header.SetTitleFont(pAPI->GetFont(FONT_DialogsBigBold));
	m_header.SetDescFont(pAPI->GetFont(FONT_Dialogs));

	if (m_header.GetIconHandle() == NULL)
		m_header.SetIconHandle(pAPI->GetIcon(ICO_Main));
	m_header.Init(this);
	m_header.MoveCtrls(this);


	return bResult;
}

