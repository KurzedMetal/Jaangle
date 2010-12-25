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
#include "EnhTrackListWnd.h"
#include "PrgAPI.h"
#include "ShortcutManager.h"
#include "SQLManager.h"
#include "ActionManager.h"
#include "StateManager.h"
#include "ShortcutManager.h"


#define IDC_SEARCHEDIT 200
#define SEARCH_DELAY_TIMER			504
#define SEARCH_DELAY_TIMER_DELAY	500


const int cEditWidth = 180;

const int cXMargin = 4;

IMPLEMENT_DYNAMIC(CEnhTrackListWnd, CWnd)
CEnhTrackListWnd::CEnhTrackListWnd():
m_TitleHeight(30),
m_locateTrackID(0)
{
	m_colors[COL_Bk] = RGB(140,140,230);
	m_colors[COL_Text] = RGB(0,0,0);
}

CEnhTrackListWnd::~CEnhTrackListWnd()
{



}


BEGIN_MESSAGE_MAP(CEnhTrackListWnd, CWnd)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_SEARCHEDIT, &CEnhTrackListWnd::OnEnChangeSearchedt)
	ON_WM_TIMER()

	ON_WM_DESTROY()
END_MESSAGE_MAP()


void CEnhTrackListWnd::OnSize(UINT nType, int cx, int cy)
{
	if (m_hWnd != 0 && cx > 0 && cy > 0)
	{
		Resize(cx, cy);
	}
}

void CEnhTrackListWnd::Resize(int cx, int cy)
{
	TSSkin& skin = PRGAPI()->GetSkinManager()->GetSkin();
	INT editHeight = skin.GetIntParameter(CSEC_TrackListTitle, IPARAM_EditHeight) + skin.GetRelativeFontSize();


	m_searchEdit.SetWindowPos(NULL, cx - cEditWidth - cXMargin, cXMargin, cEditWidth, editHeight, SWP_NOACTIVATE  | SWP_NOZORDER /*| SWP_NOREDRAW*/);
	m_caption.SetWindowPos(NULL, cXMargin, cXMargin, cx - 2 * cXMargin - cEditWidth, m_TitleHeight - 2 * cXMargin, SWP_NOACTIVATE | SWP_NOZORDER /*| SWP_NOREDRAW*/);
	m_trackList.SetWindowPos(NULL, 0, m_TitleHeight, cx, cy - m_TitleHeight, SWP_NOACTIVATE | SWP_NOREPOSITION | SWP_NOZORDER /*| SWP_NOREDRAW*/);
}

void CEnhTrackListWnd::ApplyTranslation(ITranslation& translation)
{
	CRect rc;
	GetClientRect(&rc);
	Resize(rc.Width(), rc.Height());
	PrgAPI* pAPI = PRGAPI();
	TCHAR txt[300];
	_sntprintf(txt, 300, _T("%s"), pAPI->GetString(IDS_SEARCH));
	m_searchEdit.SetEmptyMessage(txt);
	m_caption.Invalidate();
}

void CEnhTrackListWnd::ApplySkin(ISkin& _skin)
{
	TSSkin& skin = (TSSkin&) _skin;
	m_searchEdit.SetFont(skin.GetFont(CSEC_TrackListTitle, FSTY_Edit));
	m_caption.SetFont(skin.GetFont(CSEC_TrackListTitle, FSTY_Normal));
	m_caption.SetSpecialFont(skin.GetFont(CSEC_TrackListTitle, FSTY_NormalBold));
	m_caption.Invalidate();

	SetColor(COL_Bk, skin.GetColor(CSEC_TrackListTitle, CVAL_Bk));
	SetColor(COL_Text, skin.GetColor(CSEC_TrackListTitle, CVAL_Text));

	SetTitleHeight(skin.GetIntParameter(CSEC_TrackListTitle, IPARAM_Height) + skin.GetRelativeFontSize());
	m_trackList.ApplySkin(skin);
}


void CEnhTrackListWnd::OnEnChangeSearchedt()
{
	KillTimer(SEARCH_DELAY_TIMER);
	SetTimer(SEARCH_DELAY_TIMER, SEARCH_DELAY_TIMER_DELAY, NULL);

}

