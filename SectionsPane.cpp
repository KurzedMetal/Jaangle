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
#include "SectionsPane.h"
#include "MainTreeCtrl.h"
#include "PrgAPI.h"
//#include "MenuManager.h"
#include "ShortcutManager.h"
#include "SQLManager.h"
//#include "StringSerializerHelper.h"
#include "CollectionManager.h"


enum Buttons
{
	BTN_Options,
	BTN_Last
};

SectionsPane::SectionsPane()
{
	memset(&m_pMenu, 0, sizeof(m_pMenu));
}

SectionsPane::~SectionsPane()
{
	ResetAllMenus();
	for (int i = 0; i < MNU_Last; i++)
	{
		if (m_pMenu[i] != NULL)
			delete m_pMenu[i];
	}

}

void SectionsPane::ResetAllMenus()
{
	for (int i = 0; i < MNU_Last; i++)
	{
		if (m_pMenu[i] != NULL)
			m_pMenu[i]->Destroy();
	}
}

BOOL SectionsPane::Init(HWND hwndParent)
{
	if (m_treeControl.m_hWnd == NULL)
	{
		//=== Create and configure the Tree Control
		PrgAPI* pAPI = PRGAPI();
		m_treeControl.Create(WS_CHILD, CRect(0,0,0,0), CWnd::FromHandle(hwndParent), 345);
		m_treeControl.EnableDragDrop(TRUE);
		m_treeControl.SetExpandButton(pAPI->GetIcon(ICO_Expanded16), pAPI->GetIcon(ICO_Collapsed16), 16, 16);
		m_treeControl.GetScrollBar().SetIcons(pAPI->GetIcon(ICO_Up16), pAPI->GetIcon(ICO_Down16), 16, 16);
	}
	return TRUE;
}

void SectionsPane::UnInit()
{
	Hide();
	m_treeControl.DestroyWindow();
}
BOOL SectionsPane::Show()
{
	ASSERT(m_treeControl.GetSafeHwnd());
	if (m_treeControl.GetSafeHwnd() == NULL)
		return FALSE;
	if (m_treeControl.IsWindowVisible() == FALSE)
	{
		PrgAPI* pAPI = PRGAPI();
		pAPI->GetShortcutManager()->RegisterShortcutForFocus(VK_F1, m_treeControl.m_hWnd);
		pAPI->GetStateManager()->RegisterStateChangeListener(*this);
		TreeNode& root = m_treeControl.GetRootNode();
		if (root.bExpanded == FALSE)
			m_treeControl.Expand(root, TRUE);
		m_treeControl.ShowWindow(SW_SHOW);
	}
	return TRUE;
}
void SectionsPane::Hide()
{
	if (m_treeControl.GetSafeHwnd() == NULL)
		return;
	if (m_treeControl.IsWindowVisible() == TRUE)
	{
		m_treeControl.ShowWindow(SW_HIDE);
		PrgAPI* pAPI = PRGAPI();
		pAPI->GetShortcutManager()->UnRegisterShortcutForFocus(m_treeControl.m_hWnd);
		pAPI->GetStateManager()->UnRegisterStateChangeListener(*this);
	}
}

void SectionsPane::MovePane(INT x, INT y, INT cx, INT cy)
{
	ASSERT(m_treeControl.GetSafeHwnd());
	if (m_treeControl.GetSafeHwnd())
		m_treeControl.MoveWindow(x, y, cx, cy, FALSE);
}

const LPCTSTR cSectionsPane_Header = _T("Header");
const LPCTSTR cSectionsPane_Sorting = _T("Sorting");
const LPCTSTR cSectionsPane_Mode = _T("Mode");
const LPCTSTR cSectionsPane_GroupSmallSections = _T("GroupSmallSections");
const LPCTSTR cSectionsPane_SmallSectionLimit = _T("SmallSectionLimit");
const LPCTSTR cSectionsPane_SectionStyle = _T("SectionStyle");
const LPCTSTR cSectionsPane_SectionFilter = _T("SectionFilter");
const LPCTSTR cSectionsPane_ScrollPos = _T("ScrollPos");
const LPCTSTR cSectionsPane_Depth = _T("Depth");

