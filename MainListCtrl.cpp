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
#include "MainListCtrl.h"
#include "TeenSpirit.h"
//#include "AppSettings/AppSettings.h"
#include "MediaPlayer.h"
#include "ActionManager.h"//For Sync Track / ExportManager
//-------------------to be removed
#include "PrgApi.h"
#include "ServicesRegistrationSrv.h"
#include "InfoDownloadManager.h"
#include "InformationProviders/InfoProviderFactory.h"
#include "MenuManager.h"
#include "SQLManager.h"
#include "DataRecordsXmlSerializer.h"
#include "ReEntryLock.h"
#include "FileNameTagger.h"
#include "RenamerDlg.h"
#include "TagsFromFilenameDlg.h"
#include "StateManager.h"
#include "TSOnlineServices.h"


#define ID_PROVIDERS 1000
//#define ID_RATING 1100
#define ID_CASESTYLE 1200

//const INT sRenamePatternCount = sizeof(sRenamePattern)/sizeof(LPCTSTR);


//#define ID_RENAMER 400
#define ID_MAINLISTNORMAL_EXPORT 500
#define ID_MAINLISTNORMAL_EXPORT_SELECTION 501

#define ACMD_CopyToCLIPBOARD 600
#define ACMD_CopyToFOLDER 601
#define ACMD_CopyToLASTFOLDER1 602

#define ID_COLUMNMSG 700

const RecordSortOptionsEnum ColumnItem2RecordSortOption[] = 
{
	RSO_Title,
	RSO_None,
	RSO_Album,
	RSO_Artist,
	RSO_Rating,
	RSO_TrackNo,
	RSO_Location,
	RSO_Collection,
	RSO_Size,
	RSO_Duration,
	RSO_Bitrate,
	RSO_Year,
	RSO_Genre,
	RSO_DateAdded
};

int defWidth[] = {200, 50, 100, 100, 80, 
50, 120, 60, 60, 
60, 60, 60, 100, 100};
int defRes[] = {IDS_TITLE, IDS_CONTROLS, IDS_ALBUM, IDS_ARTIST, IDS_RATING,
IDS_TRACKNO, IDS_LOCATION, IDS_COLLECTION, IDS_SIZE,
IDS_DURATION, IDS_BITRATE, IDS_YEAR, IDS_GENRE, IDS_DATEADDED};	


IMPLEMENT_DYNAMIC(CMainListCtrl, CListCtrl)
CMainListCtrl::CMainListCtrl():
//m_mode(TVM_Normal),
m_bDoNotUseDispOptimization(FALSE),
m_bReEntryLockCustomDraw(FALSE),
m_bUpdatingStateables(FALSE),
m_bShowInfoIcons(TRUE)
{
//	SetIdentifier(_T("LIST"));
	m_colors[COL_Bk] = RGB(255,255,255);
	m_colors[COL_Text] = RGB(0,0,0);
	m_colors[COL_AltBk] = RGB(250,250,252);
	m_colors[COL_AltText] = RGB(0,0,0);
	m_colors[COL_SelBk] = RGB(125,155,225);
	m_colors[COL_SelText] = RGB(255,255,255);
	//m_Filter.TrackID.match = NUMM_Exact;
	//m_Filter.TrackID.val = 0;
	ResetColumnInfo();
}

CMainListCtrl::~CMainListCtrl()
{
	DeleteMenus();
	if (m_hWnd) DestroyWindow();
}


BEGIN_MESSAGE_MAP(CMainListCtrl, /**/CListCtrl)
	ON_NOTIFY_REFLECT(LVN_ITEMACTIVATE, OnLvnItemActivate)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnLvnGetdispinfo)
	ON_WM_DESTROY()
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemclick)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, &CMainListCtrl::OnLvnBegindrag)
	ON_NOTIFY_REFLECT(NM_CLICK, &CMainListCtrl::OnNMClick)
	ON_NOTIFY_REFLECT(LVN_ODFINDITEM, &CMainListCtrl::OnLvnOdfinditem)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CMainListCtrl::OnLvnItemchanged)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(HDN_DIVIDERDBLCLICK, 0, &CMainListCtrl::OnHdnDividerdblclick)
END_MESSAGE_MAP()


void CMainListCtrl::ResetColumnInfo()
{
	for (INT i = CL_First; i < CL_Last; i++)
	{
		m_columns[i].bVisible = 1;//(WORD) val;
		m_columns[i].cx = defWidth[i];//val;
		m_columns[i].iOrder = i;//val;
	}

}


// CMainListCtrl message handlers

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//Initialization
//---------------------------------------------------------------------
//---------------------------------------------------------------------
void CMainListCtrl::ApplyTranslation(ITranslation& translation)
{
	PrgAPI* pAPI = PRGAPI();
	_tcsncpy(cUnknown, pAPI->GetString(IDS_UNKNOWN), 100);

	//UpdateColumns();

	LVCOLUMN col;
	memset(&col, 0, sizeof(LVCOLUMN));
	col.mask = LVCF_TEXT;
	HDITEM hd;
	memset(&hd, 0, sizeof(HDITEM));
	hd.mask = HDI_LPARAM;


	CHeaderCtrl* pHeader = GetHeaderCtrl();
	if (pHeader != NULL)
	{
		int numItems = pHeader->GetItemCount();
		for (int i = 0; i < numItems; i++)
		{
			pHeader->GetItem(i, &hd);
			col.pszText = (LPTSTR) pAPI->GetString(defRes[hd.lParam]);
			SetColumn(i, &col);
		}
	}


	DeleteMenus();
}

void CMainListCtrl::ApplySkin(ISkin& _skin)
{
	TSSkin& skin = (TSSkin&) _skin;
	SetFont(skin.GetFont(CSEC_TrackList, FSTY_Normal));
	SetColor(COL_Bk, skin.GetColor(CSEC_TrackList, CVAL_Bk));
	SetColor(COL_Text, skin.GetColor(CSEC_TrackList, CVAL_Text));
	SetColor(COL_AltBk, skin.GetColor(CSEC_TrackList, CVAL_AltBk));
	SetColor(COL_AltText, skin.GetColor(CSEC_TrackList, CVAL_AltText));
	SetColor(COL_SelBk, skin.GetColor(CSEC_TrackList, CVAL_SelBk));
	SetColor(COL_SelText, skin.GetColor(CSEC_TrackList, CVAL_SelText));
}


void CMainListCtrl::Init()
{
	TRACEST(_T("CMainListCtrl::Init"));
	VERIFY(m_LVMainList.Create(16, 20, ILC_COLOR24 | ILC_MASK, 1, 0));
	SetImageList(&m_LVMainList, LVSIL_SMALL);
	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	CHeaderCtrl* pHeader = GetHeaderCtrl();
	if (pHeader->GetItemCount() == 0)
		UpdateColumns();
	PrgAPI* pAPI = PRGAPI();
	ApplyTranslation(*pAPI->GetTranslationManager());
}

void CMainListCtrl::DeleteMenus()
{
	if (m_CopyMenu.m_hMenu)		m_CopyMenu.DestroyMenu();
	if (m_RenamerMenu.m_hMenu)	m_RenamerMenu.DestroyMenu();
	if (m_FileNameTaggerMenu.m_hMenu) m_FileNameTaggerMenu.DestroyMenu();
	if (m_ChangeCaseMenu.m_hMenu)	m_ChangeCaseMenu.DestroyMenu();
	if (m_AdvancedMenu.m_hMenu)	m_AdvancedMenu.DestroyMenu();
	if (m_MainMenu.m_hMenu)		m_MainMenu.DestroyMenu();
	if (m_ColumnsMenu.m_hMenu)	m_ColumnsMenu.DestroyMenu();
}

void CMainListCtrl::ShowColumn(ColumnsEnum colName, BOOL bShow)
{
	UpdateColumnInfo();
	if (m_columns[colName].bVisible == bShow)
		return;
	m_columns[colName].bVisible = bShow;
	for (INT i = CL_First; i < CL_Last; i++)
	{
		if (m_columns[i].iOrder >= m_columns[colName].iOrder && colName != i)
			m_columns[i].iOrder++;
	}
	UpdateColumns();
	return;
}

void CMainListCtrl::UpdateColumnInfo()
{
	HDITEM hd;
	memset(&hd, 0, sizeof(HDITEM));
	hd.mask = HDI_WIDTH | HDI_ORDER | HDI_LPARAM;
	CHeaderCtrl* pHeader = GetHeaderCtrl();
	INT hdCount = pHeader->GetItemCount();
	for (int i = 0; i < hdCount; i++)
	{
		pHeader->GetItem(i, &hd);
		m_columns[hd.lParam].cx = hd.cxy;
		m_columns[hd.lParam].iOrder = hd.iOrder;
	}
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

void CMainListCtrl::Update()
{
	SetRedraw(FALSE);
	DeleteAllItems();
	RecordCollectionSorter::SortFullTrackRecordCollection(m_tracks, m_SOC);
	SetItemCount(m_tracks.size());
	SetRedraw(TRUE);
}


void CMainListCtrl::InvalidateInfoIcons()
{
	FullTrackRecordCollection::iterator it = m_tracks.begin();
	for (;it != m_tracks.end(); it++)
		(*it)->track.flags = TRF_Uninitialized;
	Invalidate(FALSE);
}

void CMainListCtrl::GetSelectedTracks(FullTrackRecordCollection& col)
{
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos != NULL)
		col.push_back(m_tracks[GetNextSelectedItem(pos)]);
}

