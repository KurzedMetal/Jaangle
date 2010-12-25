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
#include "MainOptionsPage.h"
#include "PrgAPI.h"
#include "SkinManager.h"
#include "LastFMDlg.h"
#include "ConfigurationManager.h"

IMPLEMENT_DYNAMIC(CMainOptionsPage, CPropertyPage)

CMainOptionsPage::CMainOptionsPage(UINT nIDCaption)
	: CTSPropertyPage(CMainOptionsPage::IDD, nIDCaption),
	m_bNeedsUpdating(FALSE),
	m_bSomethingChanged(FALSE)
{
}

CMainOptionsPage::~CMainOptionsPage()
{
}

void CMainOptionsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMD_LASTFM, m_cmdLastFM);
	DDX_Control(pDX, IDC_CMD_ONLINESERVICES, m_cmdMusicStory);
}


BEGIN_MESSAGE_MAP(CMainOptionsPage, CPropertyPage)
	//MainDlg
	ON_CBN_SELCHANGE(IDC_CMBLANGUAGES, &CMainOptionsPage::OnChange)
	ON_CBN_SELCHANGE(IDC_CMBSKINS, &CMainOptionsPage::OnChange)
	ON_BN_CLICKED(IDC_CHK_MIN2TRAY, &CMainOptionsPage::OnChange)
	ON_BN_CLICKED(IDC_CHK_AUTOUPDDB, &CMainOptionsPage::OnChange)
	ON_EN_CHANGE(IDC_EDT_AUTOUPDDB, &CMainOptionsPage::OnChange)
	ON_BN_CLICKED(IDC_CHK_MSNNOWPLAYING, &CMainOptionsPage::OnChange)
	ON_BN_CLICKED(IDC_CHK_LASTFMSCROBBLING, &CMainOptionsPage::OnChange)
	ON_BN_CLICKED(IDC_CHK_TSONLINESERVICES, &CMainOptionsPage::OnChange)
	ON_BN_CLICKED(IDC_CHK_CHECKFORUPDATES, &CMainOptionsPage::OnChange)
	ON_BN_CLICKED(IDC_CHK_FORCECLEARTYPEFONTS, &CMainOptionsPage::OnChange)
	ON_EN_CHANGE(IDC_EDTNUMDAYS, &CMainOptionsPage::OnChange)
	ON_BN_CLICKED(IDC_CHK_SHOWTOOLTIPSONMEDIACHANGED, &CMainOptionsPage::OnChange)
	ON_BN_CLICKED(IDC_CHK_DEBUGGING, &CMainOptionsPage::OnChange)

	ON_BN_CLICKED(IDC_CMD_LASTFM, &CMainOptionsPage::OnBnClickedCmdLastfm)
	ON_BN_CLICKED(IDC_CMD_ONLINESERVICES, &CMainOptionsPage::OnBnClickedCmdOnlineservices)

END_MESSAGE_MAP()

void CMainOptionsPage::ApplyTranslation(ITranslation& translation)
{
	if (m_hWnd)
	{
		PrgAPI* pLang = PRGAPI();
		SetDlgItemText(IDC_GR_APPLICATION, pLang->GetString(IDS_APPLICATION));
		SetDlgItemText(IDC_CHK_MIN2TRAY, pLang->GetString(IDS_MINIMIZETOTRAY));
		SetDlgItemText(IDC_CHK_AUTOUPDDB, pLang->GetString(IDS_AUTOUPDDB));
		SetDlgItemText(IDC_CHK_CHECKFORUPDATES, pLang->GetString(IDS_CHECKFORUPDATES));
		SetDlgItemText(IDC_CHK_LASTFMSCROBBLING, pLang->GetString(IDS_LASTFMSCROBBLER));
		SetDlgItemText(IDC_CHK_TSONLINESERVICES, pLang->GetString(IDS_TSONLINESERVICES));
		//SetDlgItemText(IDC_CMD_LASTFM, pLang->GetString(IDS_OPTIONS));
		SetDlgItemText(IDC_CHK_MSNNOWPLAYING, pLang->GetString(IDS_MESSENGERNOWPLAYING));
		SetDlgItemText(IDC_CHK_FORCECLEARTYPEFONTS, pLang->GetString(IDS_FORCECLEARTYPEFONTS));
		SetDlgItemText(IDC_ST_NEWSIGN, pLang->GetString(IDS_NEWDEFINITION));
		SetDlgItemText(IDC_CHK_SHOWTOOLTIPSONMEDIACHANGED, pLang->GetString(IDS_SHOWTOOLTIPONMEDIACHANGED));
		SetDlgItemText(IDC_CHK_DEBUGGING, pLang->GetString(IDS_DEBUGGING));
	}
}