void SectionsPane::ReadSortOption(IPaneState& paneState, SectionTypeEnum st)
{
	TCHAR bf[500];
	_sntprintf(bf, 500, _T("%s_%d"), cSectionsPane_Sorting, st);
	INT rso = paneState.GetIntSetting(bf);
	if (rso > -RSO_Last && rso < RSO_Last)
		m_treeControl.SetSort(st, SortOption((RecordSortOptionsEnum)(rso > 0 ? rso : - rso), rso > 0));
}

void SectionsPane::WriteSortOption(IPaneState& paneState, SectionTypeEnum st)
{
	TCHAR bf[500];
	_sntprintf(bf, 500, _T("%s_%d"), cSectionsPane_Sorting, st);
	SortOption so = m_treeControl.GetSort(st);
	paneState.SetIntSetting(bf, INT(so.option) * (so.ascending ? 1 : -1));
}

BOOL SectionsPane::LoadState(IPaneState& paneState)
{
	LPCTSTR sValue = paneState.GetLPCTSTRSetting(cSectionsPane_Header);
	if (_tcsicmp(sValue, SectionsPaneInfo.Name) != 0)
		return FALSE;
	ReadSortOption(paneState, ST_Artist);
	ReadSortOption(paneState, ST_Album);
	ReadSortOption(paneState, ST_Genre);
	ReadSortOption(paneState, ST_Collection);
	ReadSortOption(paneState, ST_Year);
	ReadSortOption(paneState, ST_MonthAdded);

	INT rso = paneState.GetIntSetting(cSectionsPane_Mode);
	if (rso > SMODE_First && rso < SMODE_Last)
		m_treeControl.SetMode((SectionModesEnum)rso);

	rso = paneState.GetIntSetting(cSectionsPane_GroupSmallSections);
	m_treeControl.GroupSmallSections(rso != 0);

	rso = paneState.GetIntSetting(cSectionsPane_SmallSectionLimit);
	m_treeControl.SetSmallSectionLimit(rso);

	rso = paneState.GetIntSetting(cSectionsPane_SectionStyle);
	if (rso > SSTYL_First && rso < SSTYL_Last)
		m_treeControl.SetStyle((SectionStylesEnum)rso);

	rso = paneState.GetIntSetting(cSectionsPane_SectionFilter);
	if (rso > SFILT_First && rso < SFILT_Last)
		m_treeControl.SetFilter((SectionFiltersEnum)rso);

	rso = paneState.GetIntSetting(cSectionsPane_ScrollPos);
	if (rso > 0 && rso < 10000)
		m_treeControl.SetScrollPos(rso);


	INT nDepth = paneState.GetIntSetting(cSectionsPane_Depth);
	if (nDepth > -1 && nDepth < 100)
	{
		TreeNode* pParentNode = &m_treeControl.GetRootNode();
		m_treeControl.Expand(*pParentNode, TRUE);
		for (INT i = 0; i < nDepth; i++)
		{
			TCHAR bf[500];
			_sntprintf(bf, 500, _T("%s_%d"), cSectionsPane_Depth, i);
			rso = paneState.GetIntSetting(bf);
			TreeNode* pChildNode = pParentNode->GetChild(rso);
			if (pChildNode != NULL)
			{
				m_treeControl.Expand(*pChildNode, TRUE);
				if (i == nDepth - 1)
					m_treeControl.SelectNode(pChildNode);
				pParentNode = pChildNode;
			}
			else
			{
				m_treeControl.SelectNode(pParentNode);
			}
		}
	}
	else
		m_treeControl.Expand(m_treeControl.GetRootNode(), TRUE);


	return TRUE;
}