INT CMainListCtrl::GetItemCount()
{
	return m_tracks.size();
}

BOOL CMainListCtrl::GetItem(FullTrackRecordSP& rec, INT idx)
{
	if (idx < GetItemCount())
	{
		rec = m_tracks[idx];
		return TRUE;
	}
	TRACE(_T("@1 CMainListCtrl::GetItem FAILED"));
	return FALSE;
}

void CMainListCtrl::ResetMouseOver()
{
	if (m_mouseOverDrawInfo.bDrawn)
	{
		CRect rc;
		GetRealSubItemRect(m_mouseOverDrawInfo.nItem, m_mouseOverDrawInfo.nSubItem, LVIR_BOUNDS, rc);
		m_mouseOverDrawInfo.bDrawn = FALSE;
		m_mouseOverDrawInfo.nItem = -1;
		m_mouseOverDrawInfo.nSubItem = 0;
		m_mouseOverDrawInfo.xPos = 0;
		InvalidateRect(&rc, FALSE);
	}
}

void CMainListCtrl::OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult)
{
	TRACEST(_T("CMainListCtrl::OnLvnItemActivate"));
	PrgAPI* pAPI = PRGAPI();


	CPoint p;
	GetCursorPos(&p);
	ScreenToClient(&p);
	INT nItem = HitTest(p);
	if (nItem != -1)
	{
		FullTrackRecordCollection col;
		col.push_back(m_tracks[nItem]);
		//GetSelectedTracks(col);
		pAPI->GetActionManager()->DefaultPlayerAction(pAPI->GetMediaPlayer(), col);
	}
	*pResult = 0;
}

void CMainListCtrl::GetRealSubItemRect(INT nItem, INT nSubItem, INT area, CRect& rcSubItem)
{
	GetSubItemRect(nItem, nSubItem, area, rcSubItem);
	if (nSubItem == 0 && area == LVIR_BOUNDS)//There is a problem with the subitem 0 (Main Item)
	{
		CRect rcHeader;
		GetHeaderCtrl()->GetItemRect(0,&rcHeader);
		rcSubItem.right = rcHeader.right + rcSubItem.left;//Vista Fix
		rcSubItem.left = rcHeader.left + rcSubItem.left;
	}

}

void CMainListCtrl::GetColorsFromItem(INT nItem, COLORREF& bkColor, COLORREF& textColor)
{
	bkColor = m_colors[nItem % 2 ? COL_Bk : COL_AltBk];
	textColor = m_colors[nItem % 2 ? COL_Text : COL_AltText];
	if (GetItemState(nItem, LVIS_SELECTED) == LVIS_SELECTED)
	{
		bkColor = m_colors[COL_SelBk];
		textColor = m_colors[COL_SelText];
	}
}

void CMainListCtrl::DrawControlsSubItem(CDC& dc, CRect& rcSubItem, 
										COLORREF bkColor, COLORREF textColor, BOOL bMouseOver)
{
	PrgAPI* pAPI = PRGAPI();
	dc.FillSolidRect(rcSubItem, bkColor);

	CRect rcIcon(rcSubItem);
	rcIcon.left += 1;
	rcIcon.top += (rcSubItem.Height() - 16) / 2;
	rcIcon.right = rcIcon.left + 16;
	rcIcon.bottom = rcIcon.top + 16;
	if (bMouseOver && rcIcon.PtInRect(CPoint(rcSubItem.left + m_mouseOverDrawInfo.xPos, rcIcon.top)))
		dc.DrawEdge(&rcIcon, EDGE_SUNKEN, BF_RECT | BF_FLAT);
	DrawIconEx(dc.m_hDC, rcIcon.left, rcIcon.top, 
		pAPI->GetIcon(ICO_Play16), 16, 16, 0,0,DI_NORMAL);
	rcIcon.OffsetRect(16, 0);
	if (bMouseOver && rcIcon.PtInRect(CPoint(rcSubItem.left + m_mouseOverDrawInfo.xPos, rcIcon.top)))
		dc.DrawEdge(&rcIcon, EDGE_SUNKEN, BF_RECT | BF_FLAT);
	DrawIconEx(dc.m_hDC, rcIcon.left, rcIcon.top, 
		pAPI->GetIcon(ICO_Play16), 16, 16, 0,0,DI_NORMAL);
	DrawIconEx(dc.m_hDC, rcIcon.left + 2, rcIcon.top, 
		pAPI->GetIcon(ICO_Add16), 12, 12, 0,0,DI_NORMAL);
	rcIcon.OffsetRect(16, 0);
	if (bMouseOver && rcIcon.PtInRect(CPoint(rcSubItem.left + m_mouseOverDrawInfo.xPos, rcIcon.top)))
		dc.DrawEdge(&rcIcon, EDGE_SUNKEN, BF_RECT | BF_FLAT);
	DrawIconEx(dc.m_hDC, rcIcon.left, rcIcon.top, 
		pAPI->GetIcon(ICO_Add16), 16, 16, 0,0,DI_NORMAL);
}

void CMainListCtrl::DrawRatingsSubItem(CDC& dc, CRect& rcSubItem, 
									   COLORREF bkColor, COLORREF textColor, BOOL bMouseOver, 
									   INT rating)
{
	dc.FillSolidRect(rcSubItem, bkColor);
	CRect rcIcon(rcSubItem);
	rcIcon.left += 1;
	rcIcon.top += (rcSubItem.Height() - 16) / 2;
	rcIcon.right = rcIcon.left + 16;
	rcIcon.bottom = rcIcon.top + 16;
	FLOAT fStars = Rating2Stars(rating);
	int xPos = m_mouseOverDrawInfo.xPos;
	if (bMouseOver && xPos > 1 && xPos < (1 + 16 * 5))
		fStars = Rating2Stars((xPos - 1) * 255 / 80);
	//TRACE(_T("xPos: %d Stars: %d\r\n"), xPos, fStars * 100);
	for (int i = 0; i < 5; i++)
	{
		APP_ICONS nIcon = ICO_StarEmpty16;
		if (i == 0)
		{
			nIcon = ICO_StarEmptyBad16;
			if (fStars > 0.0f && fStars <= 1.0f)
				nIcon = ICO_StarBad16;
		}
		else
		{
			if (fStars >= i + .5f)
				nIcon = ICO_StarGold16;
			else if (fStars > i && fStars < i + .5f)
				nIcon = ICO_StarGoldHalf16;

		}
		DrawIconEx(dc.m_hDC, rcIcon.left, rcIcon.top, 
			PRGAPI()->GetIcon(nIcon), 16, 16, 0,0,DI_NORMAL);
		rcIcon.OffsetRect(16, 0);
	}
}


void CMainListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	ReEntryLock lock(m_bReEntryLockCustomDraw);
	if (lock.WasAlreadyLocked())
	{
		TRACE(_T("@0 CMainListCtrl::OnNMCustomdraw. Re-entry\r\n"));
		return;
	}
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	// Take the default processing unless we set this to something else below.
	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.
	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW | CDRF_NEWFONT | CDRF_NOTIFYPOSTPAINT;
		m_bDoNotUseDispOptimization = TRUE;
		break;
	case CDDS_POSTPAINT:
		m_bDoNotUseDispOptimization = FALSE;
		*pResult = CDRF_DODEFAULT;
		break;
	case CDDS_ITEMPREPAINT:
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		{
			*pResult = CDRF_SKIPDEFAULT;
			if ( pLVCD->nmcd.rc.right - pLVCD->nmcd.rc.left > 0 )
			{
				int nItem = static_cast<int>( pLVCD->nmcd.dwItemSpec );
				GetColorsFromItem(nItem, pLVCD->clrTextBk, pLVCD->clrText);
				const int cMargin = 2;
				PrgAPI* pAPI = PRGAPI();
				ColumnsEnum col = SubItem2Column(pLVCD->iSubItem);
				CDC*  pDC = CDC::FromHandle ( pLVCD->nmcd.hdc );
				pDC->SetTextColor(pLVCD->clrText);
				CRect rcSubItem;
				GetRealSubItemRect(nItem, pLVCD->iSubItem, LVIR_BOUNDS, rcSubItem);
				if ((int)m_tracks.size() <= nItem)
				{
					TRACE(_T("@0 CMainListCtrl::OnNMCustomdraw. ItemCount not synced\r\n"));
					ASSERT(0);
					return;
				}
				FullTrackRecordSP& it = m_tracks[nItem];
				const INT bfLen = 300;
				BOOL bDrawMouseOver = FALSE;
				if (m_mouseOverDrawInfo.nItem != -1 &&
					m_mouseOverDrawInfo.nItem == nItem &&
					m_mouseOverDrawInfo.nSubItem == pLVCD->iSubItem)
				{
					bDrawMouseOver = TRUE;
					m_mouseOverDrawInfo.bDrawn = TRUE;
				}
				TCHAR bf[bfLen];
				switch (col)
				{
				case CL_Controls:
					DrawControlsSubItem(*pDC, rcSubItem, pLVCD->clrTextBk, pLVCD->clrText, bDrawMouseOver);
					break;
				case CL_Rating:
					DrawRatingsSubItem(*pDC, rcSubItem, pLVCD->clrTextBk, pLVCD->clrText, bDrawMouseOver, it->track.rating);
					break;
				case CL_Collection://Collection
					{
						pDC->FillSolidRect(rcSubItem, pLVCD->clrTextBk);
						CRect rcIcon(rcSubItem);
						rcIcon.left += 1;
						rcIcon.top += (rcSubItem.Height() - 16) / 2;
						rcIcon.right = rcIcon.left + 16;
						rcIcon.bottom = rcIcon.top + 16;
						DrawIconEx(pLVCD->nmcd.hdc, rcIcon.left, rcIcon.top, 
							pAPI->GetIconForCollectionType(it->collection.type), 16, 16, 0,0,DI_NORMAL);
						rcSubItem.left += 19;
						rcSubItem.right -= cMargin;
						pDC->DrawText(it->collection.name.c_str(), &rcSubItem, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
					}
					break;
				case CL_Title:
					{
						CRect rcIcon(rcSubItem);
						rcIcon.left += 1;
						rcIcon.top += (rcSubItem.Height() - 16) / 2;
						rcIcon.right = rcIcon.left + 16;
						rcIcon.bottom = rcIcon.top + 16;

						pDC->FillSolidRect(rcSubItem, pLVCD->clrTextBk);
						//=== Translation TS_UnknownString
						LPCTSTR txt = it->track.name.c_str();
						if (it->track.name == TS_UnknownString)
							txt = pAPI->GetString(IDS_UNKNOWN);

						CPoint tl = rcSubItem.TopLeft();
						tl.y += 3;
						tl.x += 1;
						if (rcSubItem.Width() > 100)
						{
							DrawIconEx(pLVCD->nmcd.hdc, tl.x, rcIcon.top, 
								pAPI->GetIconForTrackType(it->track.trackType), 16, 16, 0, 0, DI_NORMAL);

							tl.x = rcSubItem.right;
							if (m_bShowInfoIcons)
							{
								if (it->track.flags == TRF_Uninitialized)
									it->track.flags = pAPI->GetSQLManager()->GetTrackInfoFlags(it->track.ID);
								if (it->track.flags & TRF_HasComments)
								{
									tl.Offset(-16, 0);
									DrawIconEx(pLVCD->nmcd.hdc, tl.x, rcIcon.top, 
										pAPI->GetIcon(ICO_Comment16), 16, 16, 0,0,DI_NORMAL);
								}
								if (it->track.flags & TRF_HasPersonal)
								{
									tl.Offset(-16, 0);
									DrawIconEx(pLVCD->nmcd.hdc, tl.x, rcIcon.top, 
										pAPI->GetIcon(ICO_Personal16), 16, 16, 0,0,DI_NORMAL);
								}
								if(it->track.flags & TRF_HasLyrics)
								{
									tl.Offset(-16, 0);
									DrawIconEx(pLVCD->nmcd.hdc, tl.x, rcIcon.top, 
										pAPI->GetIcon(ICO_Lyrics16), 16, 16, 0,0,DI_NORMAL);
								}
								if(it->track.flags & TRF_HasTablatures)
								{
									tl.Offset(-16, 0);
									DrawIconEx(pLVCD->nmcd.hdc, tl.x, rcIcon.top, 
										pAPI->GetIcon(ICO_Tablature16), 16, 16, 0,0,DI_NORMAL);
								}
								if (pAPI->IsTrackNew(it->track.dateAdded))
								{
									tl.Offset(-16, 0);
									DrawIconEx(pLVCD->nmcd.hdc, tl.x, rcIcon.top, 
										pAPI->GetIcon(ICO_New16), 16, 16, 0,0,DI_NORMAL);
								}
							}

							tl.Offset(-2, 0);//Extra Space Before the Letters
							CRect rcText(rcSubItem.left + 16 + 1 + 2, tl.y, tl.x - 1, rcSubItem.bottom - 2);
							rcSubItem.left = tl.x;
							pDC->DrawText(txt, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
						}
						else
						{
							pDC->DrawText(txt, &rcSubItem, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
						}
					}
					break;
				default:
					{
						int a = 0;
						pDC->FillSolidRect(rcSubItem, pLVCD->clrTextBk);
						bf[0] = 0;
						int dtFlags = DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
						//int dtFlags = DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_LEFT | DT_END_ELLIPSIS;
						LPCTSTR drawBf = bf;
						switch (col)
						{
						case CL_Album:
							dtFlags |= DT_LEFT | DT_END_ELLIPSIS;
							_tcsncpy(bf, FixName(it->album.name.c_str()), bfLen);
							break;
						case CL_Artist:
							dtFlags |= DT_LEFT | DT_END_ELLIPSIS;
							_tcsncpy(bf, FixName(it->artist.name.c_str()), bfLen);
							break;
						case CL_TrackNo:
							dtFlags |= DT_CENTER;
							if (it->track.trackNo > 0)
								_sntprintf(bf, bfLen, _T("%02d"), it->track.trackNo);
							break;
						case CL_Path:
							dtFlags |= DT_LEFT | DT_END_ELLIPSIS;
							drawBf = it->track.location.c_str();
							break;
						case CL_Size:
							dtFlags |= DT_RIGHT;
							if (it->track.size > 0)
								_sntprintf(bf, bfLen, _T("%3.2f MB"), it->track.size / 1024.0);
							break;
						case CL_Duration:
							dtFlags |= DT_RIGHT;
							if (it->track.duration > 0)
								PrgAPI::FormatDuration(bf, bfLen, it->track.duration);
							break;
						case CL_Bitrate:
							dtFlags |= DT_RIGHT;
							if (it->track.bitrate)
								_sntprintf(bf, bfLen, _T("%d kbps"), it->track.bitrate);
							break;
						case CL_Year:
							dtFlags |= DT_RIGHT;
							if (it->track.year > 1500)
								_itot(it->track.year, bf, 10);
							break;
						case CL_Genre:
							dtFlags |= DT_LEFT | DT_END_ELLIPSIS;
							_tcsncpy(bf, FixName(it->genre.name.c_str()), bfLen);
							break;
						case CL_DateAdded:
							dtFlags |= DT_LEFT | DT_END_ELLIPSIS;
							PrgAPI::FormatDate(bf, bfLen, it->track.dateAdded);
							break;
						default:
							ASSERT(FALSE);
						}
						rcSubItem.left += cMargin;
						rcSubItem.right -= cMargin;
						pDC->DrawText(drawBf, &rcSubItem, dtFlags);
					}
					break;
				}

			}
		}
		break;
	default:
		*pResult = CDRF_DODEFAULT;
	}
}



//We need to implement this in order GetItemText (Export) to work correctly
void CMainListCtrl::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	return;//TESTING

	//LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	//LV_ITEM* pItem= &(pDispInfo)->item;
	//if (m_bDoNotUseDispOptimization)
	//{
	//	if( pItem->mask & LVIF_TEXT ) 
	//		pItem->pszText[0] = 0;
	//	*pResult = 0;
	//	return;
	//}

	////Do the list need text information?
	//if( pItem->mask & LVIF_TEXT ) 
	//{
	//	int itemid = pItem->iItem;
	//	FullTrackRecordSP it = m_tracks[itemid];
	//	pItem->pszText[0] = 0;
	//	switch (SubItem2Column(pItem->iSubItem))
	//	{
	//	case CL_Controls:
	//		pItem->pszText[0] = 0;
	//		break;
	//	case CL_Title:
	//		_tcsncpy(pItem->pszText, it->track.name.c_str(), pItem->cchTextMax);
	//		break;
	//	case CL_Album:
	//		_tcsncpy(pItem->pszText, it->album.name.c_str(), pItem->cchTextMax);
	//		FixName(pItem->pszText, pItem->cchTextMax);
	//		break;
	//	case CL_Artist:
	//		_tcsncpy(pItem->pszText, it->artist.name.c_str(), pItem->cchTextMax);
	//		FixName(pItem->pszText, pItem->cchTextMax);
	//		break;
	//	case CL_Rating:
	//		pItem->pszText[0] = 0;//m_schema.GetName(pItem->pszText, pItem->cchTextMax);
	//		break;
	//	case CL_TrackNo:
	//		if (it->track.trackNo > 0)
	//			_sntprintf(pItem->pszText, pItem->cchTextMax, _T("%02d"), it->track.trackNo);
	//		break;
	//	case CL_Path:
	//		_tcsncpy(pItem->pszText, it->track.location.c_str(), pItem->cchTextMax);
	//		break;
	//	case CL_Collection:
	//		_tcsncpy(pItem->pszText, it->collection.name.c_str(), pItem->cchTextMax);
	//		break;
	//	case CL_Size:
	//		if (it->track.size)
	//			_sntprintf(pItem->pszText, pItem->cchTextMax, _T("%3.2f Mb"), it->track.size / 1024.0);
	//		break;
	//	case CL_Duration:
	//		if (it->track.duration > 0)
	//			PrgAPI::FormatDuration(pItem->pszText, pItem->cchTextMax, it->track.duration);
	//		break;
	//	case CL_Bitrate:
	//		if (it->track.bitrate)
	//			_sntprintf(pItem->pszText, pItem->cchTextMax, _T("%d kbps"), it->track.bitrate);
	//		break;
	//	case CL_Year:
	//		if (it->track.year > 1900)
	//			_itot(it->track.year, pItem->pszText, 10);
	//		break;
	//	case CL_Genre:
	//		_tcsncpy(pItem->pszText, it->genre.name.c_str(), pItem->cchTextMax);
	//		FixName(pItem->pszText, pItem->cchTextMax);
	//		break;
	//	case CL_DateAdded:
	//		PRGAPI()->FormatDate(pItem->pszText, pItem->cchTextMax, it->track.dateAdded);
	//		break;
	//	default:
	//		ASSERT(FALSE);
	//	}

	//}

	//*pResult = 0;
}


