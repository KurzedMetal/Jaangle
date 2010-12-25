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
#include "TrackListPane.h"
#include "resource.h"
#include "PrgAPI.h"
#include "SQLManager.h"
#include "ActionManager.h"
#include "StateManager.h"
//#include "StringSerializerHelper.h"

TrackListPane::TrackListPane():
m_bSyncWithPlayer(FALSE),
m_bSyncWithSectionChanger(TRUE),
m_poptionsMenu(NULL)
{
}


TrackListPane::~TrackListPane()
{
	delete m_poptionsMenu;
}


BOOL TrackListPane::Init(HWND hwndParent)
{
	if (m_enhTrackList.GetSafeHwnd())
		return TRUE;
	m_hwndParent = hwndParent;
	CWnd* pParent = CWnd::FromHandle(m_hwndParent);
	//=== Create the EnhTrackList Control & the TrackList
	m_enhTrackList.Create(pParent, 7391);
	//OnStateChanged(SM_CurrentSectionChanged);
	return TRUE;
}

void TrackListPane::UnInit()
{
	if (m_enhTrackList.GetSafeHwnd() == 0)
		return;

	Hide();
	PrgAPI* pAPI = PRGAPI();
	//pAPI->GetSkinManager()->UnRegisterSkinnable(*this);
	//pAPI->GetTranslationManager()->UnRegisterTranslatable(*this);
	//pAPI->GetStateManager()->UnRegisterStateChangeListener(*this);

}

BOOL TrackListPane::Show()
{
	if (m_enhTrackList.GetSafeHwnd())
	{
		PRGAPI()->GetStateManager()->RegisterStateChangeListener(*this);
		m_enhTrackList.ShowWindow(SW_SHOW);
		OnStateChanged(SM_CurrentSectionChanged);
		return TRUE;
	}
	return FALSE;
}
void TrackListPane::Hide()
{
	if (m_enhTrackList.GetSafeHwnd())
	{
		PRGAPI()->GetStateManager()->UnRegisterStateChangeListener(*this);
		m_enhTrackList.ShowWindow(SW_HIDE);
	}
}

void TrackListPane::MovePane(INT x, INT y, INT cx, INT cy)
{
	m_clientRC = CRect(x, y, x + cx, y + cy);
	m_enhTrackList.MoveWindow(x, y, cx, cy);
}

LPCTSTR TrackListPane::GetTitle(UINT captionIndex)
{
	PrgAPI* pAPI = PRGAPI();
	switch (captionIndex % 2)
	{
	case 0:
		return pAPI->GetString(IDS_TRACKLIST);
	case 1:
		_sntprintf(m_stringBuffer, 500, _T("%s: [F2]"), pAPI->GetString(IDS_FOCUS));
		return m_stringBuffer;
	}
	return NULL;
}


BOOL TrackListPane::GetButtonInfo(PaneButtonInfo& bInfo, UINT idx)
{
	bInfo = PaneButtonInfo();
	bInfo.iconSize = 16;
	PrgAPI* pAPI = PRGAPI();
	switch (idx)
	{
	case CMD_Options:
		bInfo.hIcon = pAPI->GetIcon(ICO_Options16);
		break;
	case CMD_Play:
		bInfo.hIcon = pAPI->GetIcon(ICO_Play16);
		bInfo.text = pAPI->GetString(IDS_PLAY);
		break;
	case CMD_Enqueue:
		bInfo.hIcon = pAPI->GetIcon(ICO_Add16);
		bInfo.text = pAPI->GetString(IDS_ENQUEUE);
		break;
	default:
		return FALSE;
	}
	return TRUE;

}

BOOL TrackListPane::OnButton(UINT idx)
{
	switch (idx)
	{
	case CMD_Play:
		m_enhTrackList.PlayTracks();
		break;
	case CMD_Enqueue:
		m_enhTrackList.EnqueueTracks();
		break;
	case CMD_ShowInfoIcons:
		m_enhTrackList.GetMainTrackList().ShowInfoIcons(!m_enhTrackList.GetMainTrackList().IsShowInfoIconsEnabled());
		m_enhTrackList.GetMainTrackList().Invalidate(FALSE);
		break;
	case CMD_SyncWithSectionChanger:
		m_bSyncWithSectionChanger = !m_bSyncWithSectionChanger;
		break;
	case CMD_SyncWithPlayer:
		m_bSyncWithPlayer = !m_bSyncWithPlayer;
		break;
	default:
		ASSERT(0);
		return FALSE;
	}
	return TRUE;
}

