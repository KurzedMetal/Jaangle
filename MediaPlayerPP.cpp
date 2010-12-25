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
#include "MediaPlayerPP.h"
#include "TeenSpiritDlg.h"
#include "PrgAPi.h"
#include "MediaPlayer.h"

// CMediaPlayerPP dialog

IMPLEMENT_DYNAMIC(CMediaPlayerPP, CPropertyPage)

CMediaPlayerPP::CMediaPlayerPP(UINT nIDCaption)
	: CTSPropertyPage(CMediaPlayerPP::IDD, nIDCaption)
{

}

CMediaPlayerPP::~CMediaPlayerPP()
{
}

void CMediaPlayerPP::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_PLAYCONTROL, m_cmbPlayControl);
	//DDX_Control(pDX, IDC_SLDSTART, m_SldStart);
	DDX_Control(pDX, IDC_SLDEND, m_SldEnd);
	DDX_Control(pDX, IDC_CMB_DEFAULTPLAYERACTION, m_cmbDefaultPlayerAction);
}


BEGIN_MESSAGE_MAP(CMediaPlayerPP, CPropertyPage)
	ON_BN_CLICKED(IDC_CHK_SHOWVISUALIZATIONS, &CMediaPlayerPP::OnChange)
	ON_BN_CLICKED(IDC_CHK_AUTOPLAY, &CMediaPlayerPP::OnChange)
	ON_BN_CLICKED(IDC_CHK_REMTRACK, &CMediaPlayerPP::OnChange)
	ON_BN_CLICKED(IDC_CHK_REMTIME, &CMediaPlayerPP::OnChange)
	ON_BN_CLICKED(IDC_CHK_CFENABLED, &CMediaPlayerPP::OnChange)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_CMB_PLAYCONTROL, &CMediaPlayerPP::OnChange)
END_MESSAGE_MAP()

void CMediaPlayerPP::ApplyTranslation(ITranslation& translation)
{
	if (m_hWnd)
	{
		PrgAPI* pLang = PRGAPI();
		SetDlgItemText(IDC_GR_STARTUP, pLang->GetString(IDS_STARTUP));
		SetDlgItemText(IDC_CHK_AUTOPLAY, pLang->GetString(IDS_AUTOPLAY));
		SetDlgItemText(IDC_CHK_REMTRACK, pLang->GetString(IDS_REMEMBERLASTTRACK));
		SetDlgItemText(IDC_CHK_REMTIME, pLang->GetString(IDS_REMEMBERTIME));
		SetDlgItemText(IDC_GR_GENERAL, pLang->GetString(IDS_GENERAL));
		SetDlgItemText(IDC_ST_PLAYCONTROL, pLang->GetString(IDS_PLAYCONTROL));
		SetDlgItemText(IDC_ST_DEFAULTPLAYERACTION, pLang->GetString(IDS_DEFAULTPLAYERACTION));

		TCHAR bf[300];
		_sntprintf(bf, 300, _T("%s (%s)"), pLang->GetString(IDS_SAFEMODE), pLang->GetString(IDS_NEEDSPROGRAMRESATART));
		SetDlgItemText(IDC_CHK_SAFEMODE, bf);
		SetDlgItemText(IDC_GR_CROSSFADE, pLang->GetString(IDS_CROSSFADE));
		SetDlgItemText(IDC_CHK_CFENABLED, pLang->GetString(IDS_ENABLED));
		SetDlgItemText(IDC_ST_CFSTART, pLang->GetString(IDS_START));
		SetDlgItemText(IDC_ST_CFEND, pLang->GetString(IDS_END));


		//From LoadMPCtrlOptions
		m_cmbPlayControl.ResetContent();
		m_cmbPlayControl.AddString(pLang->GetString(IDS_NORMAL));
		m_cmbPlayControl.AddString(pLang->GetString(IDS_SHUFFLE));
		m_cmbPlayControl.AddString(pLang->GetString(IDS_LOOP));
		m_cmbPlayControl.AddString(pLang->GetString(IDS_AUTOCONTINUE));
		m_cmbPlayControl.SetCurSel(INT(pLang->GetMediaPlayer()->GetNextMode(NULL)));

		m_cmbDefaultPlayerAction.ResetContent();
		m_cmbDefaultPlayerAction.AddString(pLang->GetString(IDS_PLAY));
		m_cmbDefaultPlayerAction.AddString(pLang->GetString(IDS_ENQUEUE));
		m_cmbDefaultPlayerAction.AddString(pLang->GetString(IDS_INSERTAFTER));
		m_cmbDefaultPlayerAction.SetCurSel(pLang->GetOption(OPT_PL_DefaultAction) - 1);

	}
}
BOOL CMediaPlayerPP::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	ApplyTranslation(*PRGAPI()->GetTranslationManager());
	LoadMPCtrlOptions();
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM) PRGAPI()->GetFont(FONT_Dialogs)->m_hObject);
	return TRUE;  // return TRUE unless you set the focus to a control
}
void CMediaPlayerPP::OnChange()
{
	if (IsWindowVisible())
	{
		m_bNeedsUpdating = TRUE;
		SetModified(TRUE);
	}
}