BOOL SectionsPane::SaveState(IPaneState& paneState)
{
	paneState.SetLPCTSTRSetting(cSectionsPane_Header, SectionsPaneInfo.Name);
	WriteSortOption(paneState, ST_Artist);
	WriteSortOption(paneState, ST_Album);
	WriteSortOption(paneState, ST_Genre);
	WriteSortOption(paneState, ST_Collection);
	WriteSortOption(paneState, ST_Year);
	WriteSortOption(paneState, ST_MonthAdded);

	paneState.SetIntSetting(cSectionsPane_Mode, (INT) m_treeControl.GetMode());
	paneState.SetIntSetting(cSectionsPane_GroupSmallSections, (INT) m_treeControl.IsSmallSectionsGrouped());
	paneState.SetIntSetting(cSectionsPane_SmallSectionLimit, (INT) m_treeControl.GetSmallSectionLimit());
	paneState.SetIntSetting(cSectionsPane_SectionStyle, (INT) m_treeControl.GetStyle());
	paneState.SetIntSetting(cSectionsPane_SectionFilter, (INT) m_treeControl.GetFilter());
	paneState.SetIntSetting(cSectionsPane_ScrollPos, (INT) m_treeControl.GetScrollPos());



	std::vector<INT> selIndex;
	TreeNode* pNode = m_treeControl.GetSelectedNode();
	while (pNode != NULL)
	{
		TreeNode* pParentNode = pNode->pParent;
		if (pParentNode != NULL)
		{
			selIndex.push_back(pParentNode->GetChildIndex(pNode));
		}
		pNode = pParentNode;
	}
	paneState.SetIntSetting(cSectionsPane_Depth, (INT) selIndex.size());
	for (INT i = 0; i < (INT)selIndex.size(); i++)
	{
		TCHAR bf[500];
		_sntprintf(bf, 500, _T("%s_%d"), cSectionsPane_Depth, i);
		paneState.SetIntSetting(bf, (INT) selIndex[selIndex.size() - i - 1]);
	}
	return TRUE;
}

LPCTSTR SectionsPane::GetTitle(UINT idx)
{
	PrgAPI* pAPI = PRGAPI();
	switch (idx %4)
	{
	case 0:
		return pAPI->GetString(IDS_MUSICBROWSER);
	case 1:
		_sntprintf(m_stringBuffer, 500, _T("%s: %s"), 
			pAPI->GetString(IDS_FILTER),
			pAPI->GetStringForSectionFilter(m_treeControl.GetFilter()));
		break;
	case 2:
		{
			INT curLen = _sntprintf(m_stringBuffer, 500, _T("%s: "), pAPI->GetString(IDS_MODE));
			pAPI->GetStringForSectionMode(m_treeControl.GetMode(), &m_stringBuffer[curLen], 500 - curLen - 1);
		}
		break;
	case 3:
		_sntprintf(m_stringBuffer, 500, _T("%s: [F1]"), pAPI->GetString(IDS_FOCUS));
		break;
	default:
		ASSERT(0);
		return NULL;
	}
	return m_stringBuffer;
}

