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
#include "TrackListCtrl.h"
#include "MediaPlayer.h"
#include "MenuManager.h"
#include "prgapi.h"
#include "SQLManager.h"
#include "StateManager.h"
#include "LocalPictureManager.h"


TrackListCtrl::TrackListCtrl():
m_pPlayer(NULL),
m_pBoldFont(NULL),
m_pNormalFont(NULL),
m_pCollection(NULL),
m_menuInitiator(NULL),
m_bDrawPictures(TRUE)
{
	m_colors[COL_Bk] = RGB(0,0,0);
	m_colors[COL_Text] = RGB(100,100,250);
	m_colors[COL_TextSel] = RGB(50,50,100);
	m_colors[COL_TextSelBk] =RGB(150,150,200);
	SetItemHeight(36);
	memset(&m_pMenu, 0, sizeof(m_pMenu));
//	m_configOptions[C_ShowNumbers] = 0;
}
TrackListCtrl::~TrackListCtrl()
{
	ResetAllMenus(TRUE);
}
void TrackListCtrl::ResetAllMenus(BOOL bDelete)
{
	for (int i = 0; i < M_Last; i++)
	{
		if (m_pMenu[i] != NULL)
		{
			m_pMenu[i]->Destroy();
			if (bDelete)
			{
				delete m_pMenu[i];
				m_pMenu[i] = NULL;
			}
		}
	}
}

void TrackListCtrl::Update()
{
	if (m_pCollection != NULL)
	{
		SetItemCount(m_pCollection->size());
		ClearSelection();
		Refresh();
	}
}


BEGIN_MESSAGE_MAP(TrackListCtrl, CFlexListCtrl)
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


void TrackListCtrl::OnItemMouseEvent(const ItemMouseEvent& ev)
{
	switch (ev.type)
	{
	case IMET_LButtonDblClick:
		//if (m_pPlayer != NULL)
		//{
		//	if (ev.activeItem != -1)
		//	{
		//		m_pPlayer->SetPlayListPos(ev.activeItem);
		//		//if (m_pPlayer->Open())
		//		m_pPlayer->Start();
		//	}
		//}
		break;
	}
}