void CMainListCtrl::OnDestroy()
{
	UpdateColumnInfo();
	__super::OnDestroy();
}

UINT CMainListCtrl::GetTrackID(int nItem)
{
	ASSERT(nItem < GetItemCount());
	return m_tracks[nItem]->track.ID;
}


void CMainListCtrl::OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	TRACEST(_T("CMainListCtrl::OnHdnItemclick"));
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	if (phdr->iButton == 0) //Left Button
	{
		RecordSortOptionsEnum rso = SubItem2RecordSortOption(phdr->iItem);
		BOOL bSortOptionExists = FALSE;
		for (UINT i = 0; i < m_SOC.GetSortOptionsCount(); i++)
		{
			SortOption so = m_SOC.GetSortOption(i);
			if (so.option == rso)
			{
				so.ascending = !so.ascending;
				m_SOC.ReverseSortOption(i);
				bSortOptionExists = TRUE;
				break;
			}
		}
		if (!bSortOptionExists)
		{
			BOOL Shift = GetKeyState (VK_SHIFT) < 0;
			if (!Shift)
				m_SOC.ClearSortOptions();
			m_SOC.ApplySortOption(SortOption(rso, TRUE));
		}
		//RecordCollectionSorter::SortFullTrackRecordCollection(m_tracks, m_SOC);
		UpdateSorting();
		Invalidate(TRUE);
	}
	*pResult = 0;
}

CMainListCtrl::ColumnsEnum CMainListCtrl::SubItem2Column(INT iSubItem)
{
	HDITEM hd;
	memset(&hd, 0, sizeof(HDITEM));
	hd.mask = HDI_LPARAM;
	GetHeaderCtrl()->GetItem(iSubItem, &hd);
	return ColumnsEnum(hd.lParam);
}




INT CMainListCtrl::Column2SubItem(ColumnsEnum col)
{
	ASSERT(col >= CL_First && col < CL_Last);
	INT hdCount = GetHeaderCtrl()->GetItemCount();
	ASSERT(hdCount > 0);
	for (int i = 0; i < hdCount; i++)
	{
		if (SubItem2Column(i) == col)
			return i;
	}
	TRACE(_T("@2CMainListCtrl::Column2SubItem. SubItem not found"));
	return -1;//Error
}

void CMainListCtrl::UpdateSorting()
{
	CHeaderCtrl* pHeader = GetHeaderCtrl();
	if (pHeader != NULL)
	{
		HDITEM hdi;
		memset(&hdi, 0, sizeof(HDITEM));
		hdi.mask = HDI_FORMAT;
		hdi.fmt =  HDF_STRING;
		for (int i=0; i < pHeader->GetItemCount(); i++)
		{
			pHeader->SetItem(i, &hdi);
		}
		for (UINT i=0; i<m_SOC.GetSortOptionsCount(); i++)
		{
			SortOption so = m_SOC.GetSortOption(i);
			hdi.fmt =  HDF_STRING | (so.ascending ? HDF_SORTDOWN : HDF_SORTUP);
			pHeader->SetItem(RecordSortOptions2SubItem(so.option), &hdi);
		}
	}

	std::vector<UINT> selIDs;
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos != NULL)
		selIDs.push_back(m_tracks[GetNextSelectedItem(pos)]->track.ID);

	RecordCollectionSorter::SortFullTrackRecordCollection(m_tracks, m_SOC);

	INT iPos = 0;
	FullTrackRecordCollection::iterator it = m_tracks.begin();
	for (; it != m_tracks.end(); it++)
	{
		BOOL bFound = FALSE;
		INT trackID = (*it)->track.ID;
		for (INT i = 0; i < (INT)selIDs.size(); i++)
		{
			if (trackID == selIDs[i])
			{
				bFound = TRUE;
				break;
			}
		}
		SetItemState(iPos, bFound ? LVIS_SELECTED : 0, LVIS_SELECTED);
		iPos++;
	}


}

void CMainListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rcHeader;
	//GetClientRect(&rc);
	GetHeaderCtrl()->GetClientRect(&rcHeader);
	//rc.top = rcHeader.bottom;
	CPoint sPoint(point);
	ClientToScreen(&sPoint);
	INT nItem = -1;
	if (::WindowFromPoint(sPoint) == m_hWnd)
	{
		nItem = HitTest(point);
		if (rcHeader.PtInRect(point))
			nItem = -1;
	}
	if (nItem >= 0)
	{
		if (::GetCapture() != m_hWnd)
		{
			SetCapture();
		}
	}
	else
	{
		if (::GetCapture() == m_hWnd)
		{
			ReleaseCapture();
			ResetMouseOver();
		}
	}

	BOOL bHandled = FALSE;
	if (nItem != -1)
	{
		LVHITTESTINFO htInfo;
		GetCursorPos(&htInfo.pt);
		ScreenToClient(&htInfo.pt);
		SubItemHitTest(&htInfo);
		if (htInfo.iItem >= 0)
		{
			ColumnsEnum col = SubItem2Column(htInfo.iSubItem);
			if (col == CL_Controls || col == CL_Rating)
			{
				CRect rc;
				if (htInfo.iItem != m_mouseOverDrawInfo.nItem || 
					m_mouseOverDrawInfo.nSubItem != htInfo.iSubItem)
					ResetMouseOver();
				GetRealSubItemRect(htInfo.iItem, htInfo.iSubItem, LVIR_BOUNDS, rc);
				m_mouseOverDrawInfo.nItem = htInfo.iItem;
				m_mouseOverDrawInfo.nSubItem = htInfo.iSubItem;
				m_mouseOverDrawInfo.bDrawn = FALSE;
				m_mouseOverDrawInfo.xPos = htInfo.pt.x - rc.left;
				InvalidateRect(&rc, FALSE);
				bHandled = TRUE;
			}
		}
	}
	if (!bHandled && m_mouseOverDrawInfo.nItem != -1)
		ResetMouseOver();
	CWnd::OnMouseMove(nFlags, point);
}

void CMainListCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	TRACEST(_T("CMainListCtrl::OnContextMenu"));
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
			PrgAPI* pAPI = PRGAPI();
			if (!m_MainMenu.m_hMenu)//Create On Demand
			{
				ITSMenu& mnuServices = pAPI->GetMenuManager()->GetMenu(MenuManager::MNU_TrackServices);
				m_RenamerMenu.CreatePopupMenu();
				m_RenamerMenu.SetFont(pAPI->GetFont(FONT_Dialogs));
				m_RenamerMenu.AppendMenu(MF_STRING, ACMD_Renamer, (LPTSTR)pAPI->GetString(IDS_MANUALLY), pAPI->GetIcon(ICO_Properties16));
				m_RenamerMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);

				INT i = 0;
				while (TRUE)
				{
					LPCTSTR curPat = FileNameTagger::GetWritePredifinedFormat(i);
					if (curPat == NULL)
						break;
					TCHAR newFileTitle[MAX_PATH];
					if (FileNameTagger::TranslatePattern(newFileTitle, MAX_PATH, curPat, 
						NULL,NULL,NULL,NULL,NULL,NULL))
						m_RenamerMenu.AppendMenu(MF_STRING, MENU_RenamerOptions + i, newFileTitle);
					i++;
				}

				m_FileNameTaggerMenu.CreatePopupMenu();
				m_FileNameTaggerMenu.SetFont(pAPI->GetFont(FONT_Dialogs));
				m_FileNameTaggerMenu.AppendMenu(MF_STRING, ACMD_GetTagsFromFilename, (LPTSTR)pAPI->GetString(IDS_MANUALLY), pAPI->GetIcon(ICO_Properties16));
				m_FileNameTaggerMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);

				i = 0;
				while (TRUE)
				{
					LPCTSTR pat = FileNameTagger::GetReadPredifinedFormat(i);
					if (pat == NULL)
						break;
					TCHAR bf[500];
					if (FileNameTagger::TranslatePattern(bf, 500, pat,
						NULL, NULL,(LPCTSTR) NULL, NULL, (LPCTSTR)NULL, NULL))
						m_FileNameTaggerMenu.AppendMenu(MF_STRING, MENU_ReadTagsFromFilename + i, bf);
					i++;
				}
				
				m_CopyMenu.CreatePopupMenu();
				m_CopyMenu.SetFont(pAPI->GetFont(FONT_Dialogs));
				m_CopyMenu.AppendMenu(MF_STRING, ACMD_CopyToCLIPBOARD, (LPTSTR)pAPI->GetString(IDS_CLIPBOARD));
				m_CopyMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
				m_CopyMenu.AppendMenu(MF_STRING, ACMD_CopyToFOLDER, (LPTSTR)pAPI->GetString(IDS_FOLDER));
				m_ChangeCaseMenu.CreatePopupMenu();
				m_ChangeCaseMenu.SetFont(pAPI->GetFont(FONT_Dialogs));
				for (INT i = 0; i<CASE_Last; i++)
				{
					m_ChangeCaseMenu.AppendMenu(MF_STRING, ID_CASESTYLE + i, 
						(LPTSTR) pAPI->GetStringForCaseStyle((CaseStyleEnum)i));
				}


				ITSMenu& mnuRatings = pAPI->GetMenuManager()->GetMenu(MenuManager::MNU_Rating); 

				m_AdvancedMenu.CreatePopupMenu();
				m_AdvancedMenu.SetFont(pAPI->GetFont(FONT_Dialogs));
				m_AdvancedMenu.SetIconSize(16,16);
				TCHAR bf[1000];
				_sntprintf(bf, 300, _T("%s (%s)"), 
					(LPTSTR)pAPI->GetString(IDS_PLAY), (LPTSTR)pAPI->GetString(IDS_EXTERNALPLAYER));
				m_AdvancedMenu.AppendMenu(MF_STRING, ACMD_PlayExternal, 
					bf, pAPI->GetIcon(ICO_Play16));
				_sntprintf(bf, 300, _T("%s (%s)"), 
					(LPTSTR)pAPI->GetString(IDS_ENQUEUE), (LPTSTR)pAPI->GetString(IDS_EXTERNALPLAYER));
				m_AdvancedMenu.AppendMenu(MF_STRING, ACMD_EnqueueExternal, 
					bf, pAPI->GetIcon(ICO_Add16));
				m_AdvancedMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
				m_AdvancedMenu.AppendMenu(MF_STRING | MF_POPUP, (UINT) m_CopyMenu.m_hMenu, 
					(LPTSTR)pAPI->GetString(IDS_COPYTO), pAPI->GetIcon(ICO_Copy16));
				m_AdvancedMenu.AppendMenu(MF_STRING, ACMD_CopyPath, (LPTSTR)pAPI->GetString(IDS_COPYPATH));
				m_AdvancedMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
				m_AdvancedMenu.AppendMenu(MF_STRING, ACMD_ReadTags, (LPTSTR)pAPI->GetString(IDS_READTAGS));
				m_AdvancedMenu.AppendMenu(MF_STRING, ACMD_WriteTags, (LPTSTR)pAPI->GetString(IDS_WRITETAGS));
				m_AdvancedMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);

				m_AdvancedMenu.AppendMenu(MF_STRING | MF_POPUP, (UINT) m_FileNameTaggerMenu.m_hMenu, (LPTSTR)pAPI->GetString(IDS_GETTAGSFROMFILENAME));
				m_AdvancedMenu.AppendMenu(MF_STRING | MF_POPUP, (UINT) m_RenamerMenu.m_hMenu, (LPTSTR)pAPI->GetString(IDS_RENAME));
				
				m_AdvancedMenu.AppendMenu(MF_STRING | MF_POPUP, (UINT) m_ChangeCaseMenu.m_hMenu, (LPTSTR)pAPI->GetString(IDS_CHANGECASE));
				m_AdvancedMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
				m_AdvancedMenu.AppendMenu(MF_STRING, ACMD_Delete, 
					(LPTSTR)pAPI->GetString(IDS_DELETE), pAPI->GetIcon(ICO_Delete16));
				m_AdvancedMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
				m_AdvancedMenu.AppendMenu(MF_STRING, ID_MAINLISTNORMAL_EXPORT_SELECTION, (LPTSTR)pAPI->GetString(IDS_EXPORTSELECTEDLINES));
				m_AdvancedMenu.AppendMenu(MF_STRING, ID_MAINLISTNORMAL_EXPORT, (LPTSTR)pAPI->GetString(IDS_EXPORTALL));

				m_MainMenu.CreatePopupMenu();
				m_MainMenu.SetFont(pAPI->GetFont(FONT_Dialogs));
				m_MainMenu.SetIconSize(16,16);
				
				_sntprintf(bf, 1000, _T("%s\t%s"), pAPI->GetString(IDS_PLAY), pAPI->GetString(IDS_KEYENTER));
				m_MainMenu.AppendMenu(MF_STRING, ACMD_Play, bf, pAPI->GetIcon(ICO_Play16));
				_sntprintf(bf, 1000, _T("%s\t%s + %s"), pAPI->GetString(IDS_ENQUEUE), pAPI->GetString(IDS_KEYSHIFT), pAPI->GetString(IDS_KEYENTER));
				m_MainMenu.AppendMenu(MF_STRING, ACMD_Enqueue, bf, pAPI->GetIcon(ICO_Add16));
				m_MainMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
				m_MainMenu.AppendMenu(MF_STRING, ACMD_LocateAlbum, 
					(LPTSTR)pAPI->GetString(IDS_LOCATEALBUM));
				m_MainMenu.AppendMenu(MF_STRING, ACMD_OpenFolder, 
					(LPTSTR)pAPI->GetString(IDS_OPENCONTAININGFOLDER), pAPI->GetIcon(ICO_Find16));
				m_MainMenu.AppendMenu(MF_STRING | MF_POPUP, (UINT) m_AdvancedMenu.m_hMenu, 
					(LPTSTR)pAPI->GetString(IDS_ADVANCED));	
				m_MainMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
				m_MainMenu.AppendMenu(MF_STRING | MF_POPUP, (UINT) mnuServices.GetInternalHandler(), 
					(LPTSTR)pAPI->GetString(IDS_SERVICES), pAPI->GetIcon(ICO_Providers16));	
				
				InfoProviderFactory* pIPF = pAPI->GetInfoProviderFactory();
				INT itCount = m_ProviderRequests.size();
				INT ipIdx = 0;
				while (pIPF->GetItemByIndex(ipIdx) != NULL)
				{
					if (pIPF->ForPublicUse(ipIdx))
					{
						const IInfoProvider* pIP = pIPF->GetItemByIndex(ipIdx);
						if (pIP->CanHandle(IInfoProvider::SRV_TrackLyrics))
							AppendInfoProviderMenuItem(m_MainMenu, *pIP, IIT_TrackLyrics);
						if (pIP->CanHandle(IInfoProvider::SRV_TrackTablatures))
							AppendInfoProviderMenuItem(m_MainMenu, *pIP, IIT_TrackTablatures);
					}
					ipIdx++;
				}
				m_MainMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
				m_MainMenu.AppendMenu(MF_STRING | MF_POPUP, (UINT) mnuRatings.GetInternalHandler(), 
					(LPTSTR)pAPI->GetString(IDS_RATING), pAPI->GetIcon(ICO_StarGold16));


				_sntprintf(bf, 500, _T("%s - %s - %s"), 
					pAPI->GetString(IDS_PREVIEW), 
					pAPI->GetString(IDS_SIMILAR), 
					pAPI->GetString(IDS_PURCHASEINFO));
				m_MainMenu.AppendMenu(MF_STRING, ACMD_PurchaseInfo, 
					bf, pAPI->GetIcon(ICO_Buy16));
				_sntprintf(bf, 1000, _T("%s\t%s"), pAPI->GetString(IDS_PROPERTIES), pAPI->GetString(IDS_KEYHOME));
				m_MainMenu.AppendMenu(MF_STRING, ACMD_Properties, bf, pAPI->GetIcon(ICO_Properties16));
				m_MainMenu.SetDefaultItem(ACMD_Play, FALSE);
			}


			ActionManager* am = PRGAPI()->GetActionManager();
			int itemCount = GetSelectedCount();
			
			BOOL bDeleteAccess = TRUE;
			BOOL bPlayAccess = TRUE;

			m_CopyMenu.RemoveMenu(ACMD_CopyToLASTFOLDER1, MF_BYCOMMAND);
			LPCTSTR targFolder = am->GetLastTargetFolder();
			if (targFolder != NULL && targFolder[0] != 0)
			{
				TCHAR copyto[200];
				UINT len = _tcslen(targFolder);
				_sntprintf(copyto, 200, 
					_T("%s '%s'"), 
					PRGAPI()->GetString(IDS_FOLDER),
					(len > 15) ? &targFolder[len - 12] : targFolder
					);
				m_CopyMenu.AppendMenu(MF_STRING, ACMD_CopyToLASTFOLDER1, copyto);
			}

			m_MainMenu.EnableMenuItem(ACMD_Play, bPlayAccess ? MF_ENABLED : MF_GRAYED);
			m_MainMenu.EnableMenuItem(ACMD_Enqueue, bPlayAccess ? MF_ENABLED : MF_GRAYED);
			//m_MainMenu.EnableMenuItem(ACMD_PlayExternal, (bPlayAccess && (itemCount==1)) ? MF_ENABLED : MF_GRAYED);
			//m_MainMenu.EnableMenuItem(ACMD_EnqueueExternal, (bPlayAccess && (itemCount==1)) ? MF_ENABLED : MF_GRAYED);
			//m_MainMenu.EnableMenuItem(ACMD_OpenFolder, (readable && (itemCount==1)) ? MF_ENABLED : MF_GRAYED);
			m_MainMenu.EnableMenuItem(ACMD_CopyPath, (itemCount==1) ? MF_ENABLED : MF_GRAYED);
			m_MainMenu.EnableMenuItem(ACMD_CopyTo, MF_ENABLED);
			m_MainMenu.EnableMenuItem(ACMD_ReadTags, MF_ENABLED);
			m_MainMenu.EnableMenuItem(ACMD_WriteTags, MF_ENABLED);
			
			m_MainMenu.EnableMenuItem(ACMD_LocateAlbum, (itemCount==1) ? MF_ENABLED : MF_GRAYED);
		
			m_MainMenu.EnableMenuItem((UINT) m_RenamerMenu.m_hMenu, MF_ENABLED);
			m_MainMenu.EnableMenuItem((UINT) m_CopyMenu.m_hMenu, MF_ENABLED);

			m_MainMenu.EnableMenuItem(ACMD_Delete, bDeleteAccess ? MF_ENABLED : MF_GRAYED);
			pAPI->GetMenuManager()->PrepareMenu(MenuManager::MNU_TrackServices);
			m_MainMenu.TrackPopupMenu(0, point.x, point.y, this);
		}
		else
		{
			TRACE(_T("@4CMainListCtrl::OnContextMenu. Right Clicked On Empty Space\n"));
		}
	}
	else
	{
		if (!m_ColumnsMenu.m_hMenu)//CreateOnDemand
		{
			PrgAPI* pLang = PRGAPI();
			m_ColumnsMenu.CreatePopupMenu();
			m_ColumnsMenu.SetFont(pLang->GetFont(FONT_Dialogs));
			for (int i = CL_First; i < CL_Last; i++)
				m_ColumnsMenu.AppendMenu(MF_STRING, ID_COLUMNMSG + i, (LPTSTR)pLang->GetString(defRes[i]));
			m_ColumnsMenu.EnableMenuItem(0, MF_BYPOSITION | MF_GRAYED);
		}
		for (int i = CL_First; i < CL_Last; i++)
		{
			int chk = MF_CHECKED;
			if (!m_columns[i].bVisible)
				chk = MF_UNCHECKED;
			m_ColumnsMenu.CheckMenuItem(i, MF_BYPOSITION | chk);
		}
		m_ColumnsMenu.TrackPopupMenu(0, point.x, point.y, this);
#ifdef _DEBUG
		pHeader->ScreenToClient(&point);
		int index = -1;
		for( int i=0; pHeader->GetItemRect( i, &rcCol); i++ )
		{
			if( rcCol.PtInRect( point ) )
			{
				index = i;
				break;
			}
		}
		ASSERT(index != -1);
		TRACE(_T("@4CMainListCtrl::OnContextMenu. Header Column Clicked. Index: %d\n"), index);
#endif
	}

}