BOOL SectionsPane::GetButtonInfo(PaneButtonInfo& bInfo, UINT idx)
{
	switch (idx)
	{
	case BTN_Options:
		bInfo = PaneButtonInfo();
		bInfo.hIcon = PRGAPI()->GetIcon(ICO_Options16);
		bInfo.iconSize = 16;
		break;
	default:
		return FALSE;
	}
	return TRUE;

}
ITSMenu* SectionsPane::GetMenu(UINT idx)
{
	switch (idx )
	{
	case BTN_Options:
		{
			//MenuManager* pMM = PRGAPI()->GetMenuManager();
			ITSMenu& mnu = GetInternalMenu(MNU_SectionOptionsMenu);
			mnu.CheckMenuRadioItem(MENU_SectionModesHolder, 
				MENU_SectionModesHolder + SMODE_Last,
				MENU_SectionModesHolder + m_treeControl.GetMode());
			//mnu.CheckMenuRadioItem(MENU_SectionSortsHolder, MENU_SectionSortsHolderEnd,
			//	0, MF_BYCOMMAND);
			for (INT i = MENU_SectionSortsHolder; i <MENU_SectionSortsHolderEnd ; i++)
				mnu.CheckMenuItem(i, MF_BYCOMMAND | MF_UNCHECKED);

			SortOption so = m_treeControl.GetSort(ST_Artist);
			INT MenuCmdID = 0;
			switch (so.option)
			{
			case RSO_Title:			MenuCmdID = MENU_SectionSortsArtName;	break;
			case RSO_Rating:		MenuCmdID = MENU_SectionSortsArtRating;	break;
			case RSO_TrackCount:	MenuCmdID = MENU_SectionSortsArtCount;	break;
			default:				ASSERT(0);MenuCmdID=0;
			}
			mnu.CheckMenuItem(MenuCmdID, MF_BYCOMMAND | MF_CHECKED);
			mnu.CheckMenuItem(MENU_SectionSortsArtDesc, MF_BYCOMMAND | (so.ascending ? MF_UNCHECKED : MF_CHECKED));

			so = m_treeControl.GetSort(ST_Album);
			switch (so.option)
			{
			case RSO_Title:			MenuCmdID = MENU_SectionSortsAlbName;	break;
			case RSO_Rating:		MenuCmdID = MENU_SectionSortsAlbRating;	break;
			case RSO_Year:			MenuCmdID = MENU_SectionSortsAlbYear;	break;
			case RSO_TrackCount:	MenuCmdID = MENU_SectionSortsAlbCount;	break;
			default:				ASSERT(0);MenuCmdID=0;
			}
			mnu.CheckMenuItem(MenuCmdID, MF_BYCOMMAND | MF_CHECKED);
			mnu.CheckMenuItem(MENU_SectionSortsAlbDesc, MF_BYCOMMAND | (so.ascending ? MF_UNCHECKED : MF_CHECKED));

			so = m_treeControl.GetSort(ST_Genre);
			switch (so.option)
			{
			case RSO_Title:			MenuCmdID = MENU_SectionSortsGenName;	break;
			case RSO_TrackCount:	MenuCmdID = MENU_SectionSortsGenCount;	break;
			default:				ASSERT(0);MenuCmdID=0;
			}
			mnu.CheckMenuItem(MenuCmdID, MF_BYCOMMAND | MF_CHECKED);
			mnu.CheckMenuItem(MENU_SectionSortsGenDesc, MF_BYCOMMAND | (so.ascending ? MF_UNCHECKED : MF_CHECKED));

			so = m_treeControl.GetSort(ST_Collection);
			switch (so.option)
			{
			case RSO_Title:			MenuCmdID = MENU_SectionSortsColName;	break;
			case RSO_TrackCount:	MenuCmdID = MENU_SectionSortsColCount;	break;
			default:				ASSERT(0);MenuCmdID=0;
			}
			mnu.CheckMenuItem(MenuCmdID, MF_BYCOMMAND | MF_CHECKED);
			mnu.CheckMenuItem(MENU_SectionSortsColDesc, MF_BYCOMMAND | (so.ascending ? MF_UNCHECKED : MF_CHECKED));

			so =m_treeControl.GetSort(ST_Year);
			switch (so.option)
			{
			case RSO_Title:			MenuCmdID = MENU_SectionSortsYeaName;	break;
			case RSO_TrackCount:	MenuCmdID = MENU_SectionSortsYeaCount;	break;
			default:				ASSERT(0);MenuCmdID=0;
			}
			mnu.CheckMenuItem(MenuCmdID, MF_BYCOMMAND | MF_CHECKED);
			mnu.CheckMenuItem(MENU_SectionSortsYeaDesc, MF_BYCOMMAND | (so.ascending ? MF_UNCHECKED : MF_CHECKED));


			so = m_treeControl.GetSort(ST_MonthAdded);
			switch (so.option)
			{
			case RSO_Title:			MenuCmdID = MENU_SectionSortsMonName;	break;
			case RSO_TrackCount:	MenuCmdID = MENU_SectionSortsMonCount;	break;
			default:				ASSERT(0);MenuCmdID=0;
			}
			mnu.CheckMenuItem(MenuCmdID, MF_BYCOMMAND | MF_CHECKED);
			mnu.CheckMenuItem(MENU_SectionSortsMonDesc, MF_BYCOMMAND | (so.ascending ? MF_UNCHECKED : MF_CHECKED));



			mnu.CheckMenuRadioItem(MENU_SectionFiltersHolder + SFILT_First,
				MENU_SectionFiltersHolder + SFILT_Last,
				MENU_SectionFiltersHolder + m_treeControl.GetFilter()); 

			mnu.CheckMenuRadioItem(MENU_SectionStylesHolder + SSTYL_First,
				MENU_SectionStylesHolderEnd,
				MENU_SectionStylesHolder + (INT)m_treeControl.GetStyle()); 


			mnu.CheckMenuRadioItem(MENU_SectionGroupingHolder,
				MENU_SectionGroupingHolderEnd,
				MENU_SectionGroupingHolder + m_treeControl.GetSmallSectionLimit()); 

			mnu.CheckMenuItem(MENU_SectionGroupingHolder, MF_BYCOMMAND | (m_treeControl.IsSmallSectionsGrouped() ? MF_CHECKED : MF_UNCHECKED));
			return &mnu;
		}
	default:
		break;
	}
	ASSERT(0);
	return NULL;
}