void TrackListCtrl::DrawItem(Gdiplus::Graphics& g, INT nItem, Gdiplus::Rect& itemRC)
{
	HDC hdc = g.GetHDC();
	if (hdc == 0)
	{
		TRACE(_T("@1 TrackListCtrl::DrawItem. Cant get HDC\r\n"));
		return;		
	}
	CDC* pDC = CDC::FromHandle(hdc);
	PrgAPI* pAPI = PRGAPI();
	//Calculate Colors
	BOOL bSelected = IsItemSelected(nItem);
	COLORREF clrText = m_colors[COL_Text];
	COLORREF clrBk = m_colors[COL_Bk];
	if (bSelected)
	{
		clrText =  m_colors[COL_TextSel];
		clrBk = m_colors[COL_TextSelBk];
	}

	CRect rcSubItem(itemRC.X, itemRC.Y, itemRC.GetRight(), itemRC.GetBottom());
	pDC->SetTextColor(clrText);
	pDC->FillSolidRect(rcSubItem, clrBk);

	const INT cMargin = 2;

	FullTrackRecordSP& rec = (*m_pCollection)[nItem];

	pDC->SetBkMode(TRANSPARENT);

	INT curx = cMargin;


	CRect rcFirstLine(rcSubItem);
	rcFirstLine.bottom = rcFirstLine.top + 20;
	rcFirstLine.left = cMargin;
	rcFirstLine.right -= cMargin;
	CRect rcSecondLine(rcSubItem);
	rcSecondLine.top = rcFirstLine.bottom;
	rcSecondLine.left = cMargin;
	rcSecondLine.right -= cMargin;


	if (m_bDrawPictures)
	{
		INT imgHeight = rcSubItem.Height() - 2 * cMargin;
		INT cury = rcSubItem.top + cMargin;
		LocalPictureManager* pLM = PRGAPI()->GetLocalPictureManager();
		CRect rcImage(curx, cury, curx + imgHeight, cury + imgHeight);
		BOOL bRet = pLM->DrawThumbnail(IIT_AlbumPicture, rec->album.ID, hdc, rcImage);
		if (!bRet)
			bRet = pLM->DrawThumbnail(IIT_ArtistPicture, rec->artist.ID, hdc, rcImage);
		if (!bRet)
			bRet = pLM->DrawDefaultThumbnail(IIT_AlbumPicture, hdc, rcImage);

		curx += 32 + cMargin ;

	}

	rcSecondLine.left = curx;
	//=== Draw the icon
	INT cury = rcFirstLine.top + (rcFirstLine.Height() - 16) / 2;
	pDC->SetTextColor(clrText);
	DrawIconEx(pDC->m_hDC, curx, cury, pAPI->GetIconForTrackType(rec->track.trackType), 16, 16, 0, 0, DI_NORMAL);
	curx += 16 + cMargin;


	//=== Draw the title
	CRect rcTitle(rcFirstLine);
	rcTitle.left = curx;
	CFont* pOldFont = pDC->SelectObject(m_pBoldFont);
	pDC->DrawText(rec->track.name.c_str(), rec->track.name.size(), &rcTitle, DT_END_ELLIPSIS | DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	pDC->DrawText(rec->track.name.c_str(), rec->track.name.size(), &rcTitle, DT_END_ELLIPSIS | DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX | DT_CALCRECT);

	//=== Draw the artist
	CRect rcArtist(rcFirstLine);
	rcArtist.left = rcTitle.right + cMargin;
	pDC->DrawText(rec->artist.name.c_str(), rec->artist.name.size(), &rcArtist, DT_END_ELLIPSIS | DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

	pDC->SelectObject(m_pNormalFont);

	//=== Next line
	//=== Draw the rating (if exists)
	if (rec->track.rating > 0 && rec->track.rating < 256)
	{
		FLOAT fStars = Rating2Stars(rec->track.rating);
		if (fStars > 0.0f && fStars <=1.0f)
		{
			DrawIconEx(hdc, rcSecondLine.left, rcSecondLine.top, pAPI->GetIcon(ICO_StarBad16), 16, 16, 0, 0, DI_NORMAL);
			rcSecondLine.left += 17;
		}
		while (fStars > 1.0f)
		{
			DrawIconEx(hdc, rcSecondLine.left, rcSecondLine.top, pAPI->GetIcon(ICO_StarGold16), 16, 16, 0, 0, DI_NORMAL);
			fStars -= 1.0f;
			rcSecondLine.left += 17;
		}
	}

	TCHAR time[500];
	UINT len = _sntprintf(time, 500, _T("%d:%02d | %s: %s | %s: %s"), 
		INT (rec->track.duration / 60), INT(rec->track.duration) % 60,
		pAPI->GetString(IDS_ALBUM), rec->album.name.c_str(),
		pAPI->GetString(IDS_LOCATION), rec->track.location.c_str()
		);
	pDC->DrawText(time, len, &rcSecondLine, DT_END_ELLIPSIS | DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

	pDC->SelectObject(pOldFont);

	g.ReleaseHDC(pDC->m_hDC);
}
void TrackListCtrl::DrawBackground(Gdiplus::Graphics& g, const Gdiplus::Rect& r)
{
	CRect rc;
	HDC hdc = g.GetHDC();
	if (hdc == NULL)
	{
		TRACE(_T("@1 TrackListCtrl::DrawBackground. Cant get HDC\r\n"));
		return;
	}
	CDC* pDC = CDC::FromHandle(hdc);
	rc.SetRect(r.X, r.Y, r.GetRight(), r.GetBottom());
	pDC->FillSolidRect(&rc, m_colors[COL_Bk]);//RGB(0,0,0));
	//if (GetItemCount() == 0)
	//{
	//	rc.top += 10;
	//	pDC->SetTextColor(m_colors[COL_Text]);
	//	pDC->SetBkMode(TRANSPARENT);
	//	CFont* oldFont = pDC->SelectObject(m_pBoldFont);
	//	pDC->DrawText(PRGAPI()->GetString(IDS_EMPTY), -1, rc, 
	//		DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);
	//	pDC->SelectObject(oldFont);
	//}
	g.ReleaseHDC(pDC->m_hDC);

}


void TrackListCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	TRACEST(_T("TrackListCtrl::OnContextMenu"));
	if (m_pPlayer == NULL)	
		return;



	CPoint htp(point);
	ScreenToClient(&htp);
	m_menuInitiator = GetItemFromPoint(htp.x, htp.y);
	if (m_menuInitiator == -1)
		return;
	if (IsItemSelected(m_menuInitiator) == FALSE)
		SetItemSelection(m_menuInitiator, TRUE);

	ITSMenu& mainMenu = GetInternalMenu(M_Main);
	mainMenu.TrackPopupMenu(point.x, point.y, pWnd->GetSafeHwnd());

}

BOOL TrackListCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("TrackListCtrl::OnCommand"), wParam);
	switch (wParam)
	{
	case MI_Play:
	case MI_Enqueue:
	case MI_InsertAfter:
		if (m_menuInitiator != -1 && m_pPlayer != NULL && m_pCollection != NULL)
		{
			FullTrackRecordSP rec = (*m_pCollection)[m_menuInitiator];
			MediaPlayListItem mpli;
			mpli.artist = rec->artist.name.c_str();
			mpli.title = rec->track.name.c_str();
			mpli.rating = rec->track.rating;
			mpli.length = rec->track.duration;
			mpli.url = rec->track.location.c_str();
			switch (wParam)
			{
			case MI_Play:
				m_pPlayer->Play(mpli);
				break;
			case MI_Enqueue:
				m_pPlayer->Enqueue(mpli);
				break;
			case MI_InsertAfter:
				m_pPlayer->InsertMedia(mpli, m_pPlayer->GetPlayListPos() + 1);
				break;
			}
		}
		break;
	case MI_LocateAlbum:
		{
			if (m_menuInitiator != -1 && m_pCollection != NULL)
			{
				FullTrackRecordSP rec = (*m_pCollection)[m_menuInitiator];
				PrgAPI* pAPI = PRGAPI();
				SQLManager* pSM = pAPI->GetSQLManager();
				pAPI->GetStateManager()->GetState().locateTrackID = rec->track.ID;
				pAPI->SendMessage(SM_LocateTrackRequest);
			}
		}
		break;
	}
	return __super::OnCommand(wParam, lParam);
}