BOOL CEnhTrackListWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	rc.bottom = m_TitleHeight;
	pDC->FillSolidRect(&rc, m_colors[COL_Bk]);
	return TRUE;
}

void CEnhTrackListWnd::SetTitleHeight(INT height)
{
	m_TitleHeight = height;
	if (m_hWnd)
	{
		CRect rc;
		GetClientRect(&rc);
		Resize(rc.Width(), rc.Height());
	}
}
void CEnhTrackListWnd::SetText(LPCTSTR txt)
{
	ASSERT(txt != NULL);
	ASSERT(m_caption.m_hWnd);
	if (m_caption.m_hWnd)
		m_caption.SetWindowText(txt);
}


void CEnhTrackListWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	//CRect itemRC;
	//GetClientRect(&itemRC);
	//itemRC.bottom = m_TitleHeight - 1;
	//itemRC.left += 3;
	//dc.SetBkMode(TRANSPARENT);
	//PrgAPI* pAPI = PRGAPI();

	//if (m_Title.IsEmpty())
	//{
	//	CFont* oldFont = dc.SelectObject(pAPI->GetFont(FONT_Main));
	//	dc.SetTextColor(m_colors[COL_Text]);// RGB(200,150,0));
	//	dc.DrawText(m_SubTitle, &itemRC,
	//		DT_END_ELLIPSIS | DT_NOPREFIX | DT_VCENTER | DT_SINGLELINE | DT_EXPANDTABS);
	//	dc.SelectObject(oldFont);

	//}
	//else
	//{
	//	CRect calcRc;
	//	//---Draw The ItemType
	//	CFont* oldFont = dc.SelectObject(pAPI->GetFont(FONT_Main));
	//	dc.SetTextColor(m_colors[COL_Text]);// RGB(200,150,0));
	//	TCHAR bf[100];
	//	INT curLen = _sntprintf(bf, 100, _T("%s: "), m_Header);
	//	dc.DrawText(bf, curLen, &calcRc,
	//		DT_NOPREFIX | DT_SINGLELINE | DT_EXPANDTABS | DT_CALCRECT);
	//	dc.DrawText(bf, curLen, &itemRC,
	//		DT_NOPREFIX | DT_SINGLELINE | DT_EXPANDTABS | DT_VCENTER );
	//	itemRC.left += calcRc.Width();

	//	//---Draw the main Title
	//	dc.SelectObject(pAPI->GetFont(FONT_MainBold));
	//	//itemRC.OffsetRect(1,1);
	//	dc.DrawText(m_Title, &calcRc,
	//		DT_NOPREFIX | DT_SINGLELINE | DT_EXPANDTABS | DT_CALCRECT);
	//	//dc.SetTextColor(m_colors[COL_TextTitleSh]);// RGB(200,150,0));
	//	//dc.DrawText(m_Title, &itemRC,
	//	//	DT_NOPREFIX | DT_EXPANDTABS | DT_SINGLELINE | DT_VCENTER );
	//	//itemRC.OffsetRect(-1,-1);
	//	dc.SetTextColor(m_colors[COL_TextTitle]);// RGB(200,150,0));
	//	dc.DrawText(m_Title, &itemRC,
	//		DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE | DT_VCENTER );
	//	itemRC.left += calcRc.Width() + 20;

	//	//---Draw The Rest Info

	//	//Subtitle (Upper)
	//	//itemRC.left += 0;
	//	dc.SelectObject(pAPI->GetFont(FONT_Main));
	//	dc.SetTextColor(m_colors[COL_Text]);//RGB(0,0,0));
	//	dc.DrawText(m_SubTitle, &itemRC,
	//		DT_END_ELLIPSIS | DT_NOPREFIX | DT_SINGLELINE | DT_EXPANDTABS | DT_VCENTER );
	//	dc.SelectObject(oldFont);

	//}
}

