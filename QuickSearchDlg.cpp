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
#include "QuickSearchDlg.h"
#include "PrgAPI.h"
#include "TeenSpirit.h"
#include "SQLManager.h"


#define IDC_SEARCHEDIT 200
#define SEARCH_DELAY_TIMER			504
#define SEARCH_DELAY_TIMER_DELAY	500

#define TIMER_AUTOHIDE 505


const int cEditWidth = 180;

const int cXMargin = 1;

QuickSearchDlg::QuickSearchDlg():
m_titleHeight(20),
m_bCloseOnAction(TRUE)
{
	m_colors[COL_Bk] = RGB(140,140,230);
	m_colors[COL_Text] = RGB(0,0,0);
}

QuickSearchDlg::~QuickSearchDlg()
{

}


BEGIN_MESSAGE_MAP(QuickSearchDlg, CWnd)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_SEARCHEDIT, &QuickSearchDlg::OnEnChangeSearchedt)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL QuickSearchDlg::Create(CWnd* pParent)
{
	PrgAPI* pAPI = PRGAPI();
	CRect rc(100,100,200,200);
	HWND hParent = pParent ? pParent->GetSafeHwnd() : NULL;

	if (CreateEx(WS_EX_OVERLAPPEDWINDOW, 
		_T("#32770"), 
		pAPI->GetString(IDS_SEARCH), 
		WS_POPUP | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, 
		0, 0, 450, 220, 
		hParent, 0, 0))
	{
		m_closeButton.Create(0, WS_VISIBLE | WS_CHILD, rc, this, IDCANCEL);	

		m_closeButton.ApplyPredifinedButtonsStyle(FlexButton::PBS_HotButton);
		m_closeButton.SetComponents(FlexButton::COMP_Image);
		m_closeButton.SetIcon(pAPI->GetIcon(ICO_Close16), 16, 16);

		m_searchEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP /*| WS_BORDER*/, rc, this, IDC_SEARCHEDIT);
		m_searchEdit.SetIcon(pAPI->GetIcon(ICO_Find16));
		m_searchEdit.SetClearIcon(pAPI->GetIcon(ICO_Clear16));

		m_trackList.Create(WS_VISIBLE | WS_CHILD, rc, this, 110);
		m_trackList.SetMediaPlayer(pAPI->GetMediaPlayer());
		m_trackList.SetSingleSelection(TRUE);

		ApplyTranslation(*pAPI->GetTranslationManager());
		ApplySkin(pAPI->GetSkinManager()->GetSkin());

		if (m_initialSearchText.size() > 0)
		{
			m_searchEdit.SetWindowText(m_initialSearchText.c_str());
			m_searchEdit.SetSel(200, 200);
			OnEnChangeSearchedt();
		}

		m_trackList.SetFullTrackRecordCollection(&m_tracks);
		SetTimer(TIMER_AUTOHIDE, 3500, 0);
		m_searchEdit.SetFocus();

		return TRUE;
	}
	else
	{
		DWORD dwLastError = GetLastError();
		TRACE(_T("QuickSearchDlg::Create Failed\r\n"));
	}
	return FALSE;
}

void QuickSearchDlg::SetSearchText(LPCTSTR initialSearchText)	
{
	m_initialSearchText = initialSearchText;
	if (m_searchEdit.GetSafeHwnd())
	{
		m_searchEdit.SetWindowText(m_initialSearchText.c_str());
		m_searchEdit.SetSel(200, 200);
		m_searchEdit.SetFocus();
		OnEnChangeSearchedt();
	}
}

void QuickSearchDlg::OnSize(UINT nType, int cx, int cy)
{
	if (m_searchEdit.GetSafeHwnd() != 0 && cx > 0 && cy > 0)
	{
		Resize(cx, cy);
	}
}

