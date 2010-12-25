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
#include "HistTracksListCtrl.h"
#include "TeenSpirit.h"
#include "HistoryDlg.h"
#include "ActionManager.h"
#include "PrgAPI.h"
#include "SQLManager.h"
#include "DataRecordsXmlSerializer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHistTracksListCtrl::CHistTracksListCtrl()
{
	m_so.option = RSO_Hits;
	m_so.ascending = FALSE;
}

void CHistTracksListCtrl::InsertColumns()
{
	LVCOLUMN col;
	memset(&col, 0, sizeof(LVCOLUMN));
	col.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
	PrgAPI* pLang = PRGAPI();
	//CString name;
	col.fmt = LVCFMT_RIGHT;
	col.cx = 50;
	col.pszText = (LPTSTR) pLang->GetString(IDS_RANK);
	InsertColumn(COL_Rank, &col);
	col.cx = 40;
	col.pszText = (LPTSTR) pLang->GetString(IDS_HITS);
	InsertColumn(COL_Hits, &col);
	col.fmt = LVCFMT_LEFT;
	col.cx = 200;
	col.pszText = (LPTSTR) pLang->GetString(IDS_TITLE);
	InsertColumn(COL_TrackName, &col);
	col.cx = 100;
	col.pszText = (LPTSTR) pLang->GetString(IDS_ARTIST);
	InsertColumn(COL_ArtistName, &col);
	col.cx = 150;
	col.pszText = (LPTSTR) pLang->GetString(IDS_FIRSTAPPEARANCE);
	InsertColumn(COL_FirstDate, &col);
	col.cx = 150;
	col.pszText = (LPTSTR) pLang->GetString(IDS_LASTAPPEARANCE);
	InsertColumn(COL_LastDate, &col);
}


void CHistTracksListCtrl::Refresh()
{
	TRACEST(_T("CHistTracksListCtrl::Refresh"));
	SetRedraw(FALSE);
	DeleteAllItems();
	SQLManager* pSM = PRGAPI()->GetSQLManager();
	m_collection.clear();
	VERIFY(pSM->GetFullHistTrackRecordCollection(m_collection,
		m_pFilter->MinTimeStatus == HistoryDlgFilter::FS_Enabled ? &m_pFilter->MinTime : NULL, 
		m_pFilter->MaxTimeStatus == HistoryDlgFilter::FS_Enabled ? &m_pFilter->MaxTime : NULL, 
		m_pFilter->ArtistStatus == HistoryDlgFilter::FS_Enabled ? m_pFilter->Artist : NULL, 
		m_pFilter->TrackStatus == HistoryDlgFilter::FS_Enabled ? m_pFilter->Track : NULL, 
		HLA_Played, 
		0));
	SortOptionCollection soc;
	soc.ApplySortOption(m_so);
	RecordCollectionSorter::SortFullHistTrackRecordCollection(m_collection, soc);

	SetItemCount(m_collection.size());
	SetRedraw(TRUE);
}

UINT CHistTracksListCtrl::GetRankingForHits(UINT hits)
{
	if (m_ranking.empty())
	{
		SQLManager* pSM = PRGAPI()->GetSQLManager();
		pSM->GetHistTrackRankCollection(m_ranking);
	}
	INT rank = 1;
	for (size_t i = 0; i < m_ranking.size(); i++)
	{
		if (m_ranking[i].Hits > hits)
			rank += m_ranking[i].HitsCount;
	}
	return rank;
}

void CHistTracksListCtrl::GetDispInfo(LV_ITEM* pItem)
{
	FullHistTrackRecord& rec = *m_collection[pItem->iItem];
	switch (pItem->iSubItem )
	{
	case COL_Rank:
		{
			UINT rank = GetRankingForHits(rec.hits);
			if(pItem->mask & LVIF_IMAGE)
			{
				if (rank <= 10)
					pItem->iImage = ICO_Top1016;
				else if (rank <=100)
					pItem->iImage = ICO_Top10016;
				else
					pItem->iImage = ICO_TopRest16;
			}
			_itot(rank, pItem->pszText, 10);
		}
		break;
	case COL_Hits:
		_itot(rec.hits, pItem->pszText, 10);
		break;
	case COL_TrackName:
		_tcsncpy(pItem->pszText, rec.track.name.c_str(),  pItem->cchTextMax);
		break;
	case COL_ArtistName:
		_tcsncpy(pItem->pszText, rec.artist.name.c_str(),  pItem->cchTextMax);
		break;
	case COL_FirstDate:
		PrgAPI::FormatDate(pItem->pszText, pItem->cchTextMax, rec.firstTime);
		break;
	case COL_LastDate:
		PrgAPI::FormatDate(pItem->pszText, pItem->cchTextMax, rec.lastTime);
		break;
	default:
		ASSERT(FALSE);
	}
}

void CHistTracksListCtrl::CreateMainMenu(BCMenu& menu)
{
	PrgAPI* pAPI = PRGAPI();
	menu.AppendMenu(MF_STRING, ACMD_Play, 
		(LPTSTR) pAPI->GetString(IDS_PLAY), pAPI->GetIcon(ICO_Play16));
	menu.SetDefaultItem(ACMD_Play);
	menu.AppendMenu(MF_STRING, ACMD_Enqueue, 
		(LPTSTR) pAPI->GetString(IDS_ENQUEUE), pAPI->GetIcon(ICO_Add16));
	menu.AppendMenu(MF_SEPARATOR, NULL, NULL);
	menu.AppendMenu(MF_STRING, ACMD_OpenFolder, 
		(LPTSTR) pAPI->GetString(IDS_OPENCONTAININGFOLDER), pAPI->GetIcon(ICO_Find16));
	menu.AppendMenu(MF_SEPARATOR, NULL, NULL);
	menu.AppendMenu(MF_STRING, ID_EXPORTSELECTION, (LPTSTR) pAPI->GetString(IDS_EXPORTSELECTEDLINES));
	menu.AppendMenu(MF_STRING, ID_EXPORTALL, (LPTSTR) pAPI->GetString(IDS_EXPORTALL));
	menu.AppendMenu(MF_SEPARATOR, NULL, NULL);
	menu.AppendMenu(MF_STRING, ACMD_History, (LPTSTR) pAPI->GetString(IDS_SHOWHISTORY));
	//menu.LoadToolbar(IDR_MAINLISTNORMAL);

}