void CEnhTrackListWnd::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case SEARCH_DELAY_TIMER:
		{
			KillTimer(nIDEvent);
			PrgAPI* pAPI = PRGAPI();
			TCHAR searchString[300];
			m_searchEdit.GetWindowText(searchString, 300);
			const INT cCharLimitForLimitedResults = 1;
			const INT cLimitedResultsCount = 100;
			const INT cCharLimitForFullResults = 3;
			const INT cTolalResultsCount = 0;

			//TSState& state = pAPI->GetStateManager()->GetState();
			INT charsLen = _tcslen(searchString);
			if (charsLen >= cCharLimitForLimitedResults)
			{
				//state.quickSearchString = searchString;
				col.clear();
				if (!pAPI->GetSQLManager()->GetFullTrackRecordCollectionBySmartSearch(col, searchString, 
					charsLen >= cCharLimitForFullResults ? cTolalResultsCount : cLimitedResultsCount))
				{
					col.clear();
				}
				m_trackList.SetFullTrackRecordCollection(col);
				m_trackList.Update();
				//state.sectionSelectionStyle = TSState::SSS_QuickSearch;
				

				TCHAR txt[500];
				UINT c = col.size();
				_sntprintf(txt, 500, _T("%s: <s>'%s'</s> | %s: <s>%d</s>"),
					pAPI->GetString(IDS_SEARCH),
					searchString,
					pAPI->GetString(IDS_TRACKS),
					c);
				SetText(txt);

			}
			else
			{
				OnStateChanged(SM_CurrentSectionChanged);
			}

		}
		break;
	default:
		CWnd::OnTimer(nIDEvent);
		break;
	}
}


//---------------------------------------------------
//ICtrlColors ---------------------------------------
//---------------------------------------------------
void CEnhTrackListWnd::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		m_colors[idx] = value;
	if (idx == COL_Bk)
	{
		m_caption.SetColor(CLabelEx::COL_Bk, value);
		//m_play.SetColor(FlexButton::COL_Bk, value);
		//m_enqueue.SetColor(FlexButton::COL_Bk, value);
	}
	else if (idx == COL_Text)
	{
		m_caption.SetColor(CLabelEx::COL_Text, value);
		//m_play.SetColor(FlexButton::COL_Text, value);
		//m_enqueue.SetColor(FlexButton::COL_Text, value);
	}
}
COLORREF CEnhTrackListWnd::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}

BOOL CEnhTrackListWnd::Create(CWnd* pParent, UINT nID)
{
	if (CWnd::Create(_T("STATIC"), _T(""), WS_CHILD, CRect(0,0,0,0), pParent, nID, 0))
	{
		PrgAPI* pAPI = PRGAPI();
		CRect rc(0,0,0,0);
		m_searchEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP /*| WS_BORDER*/, rc, this, IDC_SEARCHEDIT);
		m_searchEdit.SetIcon(pAPI->GetIcon(ICO_Find16));
		m_searchEdit.SetClearIcon(pAPI->GetIcon(ICO_Clear16));

		m_caption.Create(_T("STATIC"), NULL, WS_CHILD | WS_VISIBLE, rc, this, IDC_INFO, 0);

		//=== Create the EnhTrackList Control & the TrackList
		m_trackList.Create(WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_OWNERDATA | LVS_SHOWSELALWAYS , CRect(0,0,0,0), this, 110);

		//=== Register the shortcut
		ShortcutManager* pSCM = pAPI->GetShortcutManager();
		pSCM->RegisterShortcutForFocus(VK_F2, m_trackList.m_hWnd);
		pSCM->RegisterShortcutForFocus(VK_F3, m_searchEdit.m_hWnd);

		//=== Register the Track List as Skinnable
		SkinManager* pSM = pAPI->GetSkinManager();
		pSM->RegisterSkinnable(m_trackList);

		ApplyTranslation(*pAPI->GetTranslationManager());
		ApplySkin(pAPI->GetSkinManager()->GetSkin());

		return TRUE;
	}
	return FALSE;

}


void CEnhTrackListWnd::OnDestroy()
{
	PrgAPI* pAPI = PRGAPI();
	//=== UnRegister the Track List as Skinnable
	pAPI->GetSkinManager()->UnRegisterSkinnable(m_trackList);
	//=== Register the shortcut
	if (m_trackList.m_hWnd)
		pAPI->GetShortcutManager()->UnRegisterShortcutForFocus(m_trackList.m_hWnd);



	__super::OnDestroy();

	// TODO: Add your message handler code here
}