BOOL SectionsPane::OnButton(UINT idx)
{
	TRACEST(_T("SectionsPane::OnControlButtonCommand"), idx);
	PrgAPI* pAPI = PRGAPI();
	if (idx > MENU_SectionModesHolder && idx < MENU_SectionModesHolder + SMODE_Last)
	{
		m_treeControl.SetMode((SectionModesEnum)(idx - MENU_SectionModesHolder));
		m_treeControl.Reset();
		//Invalidate(FALSE);
	}
	else if (idx > MENU_SectionSortsHolder && idx < MENU_SectionSortsHolderEnd)
	{
		switch (idx)
		{
		case MENU_SectionSortsArtName:
			m_treeControl.SetSort(ST_Artist, SortOption(RSO_Title, TRUE));
			break;
		case MENU_SectionSortsArtRating:
			m_treeControl.SetSort(ST_Artist, SortOption(RSO_Rating, FALSE));
			break;
		case MENU_SectionSortsArtCount:
			m_treeControl.SetSort(ST_Artist, SortOption(RSO_TrackCount, FALSE));
			break;
		case MENU_SectionSortsArtDesc:
			{
				SortOption so = m_treeControl.GetSort(ST_Artist);
				so.ascending = !so.ascending; 
				m_treeControl.SetSort(ST_Artist, so);
			}
			break;

		case MENU_SectionSortsAlbName:
			m_treeControl.SetSort(ST_Album, SortOption(RSO_Title, TRUE));
			break;
		case MENU_SectionSortsAlbRating:
			m_treeControl.SetSort(ST_Album, SortOption(RSO_Rating, FALSE));
			break;
		case MENU_SectionSortsAlbCount:
			m_treeControl.SetSort(ST_Album, SortOption(RSO_TrackCount, FALSE));
			break;
		case MENU_SectionSortsAlbYear:
			m_treeControl.SetSort(ST_Album, SortOption(RSO_Year, FALSE));
			break;
		case MENU_SectionSortsAlbDesc:
			{
				SortOption so = m_treeControl.GetSort(ST_Album);
				so.ascending = !so.ascending; 
				m_treeControl.SetSort(ST_Album, so);
			}
			break;

		case MENU_SectionSortsColName:
			m_treeControl.SetSort(ST_Collection, SortOption(RSO_Title, TRUE));
			break;
		case MENU_SectionSortsColCount:
			m_treeControl.SetSort(ST_Collection, SortOption(RSO_TrackCount, FALSE));
			break;
		case MENU_SectionSortsColDesc:
			{
				SortOption so = m_treeControl.GetSort(ST_Collection);
				so.ascending = !so.ascending; 
				m_treeControl.SetSort(ST_Collection, so);
			}
			break;

		case MENU_SectionSortsGenName:
			m_treeControl.SetSort(ST_Genre, SortOption(RSO_Title, TRUE));
			break;
		case MENU_SectionSortsGenCount:
			m_treeControl.SetSort(ST_Genre, SortOption(RSO_TrackCount, FALSE));
			break;
		case MENU_SectionSortsGenDesc:
			{
				SortOption so = m_treeControl.GetSort(ST_Genre);
				so.ascending = !so.ascending; 
				m_treeControl.SetSort(ST_Genre, so);
			}
			break;

		case MENU_SectionSortsYeaName:
			m_treeControl.SetSort(ST_Year, SortOption(RSO_Title, TRUE));
			break;
		case MENU_SectionSortsYeaCount:
			m_treeControl.SetSort(ST_Year, SortOption(RSO_TrackCount, FALSE));
			break;
		case MENU_SectionSortsYeaDesc:
			{
				SortOption so = m_treeControl.GetSort(ST_Year);
				so.ascending = !so.ascending; 
				m_treeControl.SetSort(ST_Year, so);
			}
			break;


		case MENU_SectionSortsMonName:
			m_treeControl.SetSort(ST_MonthAdded, SortOption(RSO_Title, TRUE));
			break;
		case MENU_SectionSortsMonCount:
			m_treeControl.SetSort(ST_MonthAdded, SortOption(RSO_TrackCount, FALSE));
			break;
		case MENU_SectionSortsMonDesc:
			{
				SortOption so = m_treeControl.GetSort(ST_MonthAdded);
				so.ascending = !so.ascending; 
				m_treeControl.SetSort(ST_MonthAdded, so);
			}
			break;

		case MENU_SectionGroupingHolder:
			m_treeControl.GroupSmallSections(!m_treeControl.IsSmallSectionsGrouped());
			break;
		default:
			ASSERT(0);
		}
		m_treeControl.Reset();
		//Invalidate(FALSE);
	}
	else if (idx > MENU_SectionFiltersHolder + SFILT_First && idx < MENU_SectionFiltersHolder + SFILT_Last)
	{
		m_treeControl.SetFilter((SectionFiltersEnum) (idx - MENU_SectionFiltersHolder));
		m_treeControl.Reset();
		//Invalidate(FALSE);
	}
	else if (idx > MENU_SectionStylesHolder + SSTYL_First && idx < MENU_SectionStylesHolder + SSTYL_Last)
	{
		m_treeControl.SetStyle((SectionStylesEnum) (idx - MENU_SectionStylesHolder));
	}
	else if (idx >= MENU_SectionGroupingHolder && idx < MENU_SectionGroupingHolderEnd)
	{
		if (idx == MENU_SectionGroupingHolder)
			m_treeControl.GroupSmallSections(!m_treeControl.IsSmallSectionsGrouped());
		else
			m_treeControl.SetSmallSectionLimit(idx - MENU_SectionGroupingHolder);
		m_treeControl.Reset();
	}
	else
		return FALSE;
	return TRUE;
}