BOOL CMainListCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CMainListCtrl::OnCommand"), wParam);
	BOOL handled = TRUE;
	//Column Menu
	if (wParam >= ID_COLUMNMSG && wParam < ID_COLUMNMSG + CL_Last)
	{
		ShowColumn(ColumnsEnum(wParam - ID_COLUMNMSG), !m_columns[wParam - ID_COLUMNMSG].bVisible);
		return TRUE;
	}
	if (GetSelectedCount() > 0)
	{
		m_selectedTracks.clear();
		GetSelectedTracks(m_selectedTracks);
		PrgAPI* pAPI = PRGAPI();
		ActionManager* am = pAPI->GetActionManager();
		MediaPlayer* mp = pAPI->GetMediaPlayer();
		switch (wParam)
		{
		case ACMD_Play:
		case ACMD_Enqueue:
		case ACMD_EnqueueAfter:
		case ACMD_PlayExternal:
		case ACMD_EnqueueExternal:
			{
				FullTrackRecordCollection col;
				GetSelectedTracks(col);
				switch (wParam)
				{
				case ACMD_Play:
					am->Play(mp, col);
					break;
				case ACMD_Enqueue:
					am->Enqueue(mp, col);
					break;
				case ACMD_EnqueueAfter:
					am->EnqueueAfter(mp, col);
					break;
				case ACMD_PlayExternal:
					am->ExternalOpen(m_selectedTracks, _T(""));
					break;
				case ACMD_EnqueueExternal:
					am->ExternalOpen(m_selectedTracks, _T("enqueue"));
					break;
				}
			}
			break;
		case ACMD_LocateAlbum:
			ASSERT(GetSelectedCount() == 1);
			if (GetSelectedCount() == 1)
			{
				pAPI->GetStateManager()->GetState().locateTrackID = m_selectedTracks[0]->track.ID;
				pAPI->SendMessage(SM_LocateTrackRequest);
			}
			break;
		case ACMD_OpenFolder:
			ASSERT(GetSelectedCount() == 1);
			if (GetSelectedCount() == 1)
				am->OpenFolderToItem(m_selectedTracks[0]->track.location.c_str(), m_selectedTracks[0]->collection.type);
			break;
		case ACMD_CopyPath:
			ASSERT(GetSelectedCount() == 1);
			if (GetSelectedCount() == 1)
				am->CopyPathNameToClipboard(m_selectedTracks[0]->track.location.c_str(), m_selectedTracks[0]->collection.type);
			break;
		case ACMD_ReadTags:
			am->ReadTags(m_selectedTracks);
			break;
		case ACMD_WriteTags:
			am->WriteTags(m_selectedTracks);
			break;
		case ACMD_CopyToFOLDER:
			am->CopyFilesToFolder(m_selectedTracks);
			break;
		case ACMD_CopyToLASTFOLDER1:
			am->CopyFilesToFolder(m_selectedTracks, am->GetLastTargetFolder());
			break;
		case ACMD_CopyToCLIPBOARD:
			am->CopyFilesToClipboard(m_selectedTracks);
			break;
		case ACMD_Renamer:
			{
				CRenamerDlg dlg(m_selectedTracks, this);
				dlg.DoModal();
				if (dlg.IsSomethingChanged())
					Invalidate(FALSE);
			}
			break;
		case ACMD_GetTagsFromFilename:
			{
				CTagsFromFilenameDlg dlg(m_selectedTracks, this);
				dlg.DoModal();
				if (dlg.IsSomethingChanged())
					pAPI->SendMessage(SM_DatabaseUpdated);
			}
			break;
		case ACMD_Delete:
			if (pAPI->MessageBox(m_hWnd, IDS_DELETEOPERATIONTEXT,
				IDS_DELETEOPERATION, MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				if (am->DeleteFiles(m_selectedTracks))
				{
					m_selectedTracks.clear();
					int size = m_tracks.size();
					for (int i = 0; i <size; i++)
					{
						if (m_tracks[i]->track.ID == 0)
						{
							m_tracks.erase(m_tracks.begin() + i);
							size--;
							i--;
						}
					}
					pAPI->SendMessage(SM_DatabaseUpdated);
				}
			}
			break;
		case ACMD_PurchaseInfo:
			if (m_selectedTracks.size() > 0)
			{
				am->GoToTrackBuyPage(m_selectedTracks[0]->artist.name.c_str(), 
					m_selectedTracks[0]->track.name.c_str());
			}
			break;
		case ACMD_Properties:
			if (m_selectedTracks.size() == 1)
				am->ShowTrackPropertiesDlg(m_selectedTracks[0], GetParent());
			else if (m_selectedTracks.size() > 1)
				am->ShowMultipleTrackPropertiesDlg(m_selectedTracks, GetParent());
			break;
		case ID_MAINLISTNORMAL_EXPORT:
		case ID_MAINLISTNORMAL_EXPORT_SELECTION:
			{
				CFileDialog fd(FALSE, _T(".xml"), NULL, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, 
							   _T("Xml Files (*.xml)|*.xml|All Files (*.*)|*.*||"), NULL);
				if (fd.DoModal() == IDOK)
				{
					DataRecordsXmlSerializer::Export(fd.GetFileName(), 
						wParam == ID_MAINLISTNORMAL_EXPORT ? m_tracks : m_selectedTracks);
				}
			}
			break;
		default:
			if (wParam >= MENU_RatingHolder && wParam <= MENU_RatingHolder+5)
			{
				am->SetTrackRating(m_selectedTracks, Stars2Rating((FLOAT)(wParam - MENU_RatingHolder)));
				//Invalidate();
			}
			else if (wParam >= ID_CASESTYLE && wParam < ID_CASESTYLE+CASE_Last)
			{
				if (am->ChangeCaseFiles(m_selectedTracks, CaseStyleEnum(wParam - ID_CASESTYLE)))
					Invalidate();
			}
			else if (wParam >= MENU_RenamerOptions && wParam < MENU_RenamerOptionsEnd)
			{
				m_selectedTracks.clear();
				GetSelectedTracks(m_selectedTracks);
				LPCTSTR pat = FileNameTagger::GetWritePredifinedFormat(wParam - MENU_RenamerOptions);
				if (pat != NULL)
				{
					FileNameTagger fnTagger;
					TCHAR newPathName[MAX_PATH];
					for (size_t i = 0; i < m_selectedTracks.size(); i++)
					{
						FullTrackRecordSP& rec = m_selectedTracks[i];
						if (fnTagger.CustomWrite(newPathName, 
							rec->track.location.c_str(),  
							pat,
							rec->artist.name.c_str(), 
							rec->album.name.c_str(), 
							rec->track.trackNo, 
							rec->track.name.c_str(), 
							rec->track.year))
						{
							rec->track.location = newPathName;
							if (pAPI->GetSQLManager()->UpdateTrackRecord(rec->track))
							{
								//operationSucceeds++;
							}

						}
					}
					Invalidate(FALSE);
				}

			}
			else if (wParam >= MENU_ReadTagsFromFilename && wParam < MENU_ReadTagsFromFilenameEnd)
			{
				LPCTSTR pat = FileNameTagger::GetReadPredifinedFormat(wParam - MENU_ReadTagsFromFilename);
				if (pat != NULL)
				{
					m_selectedTracks.clear();
					GetSelectedTracks(m_selectedTracks);
					SQLManager* pSM = pAPI->GetSQLManager();
					FileNameTagger fnTagger;
					TagInfo ti;
					BOOL bRefreshData = FALSE;
					for (size_t i = 0; i < m_selectedTracks.size(); i++)
					{
						FullTrackRecordSP& rec = m_selectedTracks[i];
						INT aquiredFields = fnTagger.CustomRead(rec->track.location.c_str(), ti, pat);
						if (aquiredFields != TagInfo_None)
						{
							ti.validFields |= aquiredFields;
							if (pSM->UpdateTrackFromTagInfo(rec->track.ID, 
								rec->collection.ID, rec->track.location.c_str(), ti))
							{
								bRefreshData = TRUE;
							}
						}
					}
					if (bRefreshData)
						pAPI->SendMessage(SM_DatabaseUpdated);
				}
			}
			else if (wParam >= MENU_WebServicesHolder && wParam < MENU_WebServicesHolder + MENU_WebServicesMAX)
			{
				ASSERT(GetSelectedCount() == 1);
				if (GetSelectedCount() == 1)
				{
					ServicesRegistrationSrv* srv = PRGAPI()->GetServicesRegistrationSrv();
					ServicesExecuteInfo sei;
					sei.artist = m_selectedTracks[0]->artist.name.c_str();
					sei.album = m_selectedTracks[0]->album.name.c_str();
					sei.title = m_selectedTracks[0]->track.name.c_str();
					ServiceInfo si;
					if (srv->GetServiceInfo(wParam - MENU_WebServicesHolder, si))
						srv->Execute(si, sei, srv->GetWebViewer());
				}
			}
			else if (wParam >= ID_PROVIDERS && wParam < ID_PROVIDERS + m_ProviderRequests.size())
			{
				InfoDownloadManager* pIDM = PRGAPI()->GetInfoDownloadManager();
				InfoItemTypeEnum iit = m_ProviderRequests[wParam - ID_PROVIDERS].iit;
				switch (iit)
				{
				case IIT_TrackLyrics:
					{
						FullTrackRecordCollection::const_iterator it = m_selectedTracks.begin();
						for (; it!= m_selectedTracks.end(); it++)
							pIDM->RequestLyrics((*it)->track, (*it)->artist.name.c_str(), m_ProviderRequests[wParam - ID_PROVIDERS].provID.c_str(), TRUE);
					}
					break;
				case IIT_TrackTablatures:
					{
						FullTrackRecordCollection::const_iterator it = m_selectedTracks.begin();
						for (; it!= m_selectedTracks.end(); it++)
							pIDM->RequestTablatures((*it)->track, (*it)->artist.name.c_str(), m_ProviderRequests[wParam - ID_PROVIDERS].provID.c_str(), TRUE);
					}
					break;
				default:
					ASSERT(0);
					break;
				}
			}
			else
			{
				TRACE(_T("@1CMainListCtrl::OnCommand. Unknown Command\n"));
				ASSERT(0);
			}
		}
	}
	else
	{
		handled = FALSE;
		TRACE(_T("@1CMainListCtrl::OnCommand. No Selection"));
		ASSERT(0);
	}
	if (!handled)
		handled = CWnd::OnCommand(wParam, lParam);
	return handled;
}