BOOL CEnhTrackListWnd::OnStateChanged(UINT stateMessage)
{
	if (m_trackList.m_hWnd == NULL)
		return FALSE;
	BOOL bHandled = TRUE;
	switch (stateMessage)
	{
	case SM_TrackUpdated:
		m_trackList.Invalidate();
		break;
	case SM_DatabaseUpdated:
	case SM_CollectionManagerEvent:
		m_trackList.SetFullTrackRecordCollection(PRGAPI()->GetStateManager()->GetState().GetTrackList());
		m_trackList.Update();
		break;
	case SM_InfoChanged:
		m_trackList.InvalidateInfoIcons();
		break;
	case SM_SetQuickSearch:
		{
			TSState& state = PRGAPI()->GetStateManager()->GetState();
			m_searchEdit.SetWindowText(state.quickSearchString.c_str());
			OnEnChangeSearchedt();
		}
		break;
	case SM_MediaChanged:
		{
			PrgAPI* pAPI = PRGAPI();
			MediaPlayer* pMP = pAPI->GetMediaPlayer();
			MediaPlayListItem mpli;
			if (pMP->GetPlayListItem(mpli, pMP->GetPlayListPos()))
			{
				FullTrackRecordSP sp;
				if (pAPI->GetSQLManager()->GetFullTrackRecordByLocation(sp, mpli.url))
				{
					TracksFilter tf;
					tf.AlbumID.val = sp->album.ID;
					tf.AlbumID.match = NUMM_Exact;
					col.clear();
					pAPI->GetSQLManager()->GetFullTrackRecordCollectionByTracksFilter(col, tf);
					//===Translate [Unknown]
					TCHAR bf[500];
					if (sp->artist.name == TS_UnknownString)
						sp->artist.name = pAPI->GetString(IDS_UNKNOWN);
					if (sp->album.name == TS_UnknownString)
						sp->album.name = pAPI->GetString(IDS_UNKNOWN);
					_sntprintf(bf, 500, _T("%s: <s>%s</s> | %s: <s>%s</s> | %s: <s>%d</s>"),
						pAPI->GetStringForItemType(IT_Album),
						sp->album.name.c_str(),
						pAPI->GetStringForItemType(IT_Artist),
						sp->artist.name.c_str(),
						pAPI->GetString(IDS_TRACKS),
						col.size());

					SetText(bf);
					m_trackList.SetFullTrackRecordCollection(col);
					m_trackList.Update();
					if (m_locateTrackID != 0)
					{
						m_trackList.SelectItemByID(m_locateTrackID);
						m_locateTrackID = 0;
					}
				}
			}
		}
		break;
	case SM_CurrentSectionChanged:
		{
			TSState& state = PRGAPI()->GetStateManager()->GetState();
			m_trackList.SetFullTrackRecordCollection(state.GetTrackList());
			m_trackList.Update();
			UINT tracks = m_trackList.GetItemCount();

			//===Change the Track View Header
			TCHAR bf[500];
			bf[0] = 0;
			BOOL bIsVarious = FALSE;
			PrgAPI* pAPI = PRGAPI();
			if (!bIsVarious)
			{
				SQLManager* pSM = pAPI->GetSQLManager();
				switch (state.activeItemType)
				{
				case IT_Artist:
					{
						FullArtistRecordSP sp;
						if (pSM->GetFullArtistRecordByID(sp, state.activeTracksFilter.ArtistID.val))
						{
							//===Translate [Unknown]
							if (sp->artist.name == TS_UnknownString)
								sp->artist.name = pAPI->GetString(IDS_UNKNOWN);
							if (sp->genre.name == TS_UnknownString)
								sp->genre.name = pAPI->GetString(IDS_UNKNOWN);

							_sntprintf(bf, 500, _T("%s: <s>%s</s> | %s: <s>%s</s> | %s: <s>%d</s>"),
								pAPI->GetStringForItemType(IT_Artist),
								sp->artist.name.c_str(),
								pAPI->GetStringForItemType(IT_Genre),
								sp->genre.name.c_str(),
								pAPI->GetString(IDS_TRACKS),
								tracks);
						}
					}
					break;
				case IT_Album:
					{
						FullAlbumRecordSP sp;
						if (state.activeTracksFilter.AlbumID.val > 0)
						{
							if (pSM->GetFullAlbumRecordByID(sp, state.activeTracksFilter.AlbumID.val))
							{
								//===Translate [Unknown]
								if (sp->artist.name == TS_UnknownString)
									sp->artist.name = pAPI->GetString(IDS_UNKNOWN);
								if (sp->album.name == TS_UnknownString)
									sp->album.name = pAPI->GetString(IDS_UNKNOWN);

								bIsVarious = FALSE;
								_sntprintf(bf, 500, _T("%s: <s>%s</s> | %s: <s>%s</s> | %s: <s>%d</s>"),
									pAPI->GetStringForItemType(IT_Album),
									sp->album.name.c_str(),
									pAPI->GetStringForItemType(IT_Artist),
									sp->artist.name.c_str(),
									pAPI->GetString(IDS_TRACKS),
									tracks);
							}
						}
						else if (state.activeTracksFilter.Album.match != TXTM_Disabled)
						{
							bIsVarious = FALSE;
							_sntprintf(bf, 500, _T("%s: <s>%s</s> | %s: <s>%s</s>"),
								pAPI->GetStringForItemType(IT_Album),
								state.activeTracksFilter.Album.val.c_str(),
								pAPI->GetString(IDS_ARTIST),
								pAPI->GetString(IDS_VARIOUS));
						}
						else
							ASSERT(0);
					}
					break;
				case IT_Genre:
					{
						GenreRecord sp;
						if (pSM->GetGenreRecord(state.activeTracksFilter.GenreID.val, sp))
						{
							//===Translate [Unknown]
							if (sp.name == TS_UnknownString)
								sp.name = pAPI->GetString(IDS_UNKNOWN);
							_sntprintf(bf, 500, _T("%s: <s>%s</s> | %s: <s>%d</s>"),
								pAPI->GetStringForItemType(IT_Genre),
								sp.name.c_str(),
								pAPI->GetString(IDS_TRACKS),
								tracks);
						}
					}
					break;
				case IT_Collection:
					{
						CollectionRecord sp;
						if (pSM->GetCollectionRecord(state.activeTracksFilter.CollectionID.val, sp))
						{
							_sntprintf(bf, 500, _T("%s: <s>%s</s> | %s: <s>%d</s>"),
								pAPI->GetStringForItemType(IT_Genre),
								sp.name.c_str(),
								pAPI->GetString(IDS_TRACKS),
								tracks);
						}
					}
					break;
				case IT_Year:
					{
						_sntprintf(bf, 500, _T("%s: <s>%d</s> | %s: <s>%d</s>"),
							pAPI->GetStringForItemType(IT_Year),
							state.activeTracksFilter.Year.val,
							pAPI->GetString(IDS_TRACKS),
							tracks);
					}
					break;
				}
			}
			else
			{
				_sntprintf(bf, 500, _T("%s | %s: <s>%d</s>"),
					pAPI->GetStringForItemType(IT_Various),
					pAPI->GetString(IDS_TRACKS),
					tracks);
			}


			SetText(bf);
			if (m_locateTrackID != 0)
			{
				m_trackList.SelectItemByID(m_locateTrackID);
				m_locateTrackID = 0;
			}
		}
		break;
	case SM_LocateTrackRequest:
		{
			m_locateTrackID = PRGAPI()->GetStateManager()->GetState().locateTrackID;
			if (m_trackList.SelectItemByID(m_locateTrackID))
				m_locateTrackID = 0;
		}
		break;
	//case SM_TrackChanged:
	//	m_trackList.SetFullTrackRecordCollection(PRGAPI()->GetStateManager()->GetState().GetTrackList());
	//	m_trackList.Update();
	//	break;
	default:
		bHandled = FALSE;
	}

	return bHandled;
}


void CEnhTrackListWnd::PlayTracks()
{
	PrgAPI* pAPI = PRGAPI();
	ActionManager* pAM = pAPI->GetActionManager();
	MediaPlayer* pMP = pAPI->GetMediaPlayer();
	pAM->Play(pMP, m_trackList.GetFullTrackRecordCollection());
}

void CEnhTrackListWnd::EnqueueTracks()
{
	PrgAPI* pAPI = PRGAPI();
	ActionManager* pAM = pAPI->GetActionManager();
	MediaPlayer* pMP = pAPI->GetMediaPlayer();
	pAM->Enqueue(pMP, m_trackList.GetFullTrackRecordCollection());
}