void TrackListCtrl::ApplyTranslation(ITranslation& translation)
{
	ResetAllMenus(FALSE);
}

void TrackListCtrl::ApplySkin(ISkin& _skin)
{
	TSSkin& skin = (TSSkin&) _skin;
	m_pNormalFont = skin.GetFont(CSEC_PlayList, FSTY_Normal);
	m_pBoldFont = skin.GetFont(CSEC_PlayList, FSTY_NormalBold);
	SetFont(m_pNormalFont);
	SetItemHeight(36 + skin.GetRelativeFontSize());
	SetColor(COL_Bk, skin.GetColor(CSEC_PlayList, CVAL_Bk));
	SetColor(COL_Text, skin.GetColor(CSEC_PlayList, CVAL_Text));
	SetColor(COL_TextSel, skin.GetColor(CSEC_PlayList, CVAL_SelText));
	SetColor(COL_TextSelBk, skin.GetColor(CSEC_PlayList, CVAL_SelBk));
}



//---------------------------------------------------
//ICtrlColors ---------------------------------------
//---------------------------------------------------
void TrackListCtrl::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		m_colors[idx] = value;
	//SetBkColor(m_colors[COL_Bk]);//RGB(0,0,0));
	//SetTextColor(m_colors[COL_Text]);//RGB(0,128,200));
	//SetTextBkColor(m_colors[COL_Bk]);//RGB(0,0,0));
	Refresh();
}
COLORREF TrackListCtrl::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}

void TrackListCtrl::PostCreate()
{
	if (m_pBoldFont == NULL)
	{
		PrgAPI* pAPI = PRGAPI();
		ApplySkin(pAPI->GetSkinManager()->GetSkin());
		ApplyTranslation(*pAPI->GetTranslationManager());
	}

}

ITSMenu& TrackListCtrl::GetInternalMenu(MenusEnum mnu)
{
	TRACEST(_T("TrackListCtrl::GetInternalMenu."), mnu);
	ASSERT(mnu < M_Last);
	PrgAPI* pAPI = PRGAPI();
	MenuManager* pMM = pAPI->GetMenuManager();
	if (m_pMenu[mnu] == NULL)
		m_pMenu[mnu] = pAPI->CreatePopupMenu();
	ITSMenu& curMenu = *m_pMenu[mnu];
	if (curMenu.GetInternalHandler() != NULL)
		return curMenu;
	curMenu.Create();
	switch (mnu)
	{
	case M_Main:
		//================= CONTEXT MENU TEMPLATE
		TCHAR bf[1000];
		_sntprintf(bf, 1000, _T("%s\t%s"), pAPI->GetString(IDS_PLAY), pAPI->GetString(IDS_KEYENTER));
		curMenu.AppendMenu(ITSMenu::MIT_String, MI_Play, bf, pAPI->GetIcon(ICO_Play16));
		_sntprintf(bf, 1000, _T("%s\t%s + %s"), pAPI->GetString(IDS_ENQUEUE), pAPI->GetString(IDS_KEYSHIFT), pAPI->GetString(IDS_KEYENTER));
		curMenu.AppendMenu(ITSMenu::MIT_String, MI_Enqueue, bf, pAPI->GetIcon(ICO_Add16));
		_sntprintf(bf, 1000, _T("%s\t%s + %s"), pAPI->GetString(IDS_INSERTAFTER), pAPI->GetString(IDS_KEYCTRL), pAPI->GetString(IDS_KEYENTER));
		curMenu.AppendMenu(ITSMenu::MIT_String, MI_InsertAfter, bf);
		curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
		_sntprintf(bf, 1000, _T("%s\t%s"), pAPI->GetString(IDS_LOCATEALBUM), pAPI->GetString(IDS_KEYHOME));
		curMenu.AppendMenu(ITSMenu::MIT_String, MI_LocateAlbum, bf, pAPI->GetIcon(ICO_Album16));
		break;
	default:
		ASSERT(0);
	}
	return curMenu;
}

void TrackListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	INT nItem = GetItemFromPoint(point.x, point.y);
	if (nItem >= 0)
	{
		FullTrackRecordSP rec = (*m_pCollection)[nItem];
		MediaPlayListItem mpli;
		mpli.artist = rec->artist.name.c_str();
		mpli.title = rec->track.name.c_str();
		mpli.rating = rec->track.rating;
		mpli.length = rec->track.duration;
		mpli.url = rec->track.location.c_str();
		m_pPlayer->Play(mpli);
	}

	CFlexListCtrl::OnLButtonDblClk(nFlags, point);
}
