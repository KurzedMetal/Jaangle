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
#include "PlayListListCtrl.h"
#include "MediaPlayer.h"
#include "MediaPlayList.h"
#include "ActionManager.h"
#include "MenuManager.h"
#include "prgapi.h"
#include "SQLManager.h"
#include "MediaPlayerUtilities.h"
#include "StateManager.h"
#include "FileTagger.h"


#define TIMER_CHECKTAGS 100
#define TIMER_CHECKTAGS_DELAY 200

//#include "FileTagger.h"


IMPLEMENT_DYNAMIC(CPlayListListCtrl, CListCtrl)

CPlayListListCtrl::CPlayListListCtrl():
m_pPlayer(NULL),
m_bDragging(FALSE),
m_DragItem(0),
m_curCX(0),
m_bOnDelay(FALSE),
m_bInitialized(FALSE),
m_pBoldFont(NULL),
m_pNormalFont(NULL)
{
	m_colors[COL_Bk] = RGB(0,0,0);
	m_colors[COL_Text] = RGB(100,100,250);
	m_colors[COL_TextCur] = RGB(240,210,10);
	m_colors[COL_TextSelCur] = m_colors[COL_TextCur];
	m_colors[COL_TextSel] = RGB(50,50,100);
	m_colors[COL_TextSelBk] =RGB(150,150,200);
	m_colors[COL_TextDisabled] =RGB(128,128,128);
	SetItemHeight(18);
	memset(&m_pMenu, 0, sizeof(m_pMenu));
	m_configOptions[C_ShowNumbers] = 0;
}
CPlayListListCtrl::~CPlayListListCtrl()
{
	ResetAllMenus(TRUE);
}
void CPlayListListCtrl::ResetAllMenus(BOOL bDelete)
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


BEGIN_MESSAGE_MAP(CPlayListListCtrl, CFlexListCtrl)
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CPlayListListCtrl::UpdateList()
{
	if (!m_bDragging)
	{
		if (m_pPlayer != 0)
		{
			UINT c = m_pPlayer->GetPlayListCount();
			TRACEST(_T("CPlayListListCtrl::UpdateList (p=Count)"), c);
			SetItemCount(c);
			UpdateActiveItem();
			Refresh();
		}
	}
}

void CPlayListListCtrl::UpdateActiveItem()
{
	if (m_pPlayer == NULL)
		return;
	INT curPos = m_pPlayer->GetPlayListPos();
	if (curPos >= 0)
	{
		EnsureVisible(curPos, TRUE);
		Refresh();
	}
}

void CPlayListListCtrl::OnItemMouseEvent(const ItemMouseEvent& ev)
{
	switch (ev.type)
	{
	case IMET_LButtonDblClick:
		if (m_pPlayer != NULL)
		{
			if (ev.activeItem != -1)
			{
				m_pPlayer->SetPlayListPos(ev.activeItem);
				//if (m_pPlayer->Open())
				m_pPlayer->Start();
			}
		}
		break;
	case IMET_LButtonDown:
		if(!m_bDragging) 
		{
			m_bDragging = TRUE;
			m_DragItem = ev.activeItem;
		}
		break;
	case IMET_LButtonUp:
		if(m_bDragging) 
			m_bDragging = FALSE;
		break;
	}
}

