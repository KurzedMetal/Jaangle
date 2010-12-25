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
#include "MyToolBar.h"
#include ".\mytoolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrueColorToolBar

CMyToolBar::CMyToolBar()
{
	m_bDropDown = FALSE;
}

CMyToolBar::~CMyToolBar()
{
}


BEGIN_MESSAGE_MAP(CMyToolBar, CToolBar)
	//{{AFX_MSG_MAP(CTrueColorToolBar)
	ON_NOTIFY_REFLECT(TBN_DROPDOWN, OnToolbarDropDown)
	//}}AFX_MSG_MAP
	//ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrueColorToolBar message handlers
BOOL CMyToolBar::LoadTrueColorToolBar(int  nBtnWidth,
											 UINT uToolBar,
											 UINT uToolBarHot,
											 UINT uToolBarDisabled,
											 COLORREF rgbMask)
{
	if (!SetTrueColorToolBar(TB_SETIMAGELIST, uToolBar, nBtnWidth, rgbMask))
		return FALSE;
	
	if (uToolBarHot) {
		if (!SetTrueColorToolBar(TB_SETHOTIMAGELIST, uToolBarHot, nBtnWidth, rgbMask))
			return FALSE;
	}

	if (uToolBarDisabled) {
		if (!SetTrueColorToolBar(TB_SETDISABLEDIMAGELIST, uToolBarDisabled, nBtnWidth, rgbMask))
			return FALSE;
	}

	return TRUE;
}


BOOL CMyToolBar::SetTrueColorToolBar(UINT uToolBarType, 
							     	        UINT uToolBar,
										    int  nBtnWidth, 
											COLORREF rgbMask)
{
	CImageList	cImageList;
	CBitmap		cBitmap;
	BITMAP		bmBitmap;
	
	if (!cBitmap.Attach(LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(uToolBar),
								  IMAGE_BITMAP, 0, 0,
								  LR_DEFAULTSIZE|LR_CREATEDIBSECTION)) ||
	    !cBitmap.GetBitmap(&bmBitmap))
		return FALSE;

	CSize		cSize(bmBitmap.bmWidth, bmBitmap.bmHeight); 
	int			nNbBtn	= cSize.cx/nBtnWidth;
	RGBTRIPLE*	rgb		= (RGBTRIPLE*)(bmBitmap.bmBits);
	//COLORREF rgbMask = RGB(255,0,255);
	if (!cImageList.Create(nBtnWidth, cSize.cy, ILC_COLOR24|ILC_MASK, nNbBtn, 0))
		return FALSE;
	
	if (cImageList.Add(&cBitmap, rgbMask) == -1)
		return FALSE;

	SendMessage(uToolBarType, 0, (LPARAM)cImageList.m_hImageList);
	cImageList.Detach(); 
	cBitmap.Detach();
	
	return TRUE;
}

void CMyToolBar::AddDropDownButton(CWnd* pParent, UINT uButtonID, UINT uMenuID)
{
	if (!m_bDropDown) {
		GetToolBarCtrl().SendMessage(TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_DRAWDDARROWS);
		m_bDropDown = TRUE;
	}

	SetButtonStyle(CommandToIndex(uButtonID), TBSTYLE_DROPDOWN);

	stDropDownInfo DropDownInfo;
	DropDownInfo.pParent	= pParent;
	DropDownInfo.uButtonID	= uButtonID;
	DropDownInfo.uMenuID	= uMenuID;
	m_lstDropDownButton.Add(DropDownInfo);
}

void CMyToolBar::OnToolbarDropDown(NMHDR* _pnmtb, LRESULT *plr)
{
	NMTOOLBAR * pnmtb = (NMTOOLBAR*)_pnmtb;
	for (int i = 0; i < m_lstDropDownButton.GetSize(); i++) {
		
		stDropDownInfo DropDownInfo = m_lstDropDownButton.GetAt(i);

		if (DropDownInfo.uButtonID == UINT(pnmtb->iItem)) {

			CMenu menu;
			menu.LoadMenu(DropDownInfo.uMenuID);
			CMenu* pPopup = menu.GetSubMenu(0);
			
			CRect rc;
			SendMessage(TB_GETRECT, (WPARAM)pnmtb->iItem, (LPARAM)&rc);
			ClientToScreen(&rc);
			
			pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,
				                   rc.left, rc.bottom, DropDownInfo.pParent, &rc);
			break;
		}
	}
}
//void CMyToolBar::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
//	HDC m_BackgroundDC = ::CreateCompatibleDC(pNMCD->hdc);
//	HBITMAP m_bmp = ::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(140));
//	::SelectObject(m_BackgroundDC, m_bmp);
//	::BitBlt(pNMCD->hdc, 
//		pNMCD->rc.left, pNMCD->rc.top, pNMCD->rc.right, pNMCD->rc.bottom,
//		m_BackgroundDC, 0, 0, SRCCOPY);
//	::DeleteObject(m_bmp);
//	::DeleteDC(m_BackgroundDC);
//	//::Rectangle(pNMCD->hdc, pNMCD->rc.left, pNMCD->rc.top, pNMCD->rc.right, pNMCD->rc.bottom);
//
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}