void QuickSearchDlg::Resize(int cx, int cy)
{
	TSSkin& skin = PRGAPI()->GetSkinManager()->GetSkin();
	INT controlHeight = m_titleHeight - 2 * cXMargin;
	m_closeButton.SetWindowPos(NULL, cx - controlHeight - cXMargin, cXMargin, controlHeight, controlHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	m_searchEdit.SetWindowPos(NULL, cXMargin, cXMargin, cx - 3 * cXMargin - controlHeight, controlHeight, SWP_NOACTIVATE  | SWP_NOZORDER /*| SWP_NOREDRAW*/);
	m_trackList.SetWindowPos(NULL, cXMargin, m_titleHeight, cx - 2 * cXMargin, cy - m_titleHeight - cXMargin, SWP_NOACTIVATE | SWP_NOZORDER /*| SWP_NOREDRAW*/);
}

//INT QuickSearchDlg::GetDesiredHeight()
//{
//	return m_titleHeight + m_records * m_trackList.GetItemHeight() + 1;
//}


void QuickSearchDlg::ApplyTranslation(ITranslation& translation)
{
	CRect rc;
	GetClientRect(&rc);
	Resize(rc.Width(), rc.Height());
	PrgAPI* pAPI = PRGAPI();
	TCHAR txt[300];
	_sntprintf(txt, 300, _T("%s"), pAPI->GetString(IDS_SEARCH));
	m_searchEdit.SetEmptyMessage(txt);
}

void QuickSearchDlg::ApplySkin(ISkin& _skin)
{
	TSSkin& skin = (TSSkin&) _skin;


	m_searchEdit.SetFont(skin.GetFont(CSEC_MiniPlayer, FSTY_Normal));
	m_trackList.SetFont(skin.GetFont(CSEC_MiniPlayer, FSTY_Normal));

	SetColor(COL_Bk, skin.GetColor(CSEC_PlayList, CVAL_Bk));
	SetColor(COL_Text, skin.GetColor(CSEC_PlayList, CVAL_Text));

	SetTitleHeight(24 + skin.GetRelativeFontSize());
	m_trackList.ApplySkin(skin);

	//INT editHeight = skin.GetIntParameter(CSEC_TrackListTitle, IPARAM_EditHeight) + skin.GetRelativeFontSize();

}


void QuickSearchDlg::OnEnChangeSearchedt()
{
	KillTimer(SEARCH_DELAY_TIMER);
	SetTimer(SEARCH_DELAY_TIMER, SEARCH_DELAY_TIMER_DELAY, NULL);
}

BOOL QuickSearchDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(&rc);
	//rc.bottom = m_titleHeight;
	pDC->FillSolidRect(&rc, m_colors[COL_Bk]);
	return TRUE;
}

void QuickSearchDlg::SetTitleHeight(INT height)
{
	m_titleHeight = height;
	if (m_hWnd)
	{
		CRect rc;
		GetClientRect(&rc);
		Resize(rc.Width(), rc.Height());
	}
}


void QuickSearchDlg::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case SEARCH_DELAY_TIMER:
		{
			KillTimer(SEARCH_DELAY_TIMER);
			PrgAPI* pAPI = PRGAPI();
			TCHAR searchString[300];
			m_searchEdit.GetWindowText(searchString, 300);
			const INT cMinChars = 0;

			if (_tcslen(searchString) >= cMinChars)
			{
				m_tracks.clear();
				pAPI->GetSQLManager()->GetFullTrackRecordCollectionBySmartSearch(m_tracks, searchString, _tcslen(searchString) >= 3 ? 0 : 30);
				SortOptionCollection scol;
				scol.ApplySortOption(SortOption(RSO_Rating, FALSE));
				scol.ApplySortOption(SortOption(RSO_Title, TRUE));
				RecordCollectionSorter::SortFullTrackRecordCollection(m_tracks, scol);
				m_trackList.Update();
				if (m_tracks.size() > 0)
				{
					m_trackList.SetItemSelection(0, TRUE);
					m_trackList.EnsureVisible(0, TRUE);
				}
			}

		}
		break;
	case TIMER_AUTOHIDE:
		if (IsWindowVisible())
		{
			if (::GetForegroundWindow() != m_hWnd)
				HideWindow();
			else
			{
				KillTimer(TIMER_AUTOHIDE);
				SetTimer(TIMER_AUTOHIDE, 500, 0);
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
void QuickSearchDlg::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		m_colors[idx] = value;
	if (idx == COL_Bk)
	{
		m_closeButton.SetColor(FlexButton::COL_Bk, value);
	}
	else if (idx == COL_Text)
	{
		//m_caption.SetColor(CLabelEx::COL_Text, value);
	}
}
COLORREF QuickSearchDlg::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}

void QuickSearchDlg::OnDestroy()
{
	PrgAPI* pAPI = PRGAPI();
	__super::OnDestroy();
}

