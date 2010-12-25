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
#include "TaggingPP.h"
#include "PrgAPi.h"

// CTaggingPP dialog

IMPLEMENT_DYNAMIC(CTaggingPP, CPropertyPage)

CTaggingPP::CTaggingPP(UINT nIDCaption)
: CTSPropertyPage(CTaggingPP::IDD, nIDCaption),
m_bNeedsUpdating(FALSE)
{

}

CTaggingPP::~CTaggingPP()
{
}

void CTaggingPP::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLD_RD_AUTO, m_rdAuto);
	DDX_Control(pDX, IDC_SLD_RD_MANUAL, m_rdManual);
	DDX_Control(pDX, IDC_LST_READ, m_lstRead);
	DDX_Control(pDX, IDC_LST_WRITE, m_lstWrite);
}


BEGIN_MESSAGE_MAP(CTaggingPP, CPropertyPage)
	ON_BN_CLICKED(IDC_CHK_ENABLETAGGING, &CTaggingPP::OnChange)
	ON_BN_CLICKED(IDC_CHK_TAGRATING, &CTaggingPP::OnChange)
	ON_BN_CLICKED(IDC_CHK_TAGLYRICS, &CTaggingPP::OnChange)
	ON_WM_HSCROLL()
	ON_LBN_SELCHANGE(IDC_LST_READ, &CTaggingPP::OnChange)
	ON_LBN_SELCHANGE(IDC_LST_WRITE, &CTaggingPP::OnChange)
END_MESSAGE_MAP()

void CTaggingPP::ApplyTranslation(ITranslation& translation)
{
	if (m_hWnd)
	{
		PrgAPI* pAPI = PRGAPI();
		SetDlgItemText(IDC_GR_TAGWRITER, pAPI->GetString(IDS_TAGWRITER));
		//SetDlgItemText(IDC_CHK_ENABLETAGGING, pAPI->GetString(IDS_ENABLED));
		SetDlgItemText(IDC_GR_SPECIALTAGSSUPPORT, pAPI->GetString(IDS_SPECIALTAGSSUPPORT));
		SetDlgItemText(IDC_CHK_TAGRATING, pAPI->GetString(IDS_RATING));
		SetDlgItemText(IDC_CHK_TAGLYRICS, pAPI->GetString(IDS_LYRICS));
		
		SetDlgItemText(IDC_GR_TAGREADER, pAPI->GetString(IDS_TAGREADER));
		TCHAR bf[500];
		_sntprintf(bf, 500, _T("%s (%s)"), 
			pAPI->GetString(IDS_READDEPTH),
			pAPI->GetString(IDS_AUTO));
		SetDlgItemText(IDC_ST_RD_AUTO, bf);
		_sntprintf(bf, 500, _T("%s (%s)"), 
			pAPI->GetString(IDS_READDEPTH),
			pAPI->GetString(IDS_MANUALLY));
		SetDlgItemText(IDC_ST_RD_MANUAL, bf);
	}
}
BOOL CTaggingPP::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	LoadTagManagerOptions();
	ApplyTranslation(*PRGAPI()->GetTranslationManager());
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM) PRGAPI()->GetFont(FONT_Dialogs)->m_hObject);



	return TRUE;  // return TRUE unless you set the focus to a control
}
void CTaggingPP::OnOK()
{
	SaveChanges();
	CPropertyPage::OnOK();
}

void CTaggingPP::SaveChanges()
{
	if (m_bNeedsUpdating)
	{
		m_bSomethingChanged = TRUE;
		SaveTagManagerOptions();
	}
}
void CTaggingPP::OnChange()
{
	if (IsWindowVisible())
	{
		m_bNeedsUpdating = TRUE;
		SetModified(TRUE);
		//SetControlState();
	}
}