//=== From StateChangeListener
BOOL TrackListPane::OnStateChanged(UINT stateMessage)
{
	if (m_bSyncWithSectionChanger == FALSE)
	{
		switch (stateMessage)
		{
		case SM_DatabaseUpdated:
		case SM_CollectionManagerEvent:
		case SM_CurrentSectionChanged:
		case SM_LocateTrackRequest:
			return FALSE;
		}
	}
	if (m_bSyncWithPlayer == FALSE && stateMessage == SM_MediaChanged)
	{
		return FALSE;
	}
	return m_enhTrackList.OnStateChanged(stateMessage);
}



ITSMenu* TrackListPane::GetMenu(UINT idx)
{
	if (idx == CMD_Options)
	{
		PrgAPI* pAPI = PRGAPI();
		if (m_poptionsMenu == NULL)
			m_poptionsMenu = pAPI->CreatePopupMenu();
		if (m_poptionsMenu->GetInternalHandler() == NULL)
		{
			m_poptionsMenu->Create();
			m_poptionsMenu->AppendMenu(ITSMenu::MIT_String, CMD_SyncWithPlayer, (LPTSTR)pAPI->GetString(IDS_SYNCWITHPLAYER));
			m_poptionsMenu->AppendMenu(ITSMenu::MIT_String, CMD_SyncWithSectionChanger, (LPTSTR)pAPI->GetString(IDS_SYNCWITHSECTIONS));
			m_poptionsMenu->AppendMenu(ITSMenu::MIT_Separator, NULL, NULL);
			m_poptionsMenu->AppendMenu(ITSMenu::MIT_String, CMD_ShowInfoIcons, (LPTSTR)pAPI->GetString(IDS_SHOWINFOICONS));
		};
		m_poptionsMenu->CheckMenuItem(CMD_SyncWithPlayer, m_bSyncWithPlayer);
		m_poptionsMenu->CheckMenuItem(CMD_SyncWithSectionChanger, m_bSyncWithSectionChanger);
		m_poptionsMenu->CheckMenuItem(CMD_ShowInfoIcons, m_enhTrackList.GetMainTrackList().IsShowInfoIconsEnabled());
		return m_poptionsMenu;
	}
	return NULL;
}

const LPCTSTR cTrackListPane_Header = _T("Header");
const LPCTSTR cTrackListPane_ShowInfoIcons = _T("ShowInfoIcons");
const LPCTSTR cTrackListPane_SyncWithPlayer = _T("SyncWithPlayer");
const LPCTSTR cTrackListPane_SyncWithSectionChanger = _T("SyncWithSectionChanger");
const LPCTSTR cTrackListPane_ColumnInfo = _T("ColumnInfo");
const LPCTSTR cTrackListPane_SortingCount = _T("SortingCount");
const LPCTSTR cTrackListPane_Sorting = _T("Sorting");