BOOL CMainOptionsPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	LoadMainDlgOptions();
	ApplyTranslation(*PRGAPI()->GetTranslationManager());
	PrgAPI* pAPI = PRGAPI();
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM) pAPI->GetFont(FONT_Dialogs)->m_hObject);
	
	//SetDlgItemText(IDC_CHK_TSONLINESERVICES, pLang->GetString(IDS_TSONLINESERVICES));
	m_cmdLastFM.ApplyPredifinedButtonsStyle(FlexButton::PBS_HotButton);
	m_cmdLastFM.SetComponents(FlexButton::COMP_Image);
	m_cmdLastFM.SetColor(FlexButton::COL_Bk, RGB(180,180,180));
	m_cmdLastFM.SetIcon(pAPI->GetIcon(ICO_Options16), 16, 16);
	m_cmdLastFM.ApplyPredifinedButtonsStyle(FlexButton::PBS_HotButton);
	m_cmdLastFM.SetComponents(FlexButton::COMP_Image);
	m_cmdMusicStory.SetColor(FlexButton::COL_Bk, RGB(180,180,180));
	m_cmdMusicStory.SetIcon(pAPI->GetIcon(ICO_Internet16), 16, 16);



	return TRUE;  // return TRUE unless you set the focus to a control
}

void CMainOptionsPage::OnChange()
{
	if (IsWindowVisible())
	{
		GetDlgItem(IDC_EDT_AUTOUPDDB)->EnableWindow(IsDlgButtonChecked(IDC_CHK_AUTOUPDDB));
		m_bNeedsUpdating = TRUE;
		SetModified(TRUE);
		//SetControlState();
	}
}

void CMainOptionsPage::OnOK()
{
	SaveChanges();
	CPropertyPage::OnOK();
}

void CMainOptionsPage::SaveChanges()
{
	if (m_bNeedsUpdating)
	{
		m_bSomethingChanged = TRUE;
		SaveMainDlgOptions();
	}
}




void CMainOptionsPage::LoadMainDlgOptions()
{
	PrgAPI* pAPI = PRGAPI();
	CheckDlgButton(IDC_CHK_MIN2TRAY, pAPI->GetOption(OPT_GEN_MinimizeToTray));
	BOOL bEnabled = pAPI->GetOption(OPT_GEN_AutoUpdateDB);
	INT iDelay = pAPI->GetOption(OPT_GEN_AutoUpdateDBDelay);
	CheckDlgButton(IDC_CHK_AUTOUPDDB, bEnabled ? BST_CHECKED : BST_UNCHECKED);
	GetDlgItem(IDC_EDT_AUTOUPDDB)->EnableWindow(bEnabled);
	TCHAR bf[50];
	_itot(iDelay, bf, 10);
	GetDlgItem(IDC_EDT_AUTOUPDDB)->SetWindowText(bf);

	CheckDlgButton(IDC_CHK_CHECKFORUPDATES, pAPI->GetOption(OPT_GEN_CheckForUpdates) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_MSNNOWPLAYING, pAPI->GetOption(OPT_GEN_NotifyMSN) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_LASTFMSCROBBLING, pAPI->GetOption(OPT_GEN_LastFMScrobbler) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_TSONLINESERVICES, pAPI->GetOption(OPT_GEN_UseNativeOnlineServices) ? BST_CHECKED : BST_UNCHECKED);

	CheckDlgButton(IDC_CHK_FORCECLEARTYPEFONTS, pAPI->GetOption(OPT_GEN_ForceClearTypeFonts) ? BST_CHECKED : BST_UNCHECKED);

	INT val = pAPI->GetOption(OPT_GEN_DaysMeansNew);
	_itot(val, bf, 10);
	if (val<0) val = 0;
	if (val>500) val = 500;
	SetDlgItemText(IDC_EDTNUMDAYS, bf);

	CheckDlgButton(IDC_CHK_SHOWTOOLTIPSONMEDIACHANGED, pAPI->GetOption(OPT_GEN_ShowToolTipOnMediaChanged) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_DEBUGGING, pAPI->GetOption(OPT_GEN_Debugging) ? BST_CHECKED : BST_UNCHECKED);
}