BOOL SectionsPane::OnStateChanged(UINT stateMessage)
{
	//PrgAPI* pAPI = PRGAPI();
	//SQLManager* pSM = pAPI->GetSQLManager();
	BOOL bHandled = TRUE;
	StateMessageEnum msg = (StateMessageEnum) stateMessage;
	switch(msg)
	{
	case SM_CollectionManagerEvent:
		{
			CollectionManager* pCM = PRGAPI()->GetCollectionManager();
			CollectionManager::State state = pCM->GetState();
			if (state == CollectionManager::S_Idle)
			{
				m_treeControl.EnableAutoUpdates(FALSE);
				m_treeControl.Update(TRUE);
			}
			else
			{
				m_treeControl.EnableAutoUpdates(TRUE);
			}
			//m_treeControl.Update(TRUE);
		}
		break;
	case SM_DatabaseUpdated:
		m_treeControl.Update(TRUE);
		break;
	case SM_PictureChanged:
		m_treeControl.Update(FALSE);
		break;
	case SM_LocateTrackRequest:
		{
			PrgAPI* pAPI = PRGAPI();
			UINT lookID = pAPI->GetStateManager()->GetState().locateTrackID;
			FullTrackRecordSP rec;
			if (pAPI->GetSQLManager()->GetFullTrackRecordByID(rec, lookID))
			{
				m_treeControl.SelectNodeFromFullTrackRecord(rec, ST_Album);
			}
		}
		break;
	default:
		bHandled = FALSE;
	}
	return bHandled;
}

