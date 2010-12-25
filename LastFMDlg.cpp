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
#include "LastFMDlg.h"
#include "PrgAPI.h"
#include "cMD5.h"
#include "LastFMServices.h"


// CLastFMDlg dialog

IMPLEMENT_DYNAMIC(CLastFMDlg, CDialog)

CLastFMDlg::CLastFMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLastFMDlg::IDD, pParent)
{

}

CLastFMDlg::~CLastFMDlg()
{
}

void CLastFMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLastFMDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLastFMDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLastFMDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CMD_CHECKLOGIN, &CLastFMDlg::OnBnClickedCmdChecklogin)
END_MESSAGE_MAP()




BOOL CLastFMDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	PrgAPI* pAPI = PRGAPI();

	LastFMServices* pLFM = pAPI->GetLastFMServices();
	m_user = pLFM->GetUserName();
	m_pass = pLFM->GetMD5Password();

	SetDlgItemText(IDC_USER, pAPI->GetString(IDS_USERNAME));
	SetDlgItemText(IDC_PASS, pAPI->GetString(IDS_PASSWORD));

	SetDlgItemText(IDC_EDT_USER, m_user.c_str());
	if (!m_pass.empty())
		SetDlgItemText(IDC_EDT_PASS, _T("******"));

	SetDlgItemText(IDC_CMD_CHECKLOGIN, pAPI->GetString(IDS_CHECK));
	SetDlgItemText(IDOK, pAPI->GetString(IDS_OK));
	SetDlgItemText(IDCANCEL, pAPI->GetString(IDS_CANCEL_S));

	m_Header.SetIconHandle(pAPI->GetIcon(ICO_Main));
	m_Header.SetTitleText(pAPI->GetString(IDS_LASTFMSCROBBLER));
	TCHAR bf[500];
	_sntprintf(bf, 500, _T("%s - %s"), pAPI->GetString(IDS_USERNAME), pAPI->GetString(IDS_PASSWORD));
	m_Header.SetDescText(bf);
	m_Header.SetTitleFont(pAPI->GetFont(FONT_DialogsBigBold));
	m_Header.SetDescFont(pAPI->GetFont(FONT_Dialogs));
	m_Header.Init(this);
	m_Header.MoveCtrls(this);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLastFMDlg::OnBnClickedOk()
{
	PrgAPI* pAPI = PRGAPI();
	LastFMServices* pLFM = pAPI->GetLastFMServices();
	UpdateUserPass();
	pLFM->SetUserName(m_user.c_str());
	pLFM->SetMD5Password(m_pass.c_str());
	OnOK();
}

void CLastFMDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CLastFMDlg::OnBnClickedCmdChecklogin()
{
	UpdateUserPass();
	PrgAPI* pAPI = PRGAPI();
	if (!m_user.empty() && !m_pass.empty())
	{
		LastFMServices lfm;
		lfm.Init(pAPI->GetInternetHandle());
		lfm.SetUserName(m_user.c_str());
		lfm.SetMD5Password(m_pass.c_str());
		if (lfm.CheckLogin())
		{
			pAPI->MessageBox(m_hWnd, IDS_SUCCESS);
			return;
		}
	}
	pAPI->MessageBox(m_hWnd, IDS_FAILURE);

}

void CLastFMDlg::UpdateUserPass()
{
	TCHAR bf[30];
	GetDlgItemText(IDC_EDT_USER, bf, 30);
	m_user = bf;
	GetDlgItemText(IDC_EDT_PASS, bf, 30);
	if (bf[0] == 0)
		m_pass.clear();
	else if (bf[0] != '*')
	{
		cMD5 md5;
		m_pass = CA2CT(_strlwr(md5.CalcMD5FromString(CT2CA(bf))));
	}

}