void CMainOptionsPage::SaveMainDlgOptions()
{
	PrgAPI* pAPI = PRGAPI();
	pAPI->SetOption(OPT_GEN_MinimizeToTray, IsDlgButtonChecked(IDC_CHK_MIN2TRAY));

	TCHAR bf[50];
	GetDlgItem(IDC_EDT_AUTOUPDDB)->GetWindowText(bf, 50);
	INT iDelay = _ttoi(bf);
	pAPI->SetOption(OPT_GEN_AutoUpdateDB, IsDlgButtonChecked(IDC_CHK_AUTOUPDDB));
	if (iDelay < 5)
		iDelay = 5;//Sanity (delay) check
	pAPI->SetOption(OPT_GEN_AutoUpdateDBDelay, iDelay);

	pAPI->SetOption(OPT_GEN_CheckForUpdates, IsDlgButtonChecked(IDC_CHK_CHECKFORUPDATES));
	pAPI->SetOption(OPT_GEN_NotifyMSN, IsDlgButtonChecked(IDC_CHK_MSNNOWPLAYING));
	pAPI->SetOption(OPT_GEN_LastFMScrobbler, IsDlgButtonChecked(IDC_CHK_LASTFMSCROBBLING));
	pAPI->SetOption(OPT_GEN_UseNativeOnlineServices, IsDlgButtonChecked(IDC_CHK_TSONLINESERVICES));


	INT prevValue = pAPI->GetOption(OPT_GEN_ForceClearTypeFonts);
	INT newValue = IsDlgButtonChecked(IDC_CHK_FORCECLEARTYPEFONTS) == BST_CHECKED ? 1 : 0;
	if (prevValue != newValue)
	{
		pAPI->SetOption(OPT_GEN_ForceClearTypeFonts, newValue);
		pAPI->GetSkinManager()->GetSkin().SetFontQuality(newValue == 1 ? CLEARTYPE_QUALITY : DEFAULT_QUALITY);
		pAPI->GetSkinManager()->UpdateSkinnables();
	}

	GetDlgItemText(IDC_EDTNUMDAYS, bf, 20);
	bf[19] = 0;
	INT newVal = _ttoi(bf);
	pAPI->SetOption(OPT_GEN_DaysMeansNew, newVal);

	
	pAPI->SetOption(OPT_GEN_ShowToolTipOnMediaChanged, IsDlgButtonChecked(IDC_CHK_SHOWTOOLTIPSONMEDIACHANGED));
	pAPI->SetOption(OPT_GEN_Debugging, IsDlgButtonChecked(IDC_CHK_DEBUGGING));

	pAPI->GetConfigurationManager()->UpdateConfigurables();

}

void CMainOptionsPage::OnBnClickedCmdLastfm()
{
	CLastFMDlg dlg;
	dlg.DoModal();
}

void CMainOptionsPage::OnBnClickedCmdOnlineservices()
{
	PRGAPI()->GoToSiteURL(PrgAPI::SA_Navigate, _T("tsonlineservices"));
}