void CPlayListListCtrl::DrawItem(Gdiplus::Graphics& g, INT nItem, Gdiplus::Rect& itemRC)
{
	HDC hdc = g.GetHDC();
	if (hdc == 0)
	{
		TRACE(_T("@1 CPlayListListCtrl::DrawItem. Cant get HDC\r\n"));
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
	if (m_pPlayer && nItem == m_pPlayer->GetPlayListPos())
	{
		clrText = m_colors[COL_TextCur];//RGB(240,210,10);
		if (bSelected)
			clrText = m_colors[COL_TextSelCur];
	}
	//Draw Icon + Text

	CRect rcSubItem(itemRC.X, itemRC.Y, itemRC.GetRight(), itemRC.GetBottom());
	pDC->SetTextColor(clrText);
	pDC->FillSolidRect(rcSubItem, clrBk);

	const INT cMargin = 2;

	MediaPlayListItem mpli;
	if (m_pPlayer->GetPlayListItem(mpli, nItem))
	{
		if (mpli.artist == NULL)	
		{
			mpli.artist = _T("");
		}
		if (mpli.title == NULL || mpli.title[0] == 0)
		{
			mpli.title = mpli.url;
			LPCTSTR pos = _tcsrchr(mpli.title, '\\');
			if (pos != NULL)
			{
				mpli.title = pos + 1;
				m_pPlayer->UpdatePlayListItem(mpli, nItem);
			}
		}
		if (mpli.artist[0] == 0 && (mpli.flags & MPIF_TagsRead) == 0)
			SetTimer(TIMER_CHECKTAGS, TIMER_CHECKTAGS_DELAY, NULL);
		CFont* pOldFont = pDC->SelectObject(m_pNormalFont);
		pDC->SetBkMode(TRANSPARENT);

		INT curx = 0;

		//=== Draw the Track Number if it is selected
		if (m_configOptions[C_ShowNumbers])//pAPI->GetOption(OPT_PLAYLIST_ShowNumbers) != 0)
		{
			curx += cMargin;
			TCHAR num[50];
			UINT len = _sntprintf(num, 50, _T("%02d"), nItem + 1);
			CRect rcTest(rcSubItem);
			rcTest.left = curx;
			pDC->DrawText(num, len, &rcTest, DT_CALCRECT | DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			pDC->DrawText(num, len, &rcTest, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			curx += rcTest.Width();
		}

		//=== Draw the Icon
		INT cury = rcSubItem.top + (rcSubItem.Height() - 16) / 2;
		TrackTypesEnum tp = GetTrackType(mpli.url);
		if ((mpli.flags & MPIF_CannotPlay) ==  MPIF_CannotPlay)
			pDC->SetTextColor(m_colors[COL_TextDisabled]);
		else
		{
			curx += cMargin;
			pDC->SetTextColor(clrText);
			DrawIconEx(pDC->m_hDC, curx, cury, pAPI->GetIconForTrackType(tp), 16, 16, 0, 0, DI_NORMAL);
			curx += 16;
		}

		//=== Draw the Time
		TCHAR time[50];
		UINT len = _sntprintf(time, 50, _T("%d:%02d"), INT (mpli.length / 60), INT(mpli.length) % 60);
		CRect rcTest(rcSubItem);
		pDC->DrawText(time, len, &rcTest, DT_CALCRECT | DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		CRect rcTime(rcSubItem);
		rcTime.left = rcTime.right - rcTest.Width() - cMargin;
		rcTime.right -= cMargin;
		pDC->DrawText(time, len, &rcTime, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

		//=== Draw the Text
		curx += cMargin;
		CRect rcText(curx, rcSubItem.top, rcTime.left - 5, rcSubItem.bottom);

		if (mpli.artist[0] != 0)
		{
			rcTest = rcText;
			pDC->DrawText(mpli.title, -1, &rcTest, DT_CALCRECT | DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			CRect trackRC(rcText);
			if (rcText.Width() - rcTest.Width() < 25)
				pDC->DrawText(mpli.title, -1, &trackRC, DT_END_ELLIPSIS | DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			else
			{
				pDC->DrawText(mpli.title, -1, &trackRC, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				CRect artRC(rcText);
				artRC.left = rcTest.right + 10;
				pDC->SelectObject(m_pBoldFont);
				pDC->DrawText(mpli.artist, -1, &artRC, DT_END_ELLIPSIS | DT_RIGHT | DT_NOCLIP | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			}
		}
		else
		{
			pDC->DrawText(mpli.title, -1, &rcText, DT_END_ELLIPSIS | DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
		}


		pDC->SelectObject(pOldFont);

	}
	g.ReleaseHDC(pDC->m_hDC);

}

void CPlayListListCtrl::SetMediaPlayer(MediaPlayer* pPlayer)
{
	ASSERT(pPlayer != NULL);
	m_pPlayer = pPlayer;
	UpdateList();
}

void CPlayListListCtrl::DrawBackground(Gdiplus::Graphics& g, const Gdiplus::Rect& r)
{
	CRect rc;
	HDC hdc = g.GetHDC();
	if (hdc == NULL)
	{
		TRACE(_T("@1 CPlayListListCtrl::DrawBackground. Cant get HDC\r\n"));
		return;
	}
	CDC* pDC = CDC::FromHandle(hdc);
	rc.SetRect(r.X, r.Y, r.GetRight(), r.GetBottom());
	pDC->FillSolidRect(&rc, m_colors[COL_Bk]);//RGB(0,0,0));
	if (GetItemCount() == 0)
	{
		rc.top += 10;
		pDC->SetTextColor(m_colors[COL_Text]);
		pDC->SetBkMode(TRANSPARENT);
		CFont* oldFont = pDC->SelectObject(m_pBoldFont);
		pDC->DrawText(PRGAPI()->GetString(IDS_PLAYLISTISEMPTY), -1, rc, 
			DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);
		pDC->SelectObject(oldFont);
	}
	g.ReleaseHDC(pDC->m_hDC);

}

void CPlayListListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bDragging)
	{
		// Disable list drawing for less flickering
		//SetRedraw(FALSE);

		// Now find the item where the mouse cursor is
		UINT flags=0;
		UINT index = GetItemFromPoint(point.x, point.y);

		// No valid item found? Perhaps the mouse is outside the list
		if(index==-1)
		{
			int top = GetItemFromPoint(0, 0);
			CRect rc;
			GetClientRect(&rc);
			int	last = GetItemFromPoint(0, rc.Height());
			if (last == -1)
				last = GetItemCount();
			// Mouse is under the listview, so pretend it's over the last item
			// in view
			if (point.y > rc.Height())
				index=last;
			else 
				// Mouse is above the listview, so pretend it's over the top item in
				// view - 1
				index= top > 0 ? top - 1 : 0;
		}

		// Do we have a valid item now?
		if(index!=-1) {
			// calculate the offset between the two items
			int offset = index-m_DragItem;
			// Is it not the same item?
			if(offset != 0) {
				// Do we have a multiple selection?
				UINT selectedcount = GetSelectedItemCount();
				// Create an array of selected items (could use CArray here)
				UINT *selected = new UINT[selectedcount];
				// Add all selected items to this array
				UINT i = 0;
				UINT nItem = GetNextSelectedItem();
				while(nItem != -1) 
				{
					selected[i++] = nItem;
					nItem = GetNextSelectedItem(nItem);
				}
				// Now we are going to move the selected items
				for(i=0;i<selectedcount;i++){
					// If we are moving the selection downward, we'll start
					// with the last one and iterate up. Else start with
					// the first one and iterate down.
					int iterator = (offset>0) ? selectedcount-i-1 : i;
					// Now get the position of the first selected item
					int oldpos = selected[iterator];
					// Calculate the new position
					int newpos = oldpos+offset;
					// Is the new position outsize the list's boundaries? break
					if(newpos<0 || newpos>=GetItemCount()) break;
					// Unselect the item
					//SetItemSelection(oldpos, FALSE);
					// No we keep swapping items until the selected
					// item reaches the new position
					if(offset>0) {
						// Going down
						for(int j=oldpos;j<newpos;j++) 
							SwapRows(j,j+1);
					}else {
						// Going up
						for(int j=oldpos;j>newpos;j--) 
							SwapRows(j,j-1);
					}
					// Make sure the newposition is in view
					EnsureVisible(newpos,TRUE);
					// Select it again
					//SetItemSelection(oldpos, TRUE);
					//PRGAPI()->SendMessage(SM_PlayListChanged);
				}
				// Free the array
				delete [] selected;
				// Set the dragging item to the current index position,
				// so we can start over again
				m_DragItem=index;
			}
		}
		// Enable drawing in the listview again
		//SetRedraw(TRUE);
	}
	__super::OnMouseMove(nFlags, point);
}

BOOL CPlayListListCtrl::SwapRows(UINT row1,UINT row2)
{
	// In this function we need to swap two rows,
	// Here it does some mangling with the listview's item texts/image/userdata
	// If you have a virtual list view you can swap it's items here
	if (m_pPlayer == NULL)	return FALSE;
	INT idx = m_pPlayer->GetPlayListPos();
	if (idx == row1)
		idx = row2;
	else if (idx == row2)
		idx = row1;

	SwapItem(row1, row2);
	m_pPlayer->SwapPlayListItems(row1, row2);
	return 1;
}

void CPlayListListCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	TRACEST(_T("CPlayListListCtrl::OnContextMenu"));
	if (m_pPlayer == NULL)	return;
	CPoint htp(point);
	ScreenToClient(&htp);
	m_MenuInitiator = GetItemFromPoint(htp.x, htp.y);
	if (m_MenuInitiator != -1)
	{
		if (IsItemSelected(m_MenuInitiator) == FALSE)
		{
			ClearSelection();
			SetItemSelection(m_MenuInitiator, TRUE);
		}
	}
	else
		ClearSelection();

	ITSMenu& playListMenu = GetInternalMenu(M_Main);
	PrepareInsertModeMenu(playListMenu);
	PrepareSearchTracksMenu(playListMenu);
	MenuManager* pMM = PRGAPI()->GetMenuManager();
	pMM->PreparePlayerCommands(*m_pPlayer);

	INT nSelCount = GetSelectedItemCount();
	playListMenu.EnableMenuItem(MI_RemoveSelected, nSelCount > 0);
	playListMenu.EnableMenuItem(MI_RemoveOthers, nSelCount > 0);
	playListMenu.EnableMenuItem((INT)pMM->GetMenu(MenuManager::MNU_Rating).GetInternalHandler(), nSelCount > 0);
	playListMenu.EnableMenuItem(MI_LocateAlbum, nSelCount == 1);
	playListMenu.EnableMenuItem((INT)GetInternalMenu(M_SearchItem).GetInternalHandler(), nSelCount == 1);

	playListMenu.TrackPopupMenu(point.x, point.y, pWnd->GetSafeHwnd());

}

//void CPlayListListCtrl::RandomizeItems(std::vector<INT>& selItems)
//{
//	srand(GetTickCount());
//	if (selItems.size() < 2)
//	{
//		return;
//	}
//	else if (selItems.size() == 2)
//	{
//		m_pPlayer->SwapPlayListItems(selItems[0], selItems[1]);
//	}
//	else
//	{
//		UINT numItems = selItems.size();
//		UINT swapItem = 0;
//		for (UINT i = 0; i < numItems; i++)
//		{
//			swapItem = rand() * numItems / RAND_MAX;
//			if (swapItem != i)
//			{
//				m_pPlayer->SwapPlayListItems(selItems[swapItem], selItems[i]);
//			}
//		}	
//	}
//	UpdateList();
//
//}

BOOL CPlayListListCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CPlayListListCtrl::OnCommand"), wParam);
	if (m_pPlayer == NULL)	
		return __super::OnCommand(wParam, lParam);
	PrgAPI* pAPI = PRGAPI();
	MenuManager* pMM = pAPI->GetMenuManager();
	ActionManager* pAM = pAPI->GetActionManager();
	MenuCommandsEnum cmd = (MenuCommandsEnum) wParam;
	INT oldItemCount = m_pPlayer->GetPlayListCount();
	switch (wParam)
	{
	case MI_LocateAlbum:
		{
			MediaPlayListItem mpli;
			if (m_pPlayer->GetPlayListItem(mpli, m_MenuInitiator))
			{
				SQLManager* pSM = pAPI->GetSQLManager();
				FullTrackRecordSP rec;
				if (pSM->GetFullTrackRecordByLocation(rec, mpli.url))
				{
					pAPI->GetStateManager()->GetState().locateTrackID = rec->track.ID;
					pAPI->SendMessage(SM_LocateTrackRequest);
				}
			}
		}
		break;
	case MI_RandomizeList:
		{
			std::vector<INT> selItems;
			for (int i = 0; i < GetItemCount(); i++)
			{
				if (IsItemSelected(i))
					selItems.push_back(i);
			}
			if (selItems.size() <= 1)
			{
				selItems.clear();
				for (int i = 0; i < GetItemCount(); i++)
					selItems.push_back(i);
			}
			pAPI->GetActionManager()->RandomizePlayListItems(*m_pPlayer, selItems);
			UpdateList();
		}
		break;
	case MI_RandomizeNext:
		{
			std::vector<INT> selItems;
			for (int i = m_MenuInitiator + 1; i < GetItemCount(); i++)
				selItems.push_back(i);
			pAPI->GetActionManager()->RandomizePlayListItems(*m_pPlayer, selItems);
			UpdateList();
		}
		break;

	case MI_RemoveAll:
		m_pPlayer->ClearPlayList();
		break;
	case MI_RemoveSelected:
		for (int i = GetItemCount() - 1; i >= 0; i--)
		{
			if (IsItemSelected(i))
			{
				SetItemSelection(i, FALSE);
				m_pPlayer->RemovePlayListItem(i);
			}
		}
		break;
	case MI_RemoveOthers:
		for (int i = GetItemCount() - 1; i >= 0; i--)
			if (!IsItemSelected(i))
				m_pPlayer->RemovePlayListItem(i);
		ClearSelection();
		break;
	default:
		if (cmd>= MENU_RatingHolder && cmd <= MENU_RatingHolder + 5)
		{
			//=== This special case is here for multi - selected
			for (INT i = 0; i < GetItemCount(); i++)
			{
				if (IsItemSelected(i))
					pMM->HandlePlayerCommands(*m_pPlayer, cmd, i);
			}
		}
		else if (pMM->HandlePlayerCommands(*m_pPlayer, cmd, m_MenuInitiator))
			return TRUE;
		else if (pMM->HandleGeneralCommands(cmd))
			return TRUE;
		else if (wParam >= MI_InsertRandomTracksHolder && 
			wParam < MI_InsertRandomTracksHolder + RTM_Last)
		{
			INT realMenuInitiator = m_MenuInitiator;
			if (realMenuInitiator == -1)
				realMenuInitiator = GetItemCount() - 1;
			if (realMenuInitiator >= 0)
			{
				MediaPlayListItem mpli;
				if (m_pPlayer->GetPlayListItem(mpli, realMenuInitiator) && mpli.url != NULL)
				{
					FullTrackRecordSP rec;
					if (pAPI->GetSQLManager()->GetFullTrackRecordByLocation(rec, mpli.url))
					{
						ActionManager* pAM = pAPI->GetActionManager();
						TracksFilter tf;
						RandomTrackModeEnum mode = RandomTrackModeEnum(wParam - MI_InsertRandomTracksHolder);
						pAM->GetRelativeRandomTrackFilter(tf, rec, mode);
						FullTrackRecordCollection col;
						pAM->GetRandomTrackCollection(col, tf, 5);
						if (col.size() > 0)
						{
							//int nItem = GetNextSelectedItem();
							//if (nItem == -1)
							//	nItem = m_pPlayer->GetPlayListCount();
							pAM->InsertMedia(pAPI->GetMediaPlayer(), col, realMenuInitiator + 1);
						}
					}
				}
			}
		}
		else if (wParam >= MI_SearchTracksHolder && 
			wParam < MI_SearchTracksHolder + RTM_Last)
		{
			if (m_MenuInitiator >= 0)
			{
				MediaPlayListItem mpli;
				if (m_pPlayer->GetPlayListItem(mpli, m_MenuInitiator) && mpli.url != NULL)
				{
					FullTrackRecordSP rec;
					if (pAPI->GetSQLManager()->GetFullTrackRecordByLocation(rec, mpli.url))
					{
						TrackTextType ttt = TrackTextType(wParam - MI_SearchTracksHolder);
						TSState& state = pAPI->GetStateManager()->GetState();
						//						state.sectionSelectionStyle = TSState::SSS_QuickSearch;
						LPCTSTR qString = NULL;
						switch (ttt)
						{
						case TTT_Title:
							qString = rec->track.name.c_str();
							break;
						case TTT_Album:
							qString = rec->album.name.c_str();
							break;
						case TTT_Artist:
							qString = rec->artist.name.c_str();
							break;
						default:
							ASSERT(0);
						}
						//===
						pAPI->GetStateManager()->GetState().quickSearchString = qString;
						pAPI->SendMessage(SM_SetQuickSearch);
					}
				}
			}
		}


		
		break;
	}
	if (m_pPlayer->GetPlayListCount() != oldItemCount)
		PRGAPI()->SendMessage(SM_PlayListChanged);
	return __super::OnCommand(wParam, lParam);
}

BOOL CPlayListListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if ( pMsg->message == WM_CHAR )
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
		case 0x08:	// BackSpace Button
			return TRUE;
		}
	}
	else if (pMsg->message == WM_KEYDOWN)
	{
		BOOL bHandled = TRUE;
		switch (pMsg->wParam)
		{
		case VK_DELETE:
			if (m_pPlayer != NULL)
			{
				INT nSelItem = GetNextSelectedItem();
				INT firstSelItem = nSelItem;
				while (nSelItem != -1)
				{
					DeleteItems(nSelItem, 1);
					m_pPlayer->RemovePlayListItem(nSelItem);
					nSelItem = GetNextSelectedItem(nSelItem - 1);
				}
				if (firstSelItem < GetItemCount())
					SetItemSelection(firstSelItem, TRUE);
				else if (firstSelItem > 0)
					SetItemSelection(firstSelItem - 1, TRUE);
				PRGAPI()->SendMessage(SM_PlayListChanged);
			}
			break;
		case VK_RETURN:
			if (m_pPlayer != NULL)
			{
				INT nItem = GetNextSelectedItem();
				if (nItem!= -1)
				{
					m_pPlayer->SetPlayListPos(nItem);
					m_pPlayer->Start();
				}
			}
			break;
		case VK_HOME:
			m_MenuInitiator = GetNextSelectedItem();
			if (m_MenuInitiator != -1)
				OnCommand(MI_LocateAlbum, 0);
			break;
		default:
			bHandled = FALSE;
		}
		if (bHandled)
			return 0;
	}

	return __super::PreTranslateMessage(pMsg);
}

void CPlayListListCtrl::OnSelectAll()
{
	for (int i = 0; i < GetItemCount(); i++)
		SetItemSelection(i, TRUE);
}

void CPlayListListCtrl::OnInvertSelection()
{
	for (int i = 0; i < GetItemCount(); i++)
		SetItemSelection(i, !IsItemSelected(i));
}

void CPlayListListCtrl::ApplyTranslation(ITranslation& translation)
{
	ResetAllMenus(FALSE);
}

void CPlayListListCtrl::ApplySkin(ISkin& _skin)
{
	TSSkin& skin = (TSSkin&) _skin;
	m_pNormalFont = skin.GetFont(CSEC_PlayList, FSTY_Normal);
	m_pBoldFont = skin.GetFont(CSEC_PlayList, FSTY_NormalBold);
	SetFont(m_pNormalFont);
	SetItemHeight(skin.GetIntParameter(CSEC_PlayList, IPARAM_Height) + skin.GetRelativeFontSize());
	SetColor(COL_Bk, skin.GetColor(CSEC_PlayList, CVAL_Bk));
	SetColor(COL_Text, skin.GetColor(CSEC_PlayList, CVAL_Text));
	SetColor(COL_TextSel, skin.GetColor(CSEC_PlayList, CVAL_SelText));
	SetColor(COL_TextSelBk, skin.GetColor(CSEC_PlayList, CVAL_SelBk));
	SetColor(COL_TextCur, skin.GetColor(CSEC_PlayList, CVAL_CurText));
	SetColor(COL_TextSelCur, skin.GetColor(CSEC_PlayList, CVAL_SelCurText));
	SetColor(COL_TextDisabled, skin.GetColor(CSEC_PlayList, CVAL_DisabledText));
	UpdateList();
}



//---------------------------------------------------
//ICtrlColors ---------------------------------------
//---------------------------------------------------
void CPlayListListCtrl::SetColor(UINT idx, COLORREF value)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		m_colors[idx] = value;
	//SetBkColor(m_colors[COL_Bk]);//RGB(0,0,0));
	//SetTextColor(m_colors[COL_Text]);//RGB(0,128,200));
	//SetTextBkColor(m_colors[COL_Bk]);//RGB(0,0,0));
	Refresh();
}
COLORREF CPlayListListCtrl::GetColor(UINT idx)
{
	ASSERT(idx < COL_Last);
	if (idx < COL_Last)
		return m_colors[idx];
	return 0;
}

void CPlayListListCtrl::RecurseDirectories(LPCTSTR startPath, INT& playListPos)
{
	ASSERT(startPath != NULL);
	ASSERT(_tcslen(startPath) < MAX_PATH - 10);
	PrgAPI* pAPI = PRGAPI();
	TCHAR wildCard[MAX_PATH];
	UINT NumTracks = 0;
	WIN32_FIND_DATA wfd;

	_sntprintf(wildCard, MAX_PATH, _T("%s\\*.*"), startPath);
	HANDLE hFF = ::FindFirstFile(wildCard, &wfd);
	BOOL bContinueSearch = TRUE;
	while (bContinueSearch && (hFF != INVALID_HANDLE_VALUE))
	{
		if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
		{
			BOOL IsDots = FALSE;
			if (wfd.cFileName[0] == '.')
			{
				if (wfd.cFileName[1] == '\0' ||	(wfd.cFileName[1] == '.' &&	wfd.cFileName[2] == '\0'))
					IsDots = TRUE;
			}
			if (!IsDots)
			{
				_sntprintf(wildCard, MAX_PATH, _T("%s\\%s"), startPath, wfd.cFileName);
				RecurseDirectories(wildCard, playListPos);
			}
		}
		else
		{
			if (m_pPlayer->CanPlay(wfd.cFileName))
			{
				_sntprintf(wildCard, MAX_PATH, _T("%s\\%s"), startPath, wfd.cFileName);
				AddPlayListItem(wildCard, playListPos);
				playListPos++;
			}
		}
		bContinueSearch = ::FindNextFile(hFF, &wfd);
	}
	if (hFF != INVALID_HANDLE_VALUE)
		::FindClose(hFF);
}


void CPlayListListCtrl::AddPlayListItem(LPCTSTR fileName, INT playListPos)
{
	PrgAPI* pAPI = PRGAPI();
	MediaPlayListItem mpli(fileName);
	FullTrackRecordSP rec;
	TagInfo ti;
	if (pAPI->GetSQLManager()->GetFullTrackRecordByLocation(rec, fileName))
	{
		//=== File is included in the database 
		mpli.artist = rec->artist.name.c_str();
		mpli.title = rec->track.name.c_str();
		mpli.length = rec->track.duration;
		mpli.rating = rec->track.rating;
	}
	if (pAPI->GetMediaPlayer()->InsertMedia(mpli, playListPos))
		InsertItems(playListPos, 1);
}



BOOL CPlayListListCtrl::OnFileDrop(const std::vector<std::tstring>& files, CPoint& p)
{
	TRACEST(_T("CPlayListListCtrl::OnFileDrop"));
	std::vector<std::tstring>::const_iterator it = files.begin();
	INT nDropOverItem = GetItemFromPoint(p.x, p.y);
	if (nDropOverItem == -1)
		nDropOverItem = GetItemCount();
	PrgAPI* pAPI = PRGAPI();
	for (; it != files.end(); it++)
	{
		LPCTSTR fileName = (*it).c_str();
		if (PathIsDirectory(fileName))
		{
			RecurseDirectories(fileName, nDropOverItem);
		}
		else if (m_pPlayer->CanPlay(fileName))
		{
			AddPlayListItem(fileName, nDropOverItem);
			nDropOverItem++;
		}

	}
	return TRUE;
}

void CPlayListListCtrl::UpdateConfiguration()
{
	Refresh();
}

void CPlayListListCtrl::PostCreate()
{
	if (m_pBoldFont == NULL)
	{
		PrgAPI* pAPI = PRGAPI();
		ApplySkin(pAPI->GetSkinManager()->GetSkin());
		ApplyTranslation(*pAPI->GetTranslationManager());
	}

}



void CPlayListListCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_CHECKTAGS)
	{
		KillTimer(TIMER_CHECKTAGS);
		INT nItem = GetNextVisibleItem(-1);
		MediaPlayListItem mpli;
		while (nItem != -1)
		{
			if (m_pPlayer->GetPlayListItem(mpli, nItem))
			{
				if (mpli.artist[0] == 0 && (mpli.flags & MPIF_TagsRead) == 0)
				{
					mpli.flags |= MPIF_TagsRead;
					PrgAPI* pAPI = PRGAPI();
					//=== First Check the Database
					FullTrackRecordSP rec;
					TagInfo ti;
					if (pAPI->GetSQLManager()->GetFullTrackRecordByLocation(rec, mpli.url))
					{
						mpli.artist = rec->artist.name.c_str();
						mpli.title = rec->track.name.c_str();
						mpli.length = rec->track.duration;
						mpli.rating = rec->track.rating;
					}
					else
					{
						//=== If it is not in database then read the tags
						FileTagger* pFT = pAPI->GetFileTagger();
						TRACE(_T("CPlayListListCtrl::OnTimer Reading Tags: '%s'\r\n"), mpli.url);
						if (pFT->Read(mpli.url, ti))
						{
							mpli.artist = ti.Artist.c_str();
							mpli.title = ti.Title.c_str();
							mpli.length = ti.Length;
							mpli.rating = ti.Rating;
						}
					}
					m_pPlayer->UpdatePlayListItem(mpli, nItem);
					InvalidateItem(nItem);
					Invalidate(FALSE);
					SetTimer(TIMER_CHECKTAGS, TIMER_CHECKTAGS_DELAY, NULL);
					break;
				}

			}
			nItem = GetNextVisibleItem(nItem);
		}
	}


	__super::OnTimer(nIDEvent);
}