void CMediaPlayerPP::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	OnChange();
}


void CMediaPlayerPP::OnOK()
{
	SaveChanges();
	CPropertyPage::OnOK();
}

void CMediaPlayerPP::SaveChanges()
{
	if (m_bNeedsUpdating)
	{
		m_bSomethingChanged = TRUE;
		SaveMPCtrlOptions();
	}
}

void CMediaPlayerPP::LoadMPCtrlOptions()
{
	PrgAPI* pAPI = PRGAPI();
	MediaPlayer* pPlayer = pAPI->GetMediaPlayer();
	m_cmbPlayControl.SetCurSel(INT(pPlayer->GetNextMode(NULL)));
	m_cmbDefaultPlayerAction.SetCurSel(pAPI->GetOption(OPT_PL_DefaultAction) - 1);

	CheckDlgButton(IDC_CHK_AUTOPLAY, pAPI->GetOption(OPT_PL_AutoPlay));
	CheckDlgButton(IDC_CHK_REMTRACK, pAPI->GetOption(OPT_PL_RememberLastTrack));
	CheckDlgButton(IDC_CHK_REMTIME, pAPI->GetOption(OPT_PL_RememberLastTime));

	CheckDlgButton(IDC_CHK_SAFEMODE, pAPI->GetOption(OPT_PL_UseFloatDSP) ? 0 : 1);

	CheckDlgButton(IDC_CHK_CFENABLED, pPlayer->GetEngineConfig(MECO_Crossfade));
	//m_SldStart.SetRange(0, 10000);
	//m_SldStart.SetPos(pMpe->GetConfig(MECO_CrossfadeAtStart));
	m_SldEnd.SetRange(1, 12);
	m_SldEnd.SetPos(pPlayer->GetEngineConfig(MECO_CrossfadeAtEnd) / 1000);

}

void CMediaPlayerPP::SaveMPCtrlOptions()
{
	int curSel = m_cmbPlayControl.GetCurSel();
	ASSERT(curSel > -1);
	if (curSel < 0)
		return;
	m_pPlayer->SetNextMode(MediaPlayerNextMode(curSel), -1);
	
	PrgAPI* pAPI = PRGAPI();
	MediaPlayer* pPlayer = pAPI->GetMediaPlayer();

	pAPI->SetOption(OPT_PL_AutoPlay, IsDlgButtonChecked(IDC_CHK_AUTOPLAY));
	pAPI->SetOption(OPT_PL_RememberLastTrack, IsDlgButtonChecked(IDC_CHK_REMTRACK));
	pAPI->SetOption(OPT_PL_RememberLastTime, IsDlgButtonChecked(IDC_CHK_REMTIME));

	pAPI->SetOption(OPT_PL_UseFloatDSP, IsDlgButtonChecked(IDC_CHK_SAFEMODE) ? 0 : 1);
	pAPI->SetOption(OPT_PL_UseSampleFloat, IsDlgButtonChecked(IDC_CHK_SAFEMODE) ? 0 : 1);

	pPlayer->SetEngineConfig(MECO_Crossfade, IsDlgButtonChecked(IDC_CHK_CFENABLED));
	//pMpe->SetConfig(MECO_CrossfadeAtStart, m_SldStart.GetPos());
	pPlayer->SetEngineConfig(MECO_CrossfadeAtEnd, m_SldEnd.GetPos() * 1000);

	pAPI->SetOption(OPT_PL_DefaultAction, m_cmbDefaultPlayerAction.GetCurSel() + 1);
}
