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
#include "InfoRetrievalPP.h"
#include "InfoCtrl.h"
#include "PrgApi.h"
#include "LocalPictureManager.h"

// CInfoRetrievalPP dialog

IMPLEMENT_DYNAMIC(CInfoRetrievalPP, CTSPropertyPage)

CInfoRetrievalPP::CInfoRetrievalPP(UINT nIDCaption):
CTSPropertyPage(CInfoRetrievalPP::IDD, nIDCaption),
m_bSomethingChanged(FALSE)
{
}
CInfoRetrievalPP::~CInfoRetrievalPP()
{
}


void CInfoRetrievalPP::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInfoRetrievalPP, CPropertyPage)
	ON_BN_CLICKED(IDC_CHK_DLARTPIC, &CInfoRetrievalPP::OnChange)
	ON_BN_CLICKED(IDC_CHK_DLARTBIO, &CInfoRetrievalPP::OnChange)
	ON_BN_CLICKED(IDC_CHK_DLALBPIC, &CInfoRetrievalPP::OnChange)
	ON_BN_CLICKED(IDC_CHK_DLALBREV, &CInfoRetrievalPP::OnChange)
	ON_BN_CLICKED(IDC_CHK_DLARTPIC, &CInfoRetrievalPP::OnChange)

END_MESSAGE_MAP()

void CInfoRetrievalPP::ApplyTranslation(ITranslation& translation)
{
	if (m_hWnd)
	{
		PrgAPI* pLang = PRGAPI();
		//SetDlgItemText(IDC_INTERNETOPTIONS, pLang->GetString(IDS_INTERNETOPTIONS));
		//SetDlgItemText(IDC_IMPERSONATE, pLang->GetString(IDS_IMPERSONATEBROWSER));
		//SetDlgItemText(IDC_USEDEFAULTPROXY, pLang->GetString(IDS_USEDEFAULTPROXY));
		SetDlgItemText(IDC_GRPAUTOMATEDDOWNLOADER, pLang->GetString(IDS_AUTOMATEDDOWNLOADER));

		SetDlgItemText(IDC_ST_ACTIVATIONDELAY, pLang->GetString(IDS_ACTIVATIONDELAY));
		SetDlgItemText(IDC_ST_SEC, pLang->GetString(IDS_SEC));
		SetDlgItemText(IDC_ST_DEFPROVIDER, pLang->GetString(IDS_PROVIDER));

		SetDlgItemText(IDC_CHK_DLARTBIO, pLang->GetString(IDS_ARTISTBIO));
		SetDlgItemText(IDC_CHK_DLARTPIC, pLang->GetString(IDS_ARTISTPICTURE));

		SetDlgItemText(IDC_CHK_DLALBREV, pLang->GetString(IDS_ALBUMREVIEW));
		SetDlgItemText(IDC_CHK_DLALBPIC, pLang->GetString(IDS_ALBUMPICTURE));

		SetDlgItemText(IDC_CHK_DLTRALYR, pLang->GetString(IDS_LYRICS));

		SetDlgItemText(IDC_CHK_USEFOLDERIMAGES, pLang->GetString(IDS_FOLDERIMAGES));
	}
}

BOOL CInfoRetrievalPP::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	LoadNetSessionOptions();
	LoadInfoMgrOptions();

	ApplyTranslation(*PRGAPI()->GetTranslationManager());
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM) PRGAPI()->GetFont(FONT_Dialogs)->m_hObject);
	return TRUE;  // return TRUE unless you set the focus to a control
}
void CInfoRetrievalPP::OnChange()
{
	if (IsWindowVisible())
	{
		m_bNeedsUpdating = TRUE;
		SetModified(TRUE);
	}
}

void CInfoRetrievalPP::OnOK()
{
	SaveChanges();
	CPropertyPage::OnOK();
}

void CInfoRetrievalPP::SaveChanges()
{
	if (m_bNeedsUpdating)
	{
		m_bSomethingChanged = TRUE;
		SaveNetSessionOptions();
		SaveInfoMgrOptions();
	}
}

void CInfoRetrievalPP::LoadNetSessionOptions()
{
	//No Options Yet
}

void CInfoRetrievalPP::SaveNetSessionOptions()
{
	//No Options Yet
}

void CInfoRetrievalPP::LoadInfoMgrOptions()
{
	PrgAPI* pAPI = PRGAPI();
	TCHAR bf[20];
	_itot(pAPI->GetOption(OPT_IM_AutoDlActivationDelay), bf, 10);
	SetDlgItemText(IDC_EDT_DELAY, bf);
	CheckDlgButton(IDC_CHK_DLARTBIO, pAPI->GetOption(OPT_IM_AutoDlArtistBio));
	CheckDlgButton(IDC_CHK_DLARTPIC, pAPI->GetOption(OPT_IM_AutoDlArtistPicture));
	CheckDlgButton(IDC_CHK_DLALBREV, pAPI->GetOption(OPT_IM_AutoDlAlbumReview));
	CheckDlgButton(IDC_CHK_DLALBPIC, pAPI->GetOption(OPT_IM_AutoDlAlbumPicture));
	CheckDlgButton(IDC_CHK_DLTRALYR, pAPI->GetOption(OPT_IM_AutoDlLyrics));

	CheckDlgButton(IDC_CHK_USEFOLDERIMAGES, pAPI->GetOption(OPT_IM_UseFolderImages));
}

void CInfoRetrievalPP::SaveInfoMgrOptions()
{
	PrgAPI* pAPI = PRGAPI();
	TCHAR bf[20];
	GetDlgItemText(IDC_EDT_DELAY, bf, 19);
	bf[19] = 0;
	INT delay = _ttoi(bf);
	if (delay < 1)
		delay = 1;
	if (delay > 60)
		delay = 60;
	pAPI->SetOption(OPT_IM_AutoDlActivationDelay, delay);
	pAPI->SetOption(OPT_IM_AutoDlArtistBio, IsDlgButtonChecked(IDC_CHK_DLARTBIO));
	pAPI->SetOption(OPT_IM_AutoDlArtistPicture, IsDlgButtonChecked(IDC_CHK_DLARTPIC));
	pAPI->SetOption(OPT_IM_AutoDlAlbumReview, IsDlgButtonChecked(IDC_CHK_DLALBREV));
	pAPI->SetOption(OPT_IM_AutoDlAlbumPicture, IsDlgButtonChecked(IDC_CHK_DLALBPIC));
	pAPI->SetOption(OPT_IM_AutoDlLyrics, IsDlgButtonChecked(IDC_CHK_DLTRALYR));

	pAPI->SetOption(OPT_IM_UseFolderImages, IsDlgButtonChecked(IDC_CHK_USEFOLDERIMAGES));
	pAPI->GetLocalPictureManager()->EnableFolderImages(IsDlgButtonChecked(IDC_CHK_USEFOLDERIMAGES));
}
