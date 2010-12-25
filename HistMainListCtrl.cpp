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
#include "HistMainListCtrl.h"
#include "TeenSpirit.h"
#include "PrgAPI.h"
#include "ActionManager.h"
#include "SQLManager.h"
#include "DataRecordsXmlSerializer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CHistMainListCtrl::CHistMainListCtrl()
{
	m_so.option = RSO_DateAdded;
	m_so.ascending = FALSE;
}


void CHistMainListCtrl::InsertColumns()
{
	LVCOLUMN col;
	memset(&col, 0, sizeof(LVCOLUMN));
	col.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
	PrgAPI* pLang = PRGAPI();
	col.fmt = LVCFMT_LEFT;
	col.cx = 150;
	col.pszText = (LPTSTR) pLang->GetString(IDS_DATEADDED);
	InsertColumn(COL_DateAdded, &col);
	col.cx = 200;
	col.pszText = (LPTSTR) pLang->GetString(IDS_TITLE);
	InsertColumn(COL_TrackName, &col);
	col.cx = 100;
	col.pszText = (LPTSTR) pLang->GetString(IDS_ARTIST);
	InsertColumn(COL_ArtistName, &col);
}


void CHistMainListCtrl::Refresh()
{
	TRACEST(_T("CHistMainListCtrl::Refresh"));
	SetRedraw(FALSE);
	DeleteAllItems();
	SQLManager* pSM = PRGAPI()->GetSQLManager();
	m_collection.clear();
	VERIFY(pSM->GetFullLogRecordCollection(m_collection,
				m_pFilter->MinTimeStatus == HistoryDlgFilter::FS_Enabled ? &m_pFilter->MinTime : NULL, 
				m_pFilter->MaxTimeStatus == HistoryDlgFilter::FS_Enabled ? &m_pFilter->MaxTime : NULL, 
				m_pFilter->ArtistStatus == HistoryDlgFilter::FS_Enabled ? m_pFilter->Artist : NULL, 
				m_pFilter->TrackStatus == HistoryDlgFilter::FS_Enabled ? m_pFilter->Track : NULL, 
				HLA_Unknown, 
				0));
	SortOptionCollection soc;
	soc.ApplySortOption(m_so);
	RecordCollectionSorter::SortFullHistLogRecordCollection(m_collection, soc);

	SetItemCount(m_collection.size());
	SetRedraw(TRUE);
}

void CHistMainListCtrl::GetDispInfo(LV_ITEM* pItem)
{
	ASSERT(pItem->iItem >= 0 && pItem->iItem < (INT)m_collection.size());
	if (pItem->iItem >= 0 && pItem->iItem < (INT)m_collection.size())
	{
		FullHistLogRecord& rec = *m_collection[pItem->iItem];
		switch (pItem->iSubItem )
		{
		case COL_DateAdded:
			if(pItem->mask & LVIF_IMAGE)
			{
				switch (rec.log.actID)
				{
				case HLA_Played:
					pItem->iImage = ICO_Played16;
					break;
				case HLA_Clicked:
					pItem->iImage = ICO_MouseClick16;
					break;
				default:
					ASSERT(0);
				}
			}
			PrgAPI::FormatDate(pItem->pszText, pItem->cchTextMax, rec.log.dateAdded);
			break;
		case COL_TrackName:
			_tcsncpy(pItem->pszText, rec.track.name.c_str(), pItem->cchTextMax);
			break;
		case COL_ArtistName:
			_tcsncpy(pItem->pszText, rec.artist.name.c_str(), pItem->cchTextMax);
			break;
		default:
			ASSERT(FALSE);
		}
	}
}
void CHistMainListCtrl::CreateMainMenu(BCMenu& menu)
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
	menu.AppendMenu(MF_STRING, ACMD_Delete, 
		(LPTSTR) pAPI->GetString(IDS_DELETEFROMHISTORY), pAPI->GetIcon(ICO_Delete16));

}

void CHistMainListCtrl::InitMainMenu(BCMenu& menu)
{
	int a = GetSelectedCount();
	menu.EnableMenuItem(ACMD_OpenFolder, a == 1 ? MF_ENABLED : MF_GRAYED);
}

const static RecordSortOptionsEnum HistMainColumn2SortOption[] = 
{
	RSO_DateAdded,
	RSO_Title,
	RSO_Artist,
	RSO_None
};

void CHistMainListCtrl::ToggleSorting(INT ActiveColum)
{
	RecordSortOptionsEnum rso = HistMainColumn2SortOption[ActiveColum];
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
	RecordCollectionSorter::SortFullHistLogRecordCollection(m_collection, soc);
	Invalidate();
}

INT CHistMainListCtrl::GetSortingColumn()
{
	INT col = 0;
	while (HistMainColumn2SortOption[col] != RSO_None)
	{
		if (HistMainColumn2SortOption[col] == m_so.option)
			return col;
		col++;
	}
	return -1;
}




BOOL CHistMainListCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CHistMainListCtrl::OnCommand \r\n"), wParam);
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
	case ACMD_Delete:
		{
			POSITION pos = GetFirstSelectedItemPosition();
			std::vector<INT> nItems;
			while (pos)
				nItems.push_back(GetNextSelectedItem(pos));
			SQLManager* pSM = PRGAPI()->GetSQLManager();
			for (INT nItem = nItems.size() - 1; nItem >= 0; nItem--)
			{
				pSM->DeleteHistLogRecord(m_collection[nItems[nItem]]->log.ID);
				m_collection.erase(m_collection.begin() + nItems[nItem]);
			}
			SetItemCount(m_collection.size());
			Invalidate();
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
					FullHistLogRecordCollection col;
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

void CHistMainListCtrl::ExecuteTracks(BOOL enqueue)
{
	FullTrackRecordCollection col;
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos != NULL)
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

BOOL CHistMainListCtrl::GetFullTrackRecordCollectionByItemID(FullTrackRecordCollection& col, INT nItem, INT limitRecords)
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

BEGIN_MESSAGE_MAP(CHistMainListCtrl, CHistListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMACTIVATE, &CHistMainListCtrl::OnLvnItemActivate)
END_MESSAGE_MAP()

void CHistMainListCtrl::OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	ExecuteTracks(FALSE);
	*pResult = 0;
}