ITSMenu& CPlayListListCtrl::GetInternalMenu(MenusEnum mnu)
{
	TRACEST(_T("CPlayListListCtrl::GetInternalMenu."), mnu);
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
		//ContextMenu->
		//	remove (>0 items)
		//	RemoveOthers (<count items)
		//	ClearList
		//	RandomizeList
		//	-
		//	(PlayModes)
		//	-
		//	(PlayList) 
		//	-
		//	(Rating) (1 item)
		//	-
		//	InsertRandomTracks-> (>0 items)
		//	-
		//	LocateAlbum (1 item)
		//	SearchItem-> (1 item)
		curMenu.AppendMenu(ITSMenu::MIT_String, MI_RemoveSelected, (LPTSTR)pAPI->GetString(IDS_REMOVE), pAPI->GetIcon(ICO_Remove16));
		curMenu.AppendMenu(ITSMenu::MIT_String, MI_RemoveOthers, (LPTSTR)pAPI->GetString(IDS_REMOVEOTHERS));
		curMenu.AppendMenu(ITSMenu::MIT_String, MI_RemoveAll, (LPTSTR)pAPI->GetString(IDS_CLEARLIST), pAPI->GetIcon(ICO_Clear16));
		curMenu.AppendMenu(ITSMenu::MIT_String, MI_RandomizeList,(LPTSTR) pAPI->GetString(IDS_RANDOMIZELIST));
		curMenu.AppendMenu(ITSMenu::MIT_String, MI_RandomizeNext,(LPTSTR) pAPI->GetString(IDS_RANDOMIZENEXT));
		curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
		curMenu.AppendMenu(ITSMenu::MIT_Menu, (INT)pMM->GetMenu(MenuManager::MNU_PlayListModes).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_MODE));
		curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
		curMenu.AppendMenu(ITSMenu::MIT_Menu, (INT)pMM->GetMenu(MenuManager::MNU_PlayListSerialization).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_PLAYLIST));
		curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
		curMenu.AppendMenu(ITSMenu::MIT_Menu, (INT)pMM->GetMenu(MenuManager::MNU_Rating).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_RATING), pAPI->GetIcon(ICO_StarGold16));
		curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
		curMenu.AppendMenu(ITSMenu::MIT_Menu, (INT)GetInternalMenu(M_InsertRandomTracks).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_INSERTRANDOMTRACKS), pAPI->GetIcon(ICO_Insert16));
		curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
		curMenu.AppendMenu(ITSMenu::MIT_String, MI_LocateAlbum, (LPTSTR)pAPI->GetString(IDS_LOCATEALBUM), pAPI->GetIcon(ICO_Album16));
		curMenu.AppendMenu(ITSMenu::MIT_Menu, (INT)GetInternalMenu(M_SearchItem).GetInternalHandler(), (LPTSTR)pAPI->GetString(IDS_SEARCH), pAPI->GetIcon(ICO_Find16));
		break;
	case M_InsertRandomTracks:
		for (int i = RTM_All; i < RTM_Last; i++)
		{
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_InsertRandomTracksHolder + i, (LPTSTR)pAPI->GetStringForRandomMode((RandomTrackModeEnum)i));
			if (i == RTM_All)
				curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
		}
		break;
	case M_SearchItem:
		for (int i = 0; i < TTT_Last; i++)
			curMenu.AppendMenu(ITSMenu::MIT_String, MI_SearchTracksHolder + i, (LPTSTR)pAPI->GetStringForTrackTextType((TrackTextType)i));
		break;
	default:
		ASSERT(0);
	}
	return curMenu;
}