ITSMenu& SectionsPane::GetInternalMenu(MENUS mnu)
{
	TRACEST(_T("SectionsPane::GetMenu."), mnu);
	ASSERT(mnu < MNU_Last);
	PrgAPI* pAPI = PRGAPI();
	if (m_pMenu[mnu] == NULL)
		m_pMenu[mnu] = pAPI->CreatePopupMenu();
	ITSMenu& curMenu = *m_pMenu[mnu];
	if (curMenu.GetInternalHandler() != NULL)
		return curMenu;
	curMenu.Create();
	switch (mnu)
	{
	case MNU_SectionOptionsMenu:
		{
			curMenu.AppendMenu(ITSMenu::MIT_Menu, 
				(INT)GetInternalMenu(MNU_SectionModesMenu).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_MODE));
			curMenu.AppendMenu(ITSMenu::MIT_Menu, 
				(INT)GetInternalMenu(MNU_SectionSortsMenu).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_SORT));
			curMenu.AppendMenu(ITSMenu::MIT_Menu, 
				(INT)GetInternalMenu(MNU_SectionFilterMenu).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_FILTER));
			curMenu.AppendMenu(ITSMenu::MIT_Menu, 
				(INT)GetInternalMenu(MNU_SectionGroupingMenu).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_ITEMGROUPING));
			curMenu.AppendMenu(ITSMenu::MIT_Menu, 
				(INT)GetInternalMenu(MNU_SectionStylesMenu).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_STYLE));
		}
		break;
	case MNU_SectionModesMenu:
		{
			TCHAR bf[500];
			for (int i = SMODE_First + 1; i < SMODE_Last; i++)
			{
				pAPI->GetStringForSectionMode((SectionModesEnum)i, bf, 500);
				curMenu.AppendMenu(ITSMenu::MIT_String, i + MENU_SectionModesHolder, bf);
			}
		}
		break;
	case MNU_SectionSortsMenu:
		{
			curMenu.AppendMenu(ITSMenu::MIT_Menu, 
				(INT)GetInternalMenu(MNU_SectionSortArtistMenu).GetInternalHandler(), 
				(LPTSTR)pAPI->GetString(IDS_ARTIST));
			curMenu.AppendMenu(ITSMenu::MIT_Menu, 
				(INT)GetInternalMenu(MNU_SectionSortAlbumMenu).GetInternalHandler(), 
				(LPTSTR)pAPI->GetString(IDS_ALBUM));
			curMenu.AppendMenu(ITSMenu::MIT_Menu, 
				(INT)GetInternalMenu(MNU_SectionSortGenreMenu).GetInternalHandler(), 
				(LPTSTR)pAPI->GetString(IDS_GENRE));
			curMenu.AppendMenu(ITSMenu::MIT_Menu, 
				(INT)GetInternalMenu(MNU_SectionSortCollectionMenu).GetInternalHandler(), 
				(LPTSTR)pAPI->GetString(IDS_COLLECTION));
			curMenu.AppendMenu(ITSMenu::MIT_Menu, 
				(INT)GetInternalMenu(MNU_SectionSortYearMenu).GetInternalHandler(), 
				(LPTSTR)pAPI->GetString(IDS_YEAR));
			curMenu.AppendMenu(ITSMenu::MIT_Menu, 
				(INT)GetInternalMenu(MNU_SectionSortMonthAddedMenu).GetInternalHandler(), 
				(LPTSTR)pAPI->GetString(IDS_MONTH));
		}
		break;
	case MNU_SectionSortArtistMenu:
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsArtName, 
			(LPTSTR)pAPI->GetStringForRecordSortOption(RSO_Title));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsArtRating,  
			(LPTSTR)pAPI->GetStringForRecordSortOption(RSO_Rating));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsArtCount, 
			(LPTSTR)pAPI->GetStringForRecordSortOption(RSO_TrackCount));
		curMenu.AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsArtDesc, 
			(LPTSTR)pAPI->GetString(IDS_DESCENDING));
		break;
	case MNU_SectionSortAlbumMenu:
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsAlbName, 
			(LPTSTR)pAPI->GetStringForRecordSortOption(RSO_Title));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsAlbYear,  
			(LPTSTR)pAPI->GetStringForRecordSortOption(RSO_Year));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsAlbRating,  
			(LPTSTR)pAPI->GetStringForRecordSortOption(RSO_Rating));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsAlbCount, 
			(LPTSTR)pAPI->GetStringForRecordSortOption(RSO_TrackCount));
		curMenu.AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsAlbDesc, 
			(LPTSTR)pAPI->GetString(IDS_DESCENDING));
		break;
	case MNU_SectionSortGenreMenu:
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsGenName, 
			(LPTSTR)pAPI->GetStringForRecordSortOption(RSO_Title));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsGenCount, 
			(LPTSTR)pAPI->GetStringForRecordSortOption(RSO_TrackCount));
		curMenu.AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsGenDesc, 
			(LPTSTR)pAPI->GetString(IDS_DESCENDING));
		break;
	case MNU_SectionSortCollectionMenu:
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsColName, 
			(LPTSTR)pAPI->GetStringForRecordSortOption(RSO_Title));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsColCount, 
			(LPTSTR)pAPI->GetStringForRecordSortOption(RSO_TrackCount));
		curMenu.AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsColDesc, 
			(LPTSTR)pAPI->GetString(IDS_DESCENDING));
		break;
	case MNU_SectionSortYearMenu:
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsYeaName, 
			(LPTSTR)pAPI->GetStringForRecordSortOption(RSO_Title));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsYeaCount, 
			(LPTSTR)pAPI->GetStringForRecordSortOption(RSO_TrackCount));
		curMenu.AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsYeaDesc, 
			(LPTSTR)pAPI->GetString(IDS_DESCENDING));
		break;
	case MNU_SectionSortMonthAddedMenu:
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsMonName, 
			(LPTSTR)pAPI->GetStringForRecordSortOption(RSO_Title));
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsMonCount, 
			(LPTSTR)pAPI->GetStringForRecordSortOption(RSO_TrackCount));
		curMenu.AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionSortsMonDesc, 
			(LPTSTR)pAPI->GetString(IDS_DESCENDING));
		break;
	case MNU_SectionFilterMenu:
		for (int i = SFILT_First + 1; i < SFILT_Last; i++)
			curMenu.AppendMenu(ITSMenu::MIT_String, i + MENU_SectionFiltersHolder, (LPTSTR)pAPI->GetStringForSectionFilter((SectionFiltersEnum)i));
		break;
	case MNU_SectionStylesMenu:
		for (int i = SSTYL_First + 1 ; i < SSTYL_Last; i++)
		{
			TCHAR bf[100];
			_sntprintf(bf, 100, _T("%s: %d"), pAPI->GetString(IDS_STYLE), i);
			curMenu.AppendMenu(ITSMenu::MIT_String, i + MENU_SectionStylesHolder, bf);
		}
		break;
	case MNU_SectionGroupingMenu:
		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SectionGroupingHolder, (LPTSTR)pAPI->GetString(IDS_ENABLED));
		curMenu.AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);
		for (int i = 1; i<=10; i++)
		{
			TCHAR bf[20];
			curMenu.AppendMenu(ITSMenu::MIT_String, i + MENU_SectionGroupingHolder, _itot(i, bf, 10));
		}
		break;
	default:
		ASSERT(0);
	}
	return curMenu;
}