BOOL TrackListPane::LoadState(IPaneState& paneState)
{
	LPCTSTR sValue = paneState.GetLPCTSTRSetting(cTrackListPane_Header);
	if (_tcsicmp(sValue, TrackListPaneInfo.Name) != 0)
		return FALSE;

	CMainListCtrl& list = m_enhTrackList.GetMainTrackList();
	INT value = paneState.GetIntSetting(cTrackListPane_ShowInfoIcons);
	if (value != -1)
		list.ShowInfoIcons(value != 0);

	value = paneState.GetIntSetting(cTrackListPane_SyncWithPlayer);
	if (value != -1)
		m_bSyncWithPlayer = value != 0;

	value = paneState.GetIntSetting(cTrackListPane_SyncWithSectionChanger);
	if (value != -1)
		m_bSyncWithSectionChanger = value != 0;



	//=== Read Column Visibility / Width / Position Info
	TCHAR bf[500];
	CMainListCtrl::SColumnInfo* colInfo = list.GetColumnInfo();
	for (INT i = CMainListCtrl::CL_First; i < CMainListCtrl::CL_Last; i++)
	{
		_sntprintf(bf, 500, _T("%s_%02d_vis"), cTrackListPane_ColumnInfo, i);
		colInfo[i].bVisible = paneState.GetIntSetting(bf) > 0;
		_sntprintf(bf, 500, _T("%s_%02d_order"), cTrackListPane_ColumnInfo, i);
		value = paneState.GetIntSetting(bf);
		if (value >= 0 && value < 100)
			colInfo[i].iOrder = value;
		_sntprintf(bf, 500, _T("%s_%02d_cx"), cTrackListPane_ColumnInfo, i);
		value = paneState.GetIntSetting(bf);
		if (value >= 0 && value < 10000)
			colInfo[i].cx = value;
		if (colInfo[i].cx < 1)
			colInfo[i].cx = 1;
	}
	list.UpdateColumns();


	//=== Read the sorting information
	SortOptionCollection& soc = list.GetSortingInfo();
	INT sortCount = paneState.GetIntSetting(cTrackListPane_SortingCount);
	for (INT i = 0; i < sortCount; i++)
	{
		SortOption so(RSO_None, TRUE);
		INT sortOption = 0;
		_sntprintf(bf, 500, _T("%s_%02d_option"), cTrackListPane_Sorting, i);
		value = paneState.GetIntSetting(bf);
		if (value >= RSO_None && value < RSO_Last)
			so.option = RecordSortOptionsEnum(value);

		_sntprintf(bf, 500, _T("%s_%02d_asc"), cTrackListPane_Sorting, i);
		so.ascending = paneState.GetIntSetting(bf) != 0;

		soc.ApplySortOption(so);
	}
	if (sortCount > 0)
		list.UpdateSorting();
	return TRUE;
}
BOOL TrackListPane::SaveState(IPaneState& paneState)				
{
	paneState.SetLPCTSTRSetting(cTrackListPane_Header, TrackListPaneInfo.Name);
	CMainListCtrl& list = m_enhTrackList.GetMainTrackList();
	paneState.SetIntSetting(cTrackListPane_ShowInfoIcons, list.IsShowInfoIconsEnabled());
	paneState.SetIntSetting(cTrackListPane_SyncWithPlayer, m_bSyncWithPlayer);
	paneState.SetIntSetting(cTrackListPane_SyncWithSectionChanger, m_bSyncWithSectionChanger);

	//=== Write Column Visibility / Width / Position Info
	TCHAR bf[500];
	CMainListCtrl::SColumnInfo* colInfo = list.GetColumnInfo();
	for (int i = 0; i < CMainListCtrl::CL_Last; i++)
	{
		_sntprintf(bf, 500, _T("%s_%02d_vis"), cTrackListPane_ColumnInfo, i);
		paneState.SetIntSetting(bf, colInfo[i].bVisible);
		_sntprintf(bf, 500, _T("%s_%02d_order"), cTrackListPane_ColumnInfo, i);
		paneState.SetIntSetting(bf, colInfo[i].iOrder);
		_sntprintf(bf, 500, _T("%s_%02d_cx"), cTrackListPane_ColumnInfo, i);
		paneState.SetIntSetting(bf, colInfo[i].cx);
	}

	//=== Write the sorting information
	SortOptionCollection& soc = list.GetSortingInfo();
	paneState.SetIntSetting(cTrackListPane_SortingCount, soc.GetSortOptionsCount());
	for (UINT i =0; i< soc.GetSortOptionsCount(); i++)
	{
		SortOption so = soc.GetSortOption(i);
		_sntprintf(bf, 500, _T("%s_%02d_option"), cTrackListPane_Sorting, i);
		paneState.SetIntSetting(bf, so.option);
		_sntprintf(bf, 500, _T("%s_%02d_asc"), cTrackListPane_Sorting, i);
		paneState.SetIntSetting(bf, so.ascending);
	}
	return TRUE;
}





//void TrackListPane::SaveSettings()
//{
//	FullTrackRecordSP rec;
//	m_trackList.GetItem(rec, 0);
//	UINT lastID = 0;
//	if (rec.get())
//		lastID = rec->track.ID;
//	PRGAPI()->SetOption(OPT_TREE_LastSectionID, lastID);
//}

void TrackListPane::ApplyTranslation(ITranslation& translation)
{
	//=== The menus are stored in MenuManager so they automatically reset
	if (m_poptionsMenu != NULL)
		m_poptionsMenu->Destroy();
}


