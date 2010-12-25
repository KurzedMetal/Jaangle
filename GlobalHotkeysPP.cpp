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
#include "GlobalHotkeysPP.h"
#include "PrgAPi.h"
#include "MediaPlayer.h"
#include "HotKeyManager.h"

// CMediaPlayerPP dialog

IMPLEMENT_DYNAMIC(CGlobalHotkeysPP, CPropertyPage)

CGlobalHotkeysPP::CGlobalHotkeysPP(UINT nIDCaption)
	: CTSPropertyPage(CGlobalHotkeysPP::IDD, nIDCaption)
{

}

CGlobalHotkeysPP::~CGlobalHotkeysPP()
{
}

void CGlobalHotkeysPP::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HK_PLAY, m_HKPlay);
	DDX_Control(pDX, IDC_HK_STOP, m_HKStop);
	DDX_Control(pDX, IDC_HK_PREVIOUS, m_HKPrevious);
	DDX_Control(pDX, IDC_HK_NEXT, m_HKNext);
	DDX_Control(pDX, IDC_HK_MINIPLAYER, m_HKMiniPlayer);
	DDX_Control(pDX, IDC_HK_TOOLTIP, m_HKTooltip);
	DDX_Control(pDX, IDC_HK_QUICKSEARCH, m_HKQuickSearch);

	
}


BEGIN_MESSAGE_MAP(CGlobalHotkeysPP, CPropertyPage)
	ON_EN_CHANGE(IDC_HK_PLAY, &CGlobalHotkeysPP::OnChange)
	ON_EN_CHANGE(IDC_HK_STOP, &CGlobalHotkeysPP::OnChange)
	ON_EN_CHANGE(IDC_HK_PREVIOUS, &CGlobalHotkeysPP::OnChange)
	ON_EN_CHANGE(IDC_HK_NEXT, &CGlobalHotkeysPP::OnChange)
	ON_EN_CHANGE(IDC_HK_MINIPLAYER, &CGlobalHotkeysPP::OnChange)

	ON_BN_CLICKED(IDC_CHK_USEMULTIMEDIAHOTKEYS, &CGlobalHotkeysPP::OnChange)
END_MESSAGE_MAP()

void CGlobalHotkeysPP::ApplyTranslation(ITranslation& translation)
{
	if (m_hWnd)
	{
		PrgAPI* pAPI = PRGAPI();
		SetDlgItemText(IDC_GR_GLOBALHOTKEYS, pAPI->GetString(IDS_GLOBALHOTKEYS));
		SetDlgItemText(IDC_ST_PLAY, pAPI->GetString(IDS_PLAY));
		SetDlgItemText(IDC_ST_PREVIOUS, pAPI->GetString(IDS_PREVIOUS));
		SetDlgItemText(IDC_ST_NEXT, pAPI->GetString(IDS_NEXT));
		SetDlgItemText(IDC_ST_STOP, pAPI->GetString(IDS_STOP));
		TCHAR bf[1000];
		_sntprintf(bf, 1000, _T("%s (%s)"), pAPI->GetString(IDS_MINIPLAYER), pAPI->GetString(IDS_TOGGLE));
		SetDlgItemText(IDC_ST_MINIPLAYER, bf);
		SetDlgItemText(IDC_ST_TOOLTIP, pAPI->GetString(IDS_TRAYTOOLTIP));
		SetDlgItemText(IDC_ST_QUICKSEARCH, pAPI->GetString(IDS_SEARCH));
		SetDlgItemText(IDC_CHK_USEMULTIMEDIAHOTKEYS, pAPI->GetString(IDS_USEMULTIMEDIAHOTKEYS));
	}
}
BOOL CGlobalHotkeysPP::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	ApplyTranslation(*PRGAPI()->GetTranslationManager());
	LoadGlobalHotkeysOptions();
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM) PRGAPI()->GetFont(FONT_Dialogs)->m_hObject);
	return TRUE;  // return TRUE unless you set the focus to a control
}
void CGlobalHotkeysPP::OnChange()
{
	if (IsWindowVisible())
	{
		m_bNeedsUpdating = TRUE;
		SetModified(TRUE);
	}
}