void QuickSearchDlg::OnClose()
{
	__super::OnClose();
	HideWindow();
	//DestroyWindow();
}

void QuickSearchDlg::HideWindow()
{
	ShowWindow(SW_HIDE);
	m_tracks.clear();
	m_trackList.Update();
}

BOOL QuickSearchDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		BOOL bHandled = TRUE;
		switch (pMsg->wParam)
		{
		case VK_ESCAPE:
			HideWindow();
			break;
		case VK_RETURN:
			{
				MediaPlayer* pMP = PRGAPI()->GetMediaPlayer();
				if (pMP != NULL)
				{
					INT nItem = m_trackList.GetNextSelectedItem();
					if (nItem!= -1)
					{
						FullTrackRecordSP rec = m_tracks[nItem];
						MediaPlayListItem mpli;
						mpli.artist = rec->artist.name.c_str();
						mpli.title = rec->track.name.c_str();
						mpli.rating = rec->track.rating;
						mpli.length = rec->track.duration;
						mpli.url = rec->track.location.c_str();
						if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
							pMP->Enqueue(mpli);
						else if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
							pMP->InsertMedia(mpli, pMP->GetPlayListPos() + 1);
						else
							pMP->Play(mpli);
					}
					if (m_bCloseOnAction)
						HideWindow();
				}
			}
			break;
		case VK_UP:
			{
				INT nItem = m_trackList.GetNextSelectedItem();
				if (nItem > 0)
				{
					m_trackList.SetItemSelection(nItem - 1, TRUE);
					m_trackList.EnsureVisible(nItem - 1, TRUE);
				}
			}
			break;
		case VK_DOWN:
			{
				INT nItem = m_trackList.GetNextSelectedItem();
				if (nItem < m_trackList.GetItemCount() - 1)
				{
					m_trackList.SetItemSelection(nItem + 1, TRUE);
					m_trackList.EnsureVisible(nItem + 1, TRUE);
				}
			}

			break;
		default:
			bHandled = FALSE;
		}
		if (bHandled)
			return TRUE;
	}


	return __super::PreTranslateMessage(pMsg);
}

BOOL QuickSearchDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam == IDCANCEL)
	{
		HideWindow();
		return TRUE;
	}

	return __super::OnCommand(wParam, lParam);
}

void QuickSearchDlg::ShowRelative(CWnd* wndRelative, RelativePosition rp)
{
	ASSERT(!(wndRelative == NULL && rp == RP_Outside));
	if (GetSafeHwnd() == NULL)
		Create(NULL);
	if (wndRelative == NULL)
	{
		wndRelative = GetDesktopWindow();
		rp = RP_Centered;
	}
	CRect rc;
	GetWindowRect(&rc);
	CRect rcNew;
	CRect rcRelative;
	wndRelative->GetWindowRect(&rcRelative);
	if (rp == RP_Outside)
	{
		CRect rcDesktop;
		GetDesktopWindow()->GetWindowRect(&rcDesktop);
		INT spaceDownwards = rcDesktop.bottom - rcRelative.bottom;
		INT spaceUpwards = rcRelative.top - rcDesktop.top;
		if (spaceDownwards > rc.Height())
		{
			//== if there is space downwards then put it down
			rcNew.left = rcRelative.left;
			rcNew.top = rcRelative.bottom + 2;
			rcNew.right = rcNew.left + rc.Width();
			rcNew.bottom = rcNew.top + rc.Height();
		}
		else if (spaceUpwards > rc.Height())
		{
			//== else if there is space upwards then put it up
			rcNew.left = rcRelative.left;
			rcNew.top = rcRelative.top - 2 - rc.Height();
			rcNew.right = rcNew.left + rc.Width();
			rcNew.bottom = rcNew.top + rc.Height();
		}
		else
			rp = RP_Centered;

	}
	if (rp == RP_Centered)
	{
		rcNew.left = rcRelative.left + (rcRelative.Width() - rc.Width())  / 2;
		rcNew.top = rcRelative.top + (rcRelative.Height() - rc.Height())  / 2;
		rcNew.right = rcNew.left + rc.Width();
		rcNew.bottom = rcNew.top + rc.Height();
	}
	SetWindowPos(&wndTop, rcNew.left, rcNew.top, rcNew.Width(), rcNew.Height(), SWP_SHOWWINDOW);
	SetForegroundWindow();
	m_searchEdit.SetFocus();
}
