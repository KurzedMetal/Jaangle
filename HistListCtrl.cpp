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
#include "HistListCtrl.h"
#include "TeenSpirit.h"
#include "PrgAPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHistListCtrl::CHistListCtrl():
m_pFilter(NULL),
m_pHistDlg(NULL)
{}
CHistListCtrl::~CHistListCtrl()				
{
	m_MainMenu.DestroyMenu();
}

void CHistListCtrl::Init(HistoryDlgFilter* filter)
{
	ASSERT(filter != NULL);
	m_pFilter = filter;
	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES);
	SetImageList(&PRGAPI()->GetImageList16(), LVSIL_SMALL);
	InsertColumns();
	m_MainMenu.CreatePopupMenu();
	m_MainMenu.SetFont(PRGAPI()->GetFont(FONT_Dialogs));
	CreateMainMenu(m_MainMenu);
	HDITEM hdi;
	memset(&hdi, 0, sizeof(HDITEM));
	hdi.mask = HDI_FORMAT;
	hdi.fmt =  HDF_STRING | (GetSortingDesc() ? HDF_SORTDOWN : HDF_SORTUP);
	GetHeaderCtrl()->SetItem(GetSortingColumn(), &hdi);


}

BEGIN_MESSAGE_MAP(CHistListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &CHistListCtrl::OnLvnGetdispinfo)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CHistListCtrl::OnHdnItemclick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CHistListCtrl::OnNMCustomdraw)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void CHistListCtrl::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	LV_ITEM* pItem= &(pDispInfo)->item;
	int itemid = pItem->iItem;
	if( pItem->mask & LVIF_TEXT ) 
		GetDispInfo(pItem);
	*pResult = 0;
}
void CHistListCtrl::OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	if (phdr->iButton == 0) //Left Button
	{
		ToggleSorting(phdr->iItem);
		CHeaderCtrl* pHeader = GetHeaderCtrl();
		HDITEM hdi;
		memset(&hdi, 0, sizeof(HDITEM));
		hdi.mask = HDI_FORMAT;
		hdi.fmt = HDF_STRING;
		for (int i = 0; i < pHeader->GetItemCount(); i++)
			pHeader->SetItem(i, &hdi);
		hdi.fmt =  HDF_STRING | (GetSortingDesc() ? HDF_SORTDOWN : HDF_SORTUP);
		pHeader->SetItem(GetSortingColumn(), &hdi);
	}
	*pResult = 0;
}

void CHistListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	*pResult = CDRF_DODEFAULT;
	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
		pLVCD->clrTextBk = (pLVCD->nmcd.dwItemSpec & 1) ? RGB(255, 255, 255) : RGB(245, 245, 245);
		break;
	}
}

void CHistListCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point/*point*/)
{
	CRect rcCol;
	CHeaderCtrl* pHeader = GetHeaderCtrl();
	ASSERT(pHeader);
	pHeader->GetItemRect( 0, &rcCol);
	ClientToScreen(&rcCol);
	if (point.y > rcCol.bottom)
	{
		CPoint p = point;
		ScreenToClient(&p);
		INT item = HitTest(p);
		if (item != -1)
		{
			InitMainMenu(m_MainMenu);
			m_MainMenu.TrackPopupMenu(0, point.x, point.y, this);
		}
	}
}