//case MNU_InsertRandomTracks:
//	for (int i = RTM_All; i < RTM_Last; i++)
//	{
//		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_InsertRandomTracksHolder + i, (LPTSTR)pAPI->GetStringForRandomMode((RandomTrackModeEnum)i));
//		if (i == RTM_All)
//			curMenu.AppendMenu(ITSMenu::MIT_Separator, 0, NULL);
//	}
//	break;
//case MNU_SearchTracks:
//	for (int i = 0; i < TTT_Last; i++)
//		curMenu.AppendMenu(ITSMenu::MIT_String, MENU_SearchTracksHolder + i, (LPTSTR)pAPI->GetStringForTrackTextType((TrackTextType)i));
//	break;



void CPlayListListCtrl::PrepareInsertModeMenu(ITSMenu& mnu)
{
	for (int i = RTM_All; i < RTM_Last; i++)
		mnu.EnableMenuItem(MI_InsertRandomTracksHolder + i, TRUE);
	if (m_MenuInitiator == -1)
	{
		for (int i = RTM_All + 1; i < RTM_Last; i++)
			mnu.EnableMenuItem(MI_InsertRandomTracksHolder + i, FALSE);
	}
}
void CPlayListListCtrl::PrepareSearchTracksMenu(ITSMenu& mnu)
{
	for (int i = 0; i < TTT_Last; i++)
		mnu.EnableMenuItem(MI_SearchTracksHolder + i, TRUE);
}

void CPlayListListCtrl::SetConfigurationOption(ConfigurationOptions idx, INT value)
{
	ASSERT(idx < C_Last);
	if (idx < C_Last)
		m_configOptions[idx] = value;
	if (idx == C_ShowNumbers)
		Refresh();
}
INT CPlayListListCtrl::GetConfigurationOption(ConfigurationOptions idx)
{
	ASSERT(idx < C_Last);
	if (idx < C_Last)
		return m_configOptions[idx];
	return -1;
}