BOOL CMainListCtrl::Create(DWORD style, CRect& rc, CWnd* pParent, UINT nID)
{
	if (CListCtrl::Create(style | WS_CHILD | LVS_REPORT | LVS_OWNERDATA | LVS_SHOWSELALWAYS , rc, pParent, nID))
	{
		Init();
		return TRUE;
	}
	return FALSE;
}

void CMainListCtrl::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;   // return value ignored
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UINT uBuffSize = 0;
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos != NULL)
		uBuffSize += m_tracks[GetNextSelectedItem(pos)]->track.location.size() + 1;
	uBuffSize = sizeof(DROPFILES) + sizeof(TCHAR) * (uBuffSize + 1);
	//== Allocate memory from the heap for the DROPFILES struct.
	HGLOBAL hgDrop = GlobalAlloc(GHND | GMEM_SHARE, uBuffSize);
	if (hgDrop == NULL)
	{
		TRACE(_T("@1 CMainListCtrl::OnLvnBegindrag. GlobalAlloc FAILED\r\n"));
		return;
	}
	DROPFILES* pDrop = (DROPFILES*) GlobalLock (hgDrop);
	if (pDrop == NULL)
	{
		TRACE(_T("@1 CMainListCtrl::OnLvnBegindrag. GlobalLock FAILED\r\n"));
		GlobalFree(hgDrop);
		return;
	}
	// Fill in the DROPFILES struct.
	pDrop->pFiles = sizeof(DROPFILES);
#ifdef _UNICODE
	pDrop->fWide = TRUE;
#else
	pDrop->fWide = FALSE;
#endif
	// Copy all the filenames into memory after the end of the DROPFILES struct.
	TCHAR* pszBuff = (TCHAR*) (LPBYTE(pDrop) + sizeof(DROPFILES));
	pos = GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		INT nItem = GetNextSelectedItem(pos);
		INT len = m_tracks[nItem]->track.location.size();
		_tcsncpy(pszBuff, m_tracks[nItem]->track.location.c_str(), len);
		pszBuff[len] = 0;
		pszBuff = &pszBuff[len + 1];
	}
	*pszBuff = 0;
	GlobalUnlock ( hgDrop );
	// Put the data in the data source.
	FORMATETC etc = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
	COleDataSource datasrc;
	datasrc.CacheGlobalData ( CF_HDROP, hgDrop, &etc );
	DROPEFFECT dwEffect = datasrc.DoDragDrop ( DROPEFFECT_COPY /*| DROPEFFECT_MOVE*/ );
	GlobalFree ( hgDrop );

}



void CMainListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	ColumnsEnum col = SubItem2Column(pNMIA->iSubItem);
	if (pNMIA->iItem >= 0)//WARNING pNMIA->iItem >= 0 because i got once -1
	{
		TRACEST(_T("CMainListCtrl::OnNMClick item:"), pNMIA->iItem);
		switch (col)
		{
		case CL_Rating:
			{
				CRect rc;
				GetRealSubItemRect(pNMIA->iItem, pNMIA->iSubItem, LVIR_BOUNDS /*| LVIR_ICON*/, rc);
				//FLOAT star = (pNMIA->ptAction.x - rc.left) / 16.0f + 1.0f;

				//fStars = Rating2Stars((xPos - 1) * 255 / 80);
				INT newRating = (pNMIA->ptAction.x - rc.left - 1) * 255 / 80;
				if (newRating > 0 && newRating < 256)
				{
					FullTrackRecordCollection col;
					col.push_back(m_tracks[pNMIA->iItem]);
					PRGAPI()->GetActionManager()->SetTrackRating(col, newRating);
					InvalidateRect(&rc, TRUE);
				}

			}
			break;
		case CL_Controls:
			{
				CRect rc;
				GetRealSubItemRect(pNMIA->iItem, pNMIA->iSubItem, LVIR_BOUNDS /*| LVIR_ICON*/, rc);
				INT posx = pNMIA->ptAction.x  - rc.left;
				if (posx <= 49)
				{
					ActionManager* am = PRGAPI()->GetActionManager();
					MediaPlayer* mp = PRGAPI()->GetMediaPlayer();
					FullTrackRecordCollection col;
					col.push_back(m_tracks[pNMIA->iItem]);
					if (posx <= 17)
						am->Play(mp, col);
					else if (posx <= 33)
						am->EnqueueAfter(mp, col);
					else if (posx <= 49)
						am->Enqueue(mp, col);
				}
			}
			break;
		}
		NotifySelectedItemChanged();
	}
	*pResult = TRUE;
	
}

BOOL CMainListCtrl::SelectItemByID(UINT TrackID)
{
	ClearSelection();
	BOOL bFound = FALSE;
	for (UINT i = 0; i < m_tracks.size(); i++)
	{
		if (TrackID == m_tracks[i]->track.ID)
		{
			EnsureVisible(i, FALSE);
			SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			bFound = TRUE;
			NotifySelectedItemChanged();
			break;
		}
	}
	return bFound;
}

void CMainListCtrl::OnSelectAll()
{
	for (int i = 0; i < GetItemCount(); i++)
		SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
}

void CMainListCtrl::OnInvertSelection()
{
	for (int i = 0; i < GetItemCount(); i++)
		SetItemState(i, GetItemState(i, LVIS_SELECTED) ? 0 : LVIS_SELECTED, LVIS_SELECTED);
}
void CMainListCtrl::ClearSelection()
{
	for (int i = 0; i < GetItemCount(); i++)
		SetItemState(i, 0, LVIS_SELECTED);
}



BOOL CMainListCtrl::PreTranslateMessage(MSG* pMsg)
{
	//m_tooltip.RelayEvent(pMsg);
	switch  ( pMsg->message)
	{
	case WM_CHAR:
		{
			TCHAR chr = static_cast<TCHAR>(pMsg->wParam);
			switch ( chr )
			{

			case 0x01: // 0x01 is the key code for ctrl-a and also for ctrl-A
				OnSelectAll();
				break;
			case 0x09: // 0x09 is the key code for ctrl-i and ctrl-I
				OnInvertSelection();
				break;
			}
		}
		break;
	case WM_KEYDOWN:
		{
			BOOL bHandled = TRUE;
			switch (pMsg->wParam)
			{
			case VK_RETURN:
				if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
					OnCommand(ACMD_Enqueue, 0);
				else if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
					OnCommand(ACMD_EnqueueAfter, 0);
				else
					OnCommand(ACMD_Play, 0);
				break;
			case VK_HOME:
				OnCommand(ACMD_Properties, 0);
				break;
			default:
				bHandled = FALSE;
			}
			if (bHandled)
				return TRUE;
		}
		break;
	//case WM_MOUSEWHEEL:
	//	if (LOWORD(pMsg->wParam) == MK_RBUTTON)
	//	{
	//		return TRUE;
	//	}
	//	break;
	//case WM_KEYDOWN:
	//	{
	//		switch (pMsg->wParam)
	//		{
	//		case 'L':
	//			{
	//				POSITION pos = GetFirstSelectedItemPosition();
	//				if (pos != NULL)
	//				{
	//					PrgAPI* pAPI = PRGAPI();
	//					FullTrackRecordSP& rec = m_tracks[GetNextSelectedItem(pos)];
	//					SQLManager* pSM = pAPI->GetSQLManager();
	//					InfoRecord ir;
	//					if (pSM->GetTrackInfoRecord(rec->track.ID, IIT_TrackLyrics, ir))
	//					{
	//						PRGAPI()->GetTSOnlineServices()->SendLyrics(
	//							rec->artist.name.c_str(),
	//							rec->track.name.c_str(),
	//							ir.name.c_str());
	//					}
	//				}
	//			}
	//			break;
	//		}
	//	}
	//	break;
	}
	return __super::PreTranslateMessage(pMsg);
}