void CGlobalHotkeysPP::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	OnChange();
}


void CGlobalHotkeysPP::OnOK()
{
	SaveChanges();
	CPropertyPage::OnOK();
}

void CGlobalHotkeysPP::SaveChanges()
{
	if (m_bNeedsUpdating)
	{
		m_bSomethingChanged = TRUE;
		SaveGlobalHotkeysOptions();
	}
}

void CGlobalHotkeysPP::LoadGlobalHotkeysOptions()
{
	PrgAPI* pAPI = PRGAPI();
	HotKeyManager* pHKM = pAPI->GetHotKeyManager();

	WORD wVKCode, wMod;
	pHKM->GetHotKey(HotKeyManager::HK_Play, wVKCode, wMod);
	m_HKPlay.SetHotKey(wVKCode, wMod);
	pHKM->GetHotKey(HotKeyManager::HK_Stop, wVKCode, wMod);
	m_HKStop.SetHotKey(wVKCode, wMod);
	pHKM->GetHotKey(HotKeyManager::HK_Prev, wVKCode, wMod);
	m_HKPrevious.SetHotKey(wVKCode, wMod);
	pHKM->GetHotKey(HotKeyManager::HK_Next, wVKCode, wMod);
	m_HKNext.SetHotKey(wVKCode, wMod);
	pHKM->GetHotKey(HotKeyManager::HK_ToggleMiniPlayer, wVKCode, wMod);
	m_HKMiniPlayer.SetHotKey(wVKCode, wMod);
	pHKM->GetHotKey(HotKeyManager::HK_Tooltip, wVKCode, wMod);
	m_HKTooltip.SetHotKey(wVKCode, wMod);
	pHKM->GetHotKey(HotKeyManager::HK_QuickSearch, wVKCode, wMod);
	m_HKQuickSearch.SetHotKey(wVKCode, wMod);


	CheckDlgButton(IDC_CHK_USEMULTIMEDIAHOTKEYS, pHKM->IsMultimediaHotKeysEnabled());

}

void CGlobalHotkeysPP::SaveGlobalHotkeysOptions()
{
	PrgAPI* pAPI = PRGAPI();
	HotKeyManager* pHKM = pAPI->GetHotKeyManager();
	WORD wVKCode, wMod;
	m_HKPlay.GetHotKey(wVKCode, wMod);
	pHKM->SetHotKey(HotKeyManager::HK_Play, wVKCode, wMod);
	m_HKStop.GetHotKey(wVKCode, wMod);
	pHKM->SetHotKey(HotKeyManager::HK_Stop, wVKCode, wMod);
	m_HKPrevious.GetHotKey(wVKCode, wMod);
	pHKM->SetHotKey(HotKeyManager::HK_Prev, wVKCode, wMod);
	m_HKNext.GetHotKey(wVKCode, wMod);
	pHKM->SetHotKey(HotKeyManager::HK_Next, wVKCode, wMod);
	m_HKMiniPlayer.GetHotKey(wVKCode, wMod);
	pHKM->SetHotKey(HotKeyManager::HK_ToggleMiniPlayer, wVKCode, wMod);
	m_HKTooltip.GetHotKey(wVKCode, wMod);
	pHKM->SetHotKey(HotKeyManager::HK_Tooltip, wVKCode, wMod);
	m_HKQuickSearch.GetHotKey(wVKCode, wMod);
	pHKM->SetHotKey(HotKeyManager::HK_QuickSearch, wVKCode, wMod);



	pHKM->EnableMultimediaHotKeys(IsDlgButtonChecked(IDC_CHK_USEMULTIMEDIAHOTKEYS));
}
