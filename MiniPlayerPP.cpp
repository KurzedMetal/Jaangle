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
#include "MiniPlayerPP.h"
#include "PrgAPI.h"
#include "SkinManager.h"
#include "MiniPlayerDlg.h"
#include "ActionManager.h"

IMPLEMENT_DYNAMIC(CMiniPlayerPP, CPropertyPage)

CMiniPlayerPP::CMiniPlayerPP(UINT nIDCaption)
	: CTSPropertyPage(CMiniPlayerPP::IDD, nIDCaption),
	m_bNeedsUpdating(FALSE),
	m_bSomethingChanged(FALSE)
{
}

CMiniPlayerPP::~CMiniPlayerPP()
{
}

void CMiniPlayerPP::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLDTRANSPARENCY, m_sldTransparency);
	DDX_Control(pDX, IDC_SLD_MAXSIZE, m_sldMaxSize);
	//DDX_Control(pDX, IDC_CMB_VISIBILITY, m_cmbVisibility);


}


BEGIN_MESSAGE_MAP(CMiniPlayerPP, CPropertyPage)
	ON_BN_CLICKED(IDC_CHK_SHOWTOOLTIPS, &CMiniPlayerPP::OnChange)
	ON_BN_CLICKED(IDC_CHK_SHOWMOREINFO, &CMiniPlayerPP::OnChange)
	ON_BN_CLICKED(IDC_CHK_AUTOMATIC, &CMiniPlayerPP::OnChange)
	ON_BN_CLICKED(IDC_CHK_FORCEONTOP, &CMiniPlayerPP::OnChange)
	ON_WM_HSCROLL()
	//ON_CBN_SELCHANGE(IDC_CMB_VISIBILITY, &CMiniPlayerPP::OnChange)
END_MESSAGE_MAP()

void CMiniPlayerPP::ApplyTranslation(ITranslation& translation)
{
	if (m_hWnd)
	{
		PrgAPI* pLang = PRGAPI();
		SetDlgItemText(IDC_GRP_BEHAVIOUR, pLang->GetString(IDS_BEHAVIOUR));

		SetDlgItemText(IDC_CHK_AUTOMATIC, pLang->GetString(IDS_SHOWONMINIMIZEHIDEONRESTORE));
		SetDlgItemText(IDC_CHK_FORCEONTOP, pLang->GetString(IDS_FORCEONTOP));

		SetDlgItemText(IDC_GRP_APPEARANCE, pLang->GetString(IDS_APPEARANCE));
		SetDlgItemText(IDC_TRANSPARENCY, pLang->GetString(IDS_TRANSPARENCY));
		SetDlgItemText(IDC_MAXSIZE, pLang->GetString(IDS_MAXSIZE));

		SetDlgItemText(IDC_GRP_SHOW, pLang->GetString(IDS_SHOW));
		SetDlgItemText(IDC_CHK_SHOWTOOLTIPS, pLang->GetString(IDS_TOOLTIPS));
		SetDlgItemText(IDC_CHK_SHOWMOREINFO, pLang->GetString(IDS_EXTRAINFO));
	}
}


BOOL CMiniPlayerPP::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	ApplyTranslation(*PRGAPI()->GetTranslationManager());
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM) PRGAPI()->GetFont(FONT_Dialogs)->m_hObject);
	LoadOptions();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CMiniPlayerPP::OnChange()
{
	if (IsWindowVisible())
	{
		m_bNeedsUpdating = TRUE;
		SetModified(TRUE);
	}
}
void CMiniPlayerPP::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	OnChange();
}

void CMiniPlayerPP::OnOK()
{
	SaveChanges();
	CPropertyPage::OnOK();
}

void CMiniPlayerPP::SaveChanges()
{
	if (m_bNeedsUpdating)
	{
		m_bSomethingChanged = TRUE;
		SaveOptions();

	}
}




void CMiniPlayerPP::LoadOptions()
{
	PrgAPI* pAPI = PRGAPI();
	INT curSel = 0;
	CMiniPlayerDlg* pMPDlg = pAPI->GetMiniPlayerDlg(FALSE);
	CheckDlgButton(IDC_CHK_AUTOMATIC, pAPI->GetOption(OPT_MINIPLR_Automatic) != 0 ? BST_CHECKED : BST_UNCHECKED);
	m_sldTransparency.SetRange(20, 255);
	m_sldTransparency.SetTic(20);
	m_sldMaxSize.SetRange(100, 800);
	m_sldMaxSize.SetTic(100);
	if (pMPDlg != NULL)
	{
		CheckDlgButton(IDC_CHK_FORCEONTOP, pMPDlg->GetForceOnTop() ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(IDC_CHK_SHOWMOREINFO, pMPDlg->IsExtraInfoEnabled() ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(IDC_CHK_SHOWTOOLTIPS, pMPDlg->IsToolTipsEnabled() ? BST_CHECKED : BST_UNCHECKED);
		m_sldTransparency.SetPos(pMPDlg->GetTransparency());
		m_sldMaxSize.SetPos(pMPDlg->GetMaxSize());
	}
	else
	{
		CheckDlgButton(IDC_CHK_FORCEONTOP, pAPI->GetOption(OPT_MINIPLR_ForceOnTop) != 0 ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(IDC_CHK_SHOWMOREINFO, pAPI->GetOption(OPT_MINIPLR_ShowExtraInfo) ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(IDC_CHK_SHOWTOOLTIPS, pAPI->GetOption(OPT_MINIPLR_ShowTooltips) ? BST_CHECKED : BST_UNCHECKED);
		m_sldTransparency.SetPos(pAPI->GetOption(OPT_MINIPLR_Transparency));
		m_sldMaxSize.SetPos(pAPI->GetOption(OPT_MINIPLR_MaxSize));
	}

}

void CMiniPlayerPP::SaveOptions()
{
	PrgAPI* pAPI = PRGAPI();
	pAPI->SetOption(OPT_MINIPLR_Automatic, IsDlgButtonChecked(IDC_CHK_AUTOMATIC) ? 1 : 0);
	pAPI->SetOption(OPT_MINIPLR_ForceOnTop, IsDlgButtonChecked(IDC_CHK_FORCEONTOP) ? 1 : 0);
	pAPI->SetOption(OPT_MINIPLR_ShowExtraInfo, IsDlgButtonChecked(IDC_CHK_SHOWMOREINFO));
	pAPI->SetOption(OPT_MINIPLR_ShowTooltips, IsDlgButtonChecked(IDC_CHK_SHOWTOOLTIPS));
	pAPI->SetOption(OPT_MINIPLR_Transparency, m_sldTransparency.GetPos());
	pAPI->SetOption(OPT_MINIPLR_MaxSize, m_sldMaxSize.GetPos());
	CMiniPlayerDlg* pMPDlg = pAPI->GetMiniPlayerDlg(FALSE);
	if (pMPDlg != NULL)
	{
		pMPDlg->SetForceOnTop(IsDlgButtonChecked(IDC_CHK_FORCEONTOP) == BST_CHECKED);
		pMPDlg->EnableExtraInfo(IsDlgButtonChecked(IDC_CHK_SHOWMOREINFO) == BST_CHECKED);
		pMPDlg->EnableToolTips(IsDlgButtonChecked(IDC_CHK_SHOWTOOLTIPS) == BST_CHECKED);
		pMPDlg->SetTransparency(m_sldTransparency.GetPos());
		pMPDlg->SetMaxSize(m_sldMaxSize.GetPos());
	}
}