void CMainListCtrl::OnLvnOdfinditem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVFINDITEM pFindInfo = reinterpret_cast<LPNMLVFINDITEM>(pNMHDR);

	/* pFindInfo->iStart is from which item we should search.
	We search to bottom, and then restart at top and will stop
	at pFindInfo->iStart, unless we find an item that match
	*/

	// Set the default return value to -1
	// That means we didn't find any match.
	*pResult = -1;

	//Is search NOT based on string?
	if( (pFindInfo->lvfi.flags & LVFI_STRING) == 0 )
	{
		//This will probably never happen...
		return;
	}

	//This is the string we search for
	UINT searchstrLen = _tcslen(pFindInfo->lvfi.psz);

	int startPos = pFindInfo->iStart;
	//Is startPos outside the list (happens if last item is selected)
	if(startPos >= GetItemCount())
		startPos = 0;

	int currentPos=startPos;
	//Let's search...
	do
	{
		if( _tcsnicmp(m_tracks[currentPos]->track.name.c_str(), pFindInfo->lvfi.psz, searchstrLen) == 0)
		{
			//Select this item and stop search.
			*pResult = currentPos;
			break;
		}

		//Go to next item
		currentPos++;

		//Need to restart at top?
		if(currentPos >= GetItemCount())
			currentPos = 0;

		//Stop if back to start
	}while(currentPos != startPos);        
}

void CMainListCtrl::NotifySelectedItemChanged()
{
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		UINT curID = m_tracks[GetNextSelectedItem(pos)]->track.ID;
		if (curID > 0)
		{
			PrgAPI* pAPI = PRGAPI();
			StateManager* pSM = pAPI->GetStateManager();
			TSState& state = pSM->GetState();
			if (state.trackListTrackID != curID)
			{
				state.trackListTrackID = curID;
				pAPI->SendMessage(SM_CurrentTrackChanged);
			}
		}
	}
}

LPCTSTR CMainListCtrl::FixName(LPCTSTR originalName)
{
	if (_tcscmp(originalName, TS_UnknownString) == 0)
		return PRGAPI()->GetString(IDS_UNKNOWN);
	return originalName;
}



void CMainListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	NotifySelectedItemChanged();
	*pResult = 0;
}

//---------------------------------------------------
//ICtrlColors ---------------------------------------
//---------------------------------------------------
void CMainListCtrl::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		m_colors[idx] = value;
	if (idx == COL_Bk)
		SetBkColor(value);
	if (m_hWnd)		
		Invalidate();

}
COLORREF CMainListCtrl::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}
//const TCHAR* const cColorNames[] = {
//	_T("Bk"),
//	_T("Text"),
//	_T("TextBk"),
//	_T("TextAlt"),
//	_T("TextAltBk"),
//	_T("TextSel"),
//	_T("TextSelBk"),
//	NULL
//};
//
//LPCTSTR CMainListCtrl::GetColorName(UINT idx)
//{
//	ASSERT(idx <= COL_Last);
//	if (idx < COL_Last)
//		return cColorNames[idx];
//	return NULL;
//}
//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------
BOOL CMainListCtrl::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	INT count = GetItemCount();
	if (count > 0)
	{
		CRect itemRc;
		GetItemRect(count - 1, &itemRc, LVIR_BOUNDS);
		rc.top = itemRc.bottom;
		if (itemRc.right < rc.right)
		{
			itemRc.SetRect(itemRc.right, 0, rc.right, rc.bottom);
			pDC->FillSolidRect(&itemRc, m_colors[COL_Bk]);
		}
	}
	if (rc.top < rc.bottom)
		pDC->FillSolidRect(&rc, m_colors[COL_Bk]);
	return TRUE;//__super::OnEraseBkgnd(pDC);
}

void CMainListCtrl::AppendInfoProviderMenuItem(BCMenu& mnu, const IInfoProvider& IP, InfoItemTypeEnum iit)
{
	PrgAPI* pAPI = PRGAPI();
	m_ProviderRequests.push_back(ProviderRequest(IP.GetModuleInfo(IInfoProvider::IPI_UniqueID), iit));
	TCHAR bf[500];
	_sntprintf(bf, 500, _T("\"%s\" - %s"), IP.GetModuleInfo(IInfoProvider::IPI_Name), pAPI->GetStringForInfoItemType(iit));
	mnu.AppendMenu(MF_STRING, ID_PROVIDERS + m_ProviderRequests.size() - 1, bf
		,pAPI->GetIconForInfoItemType(iit));
}

RecordSortOptionsEnum CMainListCtrl::SubItem2RecordSortOption(INT subItem)
{
	CMainListCtrl::ColumnsEnum col = SubItem2Column(subItem);
	return ColumnItem2RecordSortOption[col];
}

INT CMainListCtrl::RecordSortOptions2SubItem(RecordSortOptionsEnum rso)
{
	ASSERT(rso >= RSO_None && rso < RSO_Last);
	INT hdCount = GetHeaderCtrl()->GetItemCount();
	ASSERT(hdCount > 0);
	for (int i = 0; i < hdCount; i++)
	{
		if (SubItem2RecordSortOption(i) == rso)
			return i;
	}
	TRACE(_T("@1 CMainListCtrl::Column2SubItem. SubItem not found"));
	return -1;//Error
}


void CMainListCtrl::OnHdnDividerdblclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	SetColumnWidth(phdr->iItem, defWidth[SubItem2Column(phdr->iItem)]);
	*pResult = 0;
}


CMainListCtrl::SColumnInfo* CMainListCtrl::GetColumnInfo()
{
	UpdateColumnInfo();
	return m_columns;
}

void CMainListCtrl::UpdateColumns()
{
	LVCOLUMN col;
	memset(&col, 0, sizeof(LVCOLUMN));
	col.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH | LVCF_ORDER;
	col.fmt = LVCFMT_LEFT;

	HDITEM hd;
	memset(&hd, 0, sizeof(HDITEM));
	hd.mask = HDI_LPARAM;			//Header will Take as lparam the ColumnEnum


	//int colIndex = 0;
	PrgAPI* pAPI = PRGAPI();
	CHeaderCtrl* pHeader = GetHeaderCtrl();
	if (pHeader == NULL)
		return;
	for (INT i = pHeader->GetItemCount() - 1; i >= 0; i--)
	{
		DeleteColumn(i);
		pHeader->DeleteItem(i);
	}





	//for (INT i = 0; i < CL_Last - CL_First; i++)
	//{
	//	for (INT j = CL_First; j < CL_Last; j++)
	//	{
	//		if (m_columns[j].iOrder == i)
	//		{
	//			if (m_columns[j].bVisible)
	//			{
	//				col.cx = m_columns[j].cx;
	//				col.pszText = (LPTSTR) pAPI->GetString(defRes[j]);
	//				col.iOrder = m_columns[j].iOrder;
	//				INT idx = InsertColumn(m_columns[j].iOrder, &col);
	//				hd.lParam = j;
	//				pHeader->SetItem(idx, &hd);
	//				break;
	//			}
	//		}
	//	}
	//}


	BOOL bChecked[CL_Last];
	memset(bChecked, 0, sizeof(bChecked));
	INT newOrder[CL_Last];
	INT newOrderPos = 0;

	while (TRUE)
	{
		INT iSmallerValue = 1001;
		INT iSmallerValueColumn = 0;
		for (INT i = 0; i < CL_Last - CL_First; i++)
		{
			if (bChecked[i] == FALSE)
			{
				if (m_columns[i].iOrder > 1000)
					m_columns[i].iOrder = 1000;

				if (m_columns[i].iOrder < iSmallerValue)
				{
					iSmallerValue = m_columns[i].iOrder;
					iSmallerValueColumn = i;
				}
			}
		}
		if (iSmallerValue == 1001)
		{
			//=== All Found
			ASSERT(newOrderPos == CL_Last);
			break;
		}
		bChecked[iSmallerValueColumn] = TRUE;
		newOrder[newOrderPos] = iSmallerValueColumn;
		newOrderPos++;

	}


	for (INT i = 0; i < CL_Last - CL_First; i++)
	{
		INT j = newOrder[i];
		m_columns[j].iOrder = i;
		if (m_columns[j].bVisible)
		{
			col.cx = m_columns[j].cx > 1 ? m_columns[j].cx : defWidth[i];
			col.pszText = (LPTSTR) pAPI->GetString(defRes[j]);
			col.iOrder = m_columns[j].iOrder;
			INT idx = InsertColumn(m_columns[j].iOrder, &col);
			hd.lParam = j;
			pHeader->SetItem(idx, &hd);
		}
	}



}