void CTaggingPP::LoadTagManagerOptions()
{
	PrgAPI* pAPI = PRGAPI();
	//CheckDlgButton(IDC_CHK_ENABLETAGGING, pAPI->GetOption(OPT_TAG_Enabled));
	CheckDlgButton(IDC_CHK_TAGRATING, pAPI->GetOption(OPT_TAG_WriteRating));
	CheckDlgButton(IDC_CHK_TAGLYRICS, pAPI->GetOption(OPT_TAG_WriteLyrics));
	//SetControlState();

	//SetDlgItemText(IDC_EDT_RD_AUTO, pAPI->GetOption(OPT_TAG_SafetyOption));
	//SetDlgItemText(IDC_EDT_RD_MANUAL, pAPI->GetOption(OPT_TAG_SafetyOptionManual));
	m_rdManual.SetRange(0, 20);
	m_rdAuto.SetRange(0, 20);
	//::SendMessage(m_rdManual.m_hWnd, TBM_SETBUDDY, FALSE, (LPARAM)::GetDlgItem(m_hWnd, IDC_EDT_RD_MANUAL));
	//::SendMessage(m_rdAuto.m_hWnd, TBM_SETBUDDY, FALSE, (LPARAM)::GetDlgItem(m_hWnd, IDC_EDT_RD_AUTO));
	m_rdAuto.SetPos(pAPI->GetOption(OPT_TAG_SafetyOption) / 5);
	m_rdManual.SetPos(pAPI->GetOption(OPT_TAG_SafetyOptionManual) / 5);
	UpdateScrollers();

	m_lstRead.ResetContent();
	m_lstWrite.ResetContent();

	AddString(_T("mp3"), OPT_TAG_MP3);
	AddString(_T("ogg"), OPT_TAG_OGG);
	AddString(_T("oga"), OPT_TAG_OGA);
	AddString(_T("flac"), OPT_TAG_FLAC);
	AddString(_T("mpc"), OPT_TAG_MPC);
	AddString(_T("wv"), OPT_TAG_WV);
	AddString(_T("spx"), OPT_TAG_SPX);
	AddString(_T("tta"), OPT_TAG_TTA);
	AddString(_T("m4a/m4b/m4p/3g2"), OPT_TAG_M4A);
	AddString(_T("wma"), OPT_TAG_WMA);
	AddString(_T("aif"), OPT_TAG_AIF);
	AddString(_T("wav"), OPT_TAG_WAV);

}

void CTaggingPP::AddString(LPCTSTR text, INT option)
{
	INT item = m_lstRead.AddString(text);
	m_lstRead.SetCheck(item, ((PRGAPI()->GetOption((AppOptionsEnum) option) & 1) == 1) ? 1 : 0);
	m_lstRead.SetItemData(item, option);
	switch (option)
	{
	case OPT_TAG_WMA:
	case OPT_TAG_WAV:
		break;
	default:
		item = m_lstWrite.AddString(text);
		m_lstWrite.SetCheck(item, ((PRGAPI()->GetOption((AppOptionsEnum) option) & 2) == 2) ? 1 : 0);
		m_lstWrite.SetItemData(item, option);
	}
}

void CTaggingPP::SaveTagManagerOptions()
{
	PrgAPI* pAPI = PRGAPI();
	//pAPI->SetOption(OPT_TAG_Enabled, IsDlgButtonChecked(IDC_CHK_ENABLETAGGING));
	pAPI->SetOption(OPT_TAG_WriteRating, IsDlgButtonChecked(IDC_CHK_TAGRATING));
	pAPI->SetOption(OPT_TAG_WriteLyrics, IsDlgButtonChecked(IDC_CHK_TAGLYRICS));

	pAPI->SetOption(OPT_TAG_SafetyOption, m_rdAuto.GetPos() * 5);
	pAPI->SetOption(OPT_TAG_SafetyOptionManual, m_rdManual.GetPos() * 5);

	for (INT i = 0; i < m_lstRead.GetCount(); i++)
	{
		AppOptionsEnum opt = (AppOptionsEnum)m_lstRead.GetItemData(i);
		UINT optValue = pAPI->GetOption(opt);
		if (m_lstRead.GetCheck(i) == 0)
			optValue &= ~1;
		else
			optValue |= 1;
		pAPI->SetOption(opt, optValue);
	}
	for (INT i = 0; i < m_lstWrite.GetCount(); i++)
	{
		AppOptionsEnum opt = (AppOptionsEnum)m_lstRead.GetItemData(i);
		UINT optValue = pAPI->GetOption(opt);
		if (m_lstWrite.GetCheck(i) == 0)
			optValue &= ~2;
		else
			optValue |= 2;
		pAPI->SetOption(opt, optValue);
	}

}

//void CTaggingPP::SetControlState()
//{
//	//Tagging
//	BOOL chk = IsDlgButtonChecked(IDC_CHK_ENABLETAGGING);
//	GetDlgItem(IDC_CHK_TAGRATING)->EnableWindow(chk);
//	GetDlgItem(IDC_CHK_TAGLYRICS)->EnableWindow(chk);
//}

void CTaggingPP::UpdateScrollers()
{
	TCHAR bf[20];
	_itot(m_rdAuto.GetPos() * 5, bf, 10);
	SetDlgItemText(IDC_EDT_RD_AUTO, bf);
	_itot(m_rdManual.GetPos() * 5, bf, 10);
	SetDlgItemText(IDC_EDT_RD_MANUAL, bf);
	OnChange();
}
void CTaggingPP::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if (pScrollBar->m_hWnd == m_rdAuto.m_hWnd || pScrollBar->m_hWnd == m_rdManual.m_hWnd)
	{
		UpdateScrollers();
	}

	CTSPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}