void CHistTracksListCtrl::InitMainMenu(BCMenu& menu)
{
	int a = GetSelectedCount();
	menu.EnableMenuItem(ACMD_OpenFolder, a == 1 ? MF_ENABLED : MF_GRAYED);
	menu.EnableMenuItem(ACMD_History,  a == 1 ? MF_ENABLED : MF_GRAYED);

}

BOOL CHistTracksListCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CHistTracksListCtrl::OnCommand \r\n"), wParam);
	switch (wParam)
	{
	case ACMD_Play:
		ExecuteTracks(FALSE);
		break;
	case ACMD_Enqueue:
		ExecuteTracks(TRUE);
		break;
	case ACMD_OpenFolder:
		{
			POSITION pos = GetFirstSelectedItemPosition();
			if (pos)
			{
				FullTrackRecordCollection col;
				if (GetFullTrackRecordCollectionByItemID(col, GetNextSelectedItem(pos), 1) && col.size() == 1)
					PRGAPI()->GetActionManager()->OpenFolderToItem(col[0]->track.location.c_str(), col[0]->collection.type);
			}
		}
		break;
	case ACMD_History:
		{
			ASSERT(m_pHistDlg != NULL);
			POSITION pos = GetFirstSelectedItemPosition();
			if (pos != 0)
			{
				TCHAR title[100];
				TCHAR artist[100];
				int Item = GetNextSelectedItem(pos);
				GetItemText(Item, 2, title, 100);
				GetItemText(Item, 3, artist, 100);
				m_pHistDlg->SetTrackFilterText(title, artist, TRUE);
			}
		}
		break;
	case ID_EXPORTSELECTION:
	case ID_EXPORTALL:
		{
			CFileDialog fd(FALSE, _T(".xml"), NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, 
				_T("Xml Files (*.xml)|*.xml|All Files (*.*)|*.*||"), NULL);
			if (fd.DoModal() == IDOK)
			{
				if (wParam == ID_EXPORTSELECTION)
				{
					FullHistTrackRecordCollection col;
					POSITION pos = GetFirstSelectedItemPosition();
					while (pos != NULL)
						col.push_back(m_collection[GetNextSelectedItem(pos)]);
					DataRecordsXmlSerializer::Export(fd.GetFileName(), col);
				}
				else
					DataRecordsXmlSerializer::Export(fd.GetFileName(), m_collection);
			}
		}
		break;
	default:
		ASSERT(FALSE);
	}
	return TRUE;
}


void CHistTracksListCtrl::ExecuteTracks(BOOL enqueue)
{
	FullTrackRecordCollection col;
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos)
		GetFullTrackRecordCollectionByItemID(col, GetNextSelectedItem(pos), 1);
	if (!col.empty())
	{
		ActionManager* am = PRGAPI()->GetActionManager();
		MediaPlayer* mp = PRGAPI()->GetMediaPlayer();
		if (enqueue)
			am->Enqueue(mp, col);
		else
			am->Play(mp, col);
	}

}
BEGIN_MESSAGE_MAP(CHistTracksListCtrl, CHistListCtrl)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CHistTracksListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	ExecuteTracks(FALSE);
	CHistListCtrl::OnLButtonDblClk(nFlags, point);
}

const static RecordSortOptionsEnum HistTracksColumn2SortOption[] = 
{
	RSO_Hits,
	RSO_Hits,
	RSO_Title,
	RSO_Artist,
	RSO_FirstDate,
	RSO_LastDate,
	RSO_None
};

void CHistTracksListCtrl::ToggleSorting(INT ActiveColum)
{
	RecordSortOptionsEnum rso = HistTracksColumn2SortOption[ActiveColum];
	if (m_so.option == rso)
	{
		m_so.ascending = !m_so.ascending;
	}
	else
	{
		m_so.ascending = TRUE;
		m_so.option = rso;
	}
	SortOptionCollection soc;
	soc.ApplySortOption(m_so);
	RecordCollectionSorter::SortFullHistTrackRecordCollection(m_collection, soc);
	Invalidate();
}

INT CHistTracksListCtrl::GetSortingColumn()
{
	INT col = 0;
	while (HistTracksColumn2SortOption[col] != RSO_None)
	{
		if (HistTracksColumn2SortOption[col] == m_so.option)
			return col;
		col++;
	}
	return -1;
}

BOOL CHistTracksListCtrl::GetFullTrackRecordCollectionByItemID(FullTrackRecordCollection& col, INT nItem, INT limitRecords)
{
	TracksFilter tf;
	tf.Artist.match = TXTM_Exact;
	tf.Artist.val = m_collection[nItem]->artist.name;
	tf.Title.match = TXTM_Exact;
	tf.Title.val = m_collection[nItem]->track.name;
	SQLManager* pSM = PRGAPI()->GetSQLManager();
	if (pSM->GetFullTrackRecordCollectionByTracksFilter(col, tf, limitRecords))
	{
		if (!col.empty())
			return TRUE;
	}
	PRGAPI()->NotifyUser(IDS_TRACKNOTFOUND, NULL, SEV_Warning);
	return FALSE;
}
