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
#include "SkinsPP.h"
#include "SkinManager.h"
#include "PrgAPI.h"
#include "ActionManager.h"
#include "XFolderDialog.h"
#include "TextFileDocument/textfile.h"
#include "SimpleEditorDlg.h"
#include "BitmapSaver.h"

IMPLEMENT_DYNAMIC(CSkinsPP, CPropertyPage)

#define UNM_CALLBACK	(WM_APP + 0x100)


CSkinsPP::CSkinsPP(UINT nIDCaption)
	: CTSPropertyPage(CSkinsPP::IDD, nIDCaption),
	m_bNeedsUpdating(FALSE),
	m_bSomethingChanged(FALSE)
{
}

CSkinsPP::~CSkinsPP()
{
}

void CSkinsPP::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMBSKINS, m_cmbSkins);
	DDX_Control(pDX, IDC_SKININFO, m_info);
	//DDX_Control(pDX, IDC_DOWNLOADSKIN, m_download);
	//DDX_Control(pDX, IDC_HELPSKIN, m_help);
	//DDX_Control(pDX, IDC_UPDATESKIN, m_updateSkin);
	//DDX_Control(pDX, IDC_OPENFOLDER, m_openFolder);
	DDX_Control(pDX, IDC_SCREENSHOT, m_screenShot);
}


BEGIN_MESSAGE_MAP(CSkinsPP, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_CMBSKINS, &CSkinsPP::OnChange)
	//ON_BN_CLICKED(IDC_DOWNLOADSKIN, &CSkinsPP::OnBnClickedDownload)
	//ON_BN_CLICKED(IDC_HELPSKIN, &CSkinsPP::OnBnClickedHelp)
	ON_BN_CLICKED(IDC_CREATESKIN, &CSkinsPP::OnBnClickedCreateskin)
	ON_BN_CLICKED(IDC_UPDATESKIN, &CSkinsPP::OnBnClickedUpdateSkin)
	ON_BN_CLICKED(IDC_OPENFOLDER, &CSkinsPP::OnBnClickedOpenfolder)
//	ON_STN_CLICKED(IDC_SCREENSHOT, &CSkinsPP::OnStnClickedScreenshot)
	ON_BN_CLICKED(IDC_CREATEPREVIEW, &CSkinsPP::OnBnClickedCreatepreview)
END_MESSAGE_MAP()

void CSkinsPP::ApplyTranslation(ITranslation& translation)
{
	if (m_hWnd)
	{
		PrgAPI* pLang = PRGAPI();

		SetDlgItemText(IDC_GRP_SELECT, pLang->GetString(IDS_ACTIVE));
		SetDlgItemText(IDC_GRP_OPTIONS, pLang->GetString(IDS_MOREOPTIONS));
		//m_download.SetWindowText(pLang->GetString(IDS_DOWNLOAD));
		//m_help.SetWindowText(pLang->GetString(IDS_HELP_S));
		SetDlgItemText(IDC_CREATESKIN, pLang->GetString(IDS_CREATE));
		SetDlgItemText(IDC_UPDATESKIN, pLang->GetString(IDS_UPDATE));
		SetDlgItemText(IDC_OPENFOLDER, pLang->GetString(IDS_OPENCONTAININGFOLDER));
		SetDlgItemText(IDC_CREATEPREVIEW, pLang->GetString(IDS_CREATEPREVIEW));
		RefreshInfo();
	}
}

void CSkinsPP::GetNameFromComboSelection(LPTSTR lngName, UINT bfLen)
{
	CString lngCode;
	m_cmbSkins.GetLBText(m_cmbSkins.GetCurSel(), lngCode);

	LPCTSTR fnd = _tcschr(lngCode, '/');
	lngName[0] = 0;
	if (fnd != NULL)
	{
		int copyChars = (fnd - lngCode) - 1;
		if (copyChars > 0 && copyChars < (int)bfLen)
		{
			_tcsncpy(lngName, lngCode, copyChars);
			lngName[(fnd - lngCode) - 1] = 0;
		}
	}
	else
		_tcsncpy(lngName, lngCode, bfLen);

}

void CSkinsPP::RefreshInfo()
{
	int curSel = m_cmbSkins.GetCurSel();
	if (curSel == -1)
	{
		m_info.SetWindowText(_T(""));
		return;
	}
	PrgAPI* pAPI = PRGAPI();
	CString wndText;
	CString line;
	SkinManager* pSM = pAPI->GetSkinManager();
	LPCTSTR ret = pSM->GetSkinInfo(curSel, ISkin::SI_Name);
	if (ret != NULL && ret[0] != 0)
	{
		line.Format(_T("<b>%s</b>: %s<br>"), pAPI->GetString(IDS_NAME), ret);
		wndText += line;
	}
	ret = pSM->GetSkinInfo(curSel, ISkin::SI_Version);
	if (ret != NULL && ret[0] != 0)
	{
		line.Format(_T("<b>%s</b>: %s<br>"), pAPI->GetString(IDS_VERSION), ret);
		wndText += line;
	}
	ret = pSM->GetSkinInfo(curSel, ISkin::SI_Author);
	if (ret != NULL && ret[0] != 0)
	{
		line.Format(_T("<b>%s</b>: %s<br>"), pAPI->GetString(IDS_AUTHOR), ret);
		wndText += line;
	}
	ret = pSM->GetSkinInfo(curSel, ISkin::SI_Email);
	if (ret != NULL && ret[0] != 0)
	{
		line.Format(_T("<b>%s</b>: <a href=\"mailto:%s\"><u><font color=\"#0000ff\">%s</font></u></a><br>"), 
			pAPI->GetString(IDS_EMAIL),
			ret, ret);
		wndText += line;
	}
	ret = pSM->GetSkinInfo(curSel, ISkin::SI_WebSite);
	if (ret != NULL && ret[0] != 0)
	{
		line.Format(_T("<b>%s</b>: <a href=\"%s\"><u><font color=\"#0000ff\">%s</font></u></a><br>"), 
			pAPI->GetString(IDS_WEBSITE),
			ret, ret);
		wndText += line;
	}
	ret = pSM->GetSkinInfo(curSel, ISkin::SI_Description);
	if (ret != NULL && ret[0] != 0)
	{
		line.Format(_T("<b>%s</b>: %s<br>"), pAPI->GetString(IDS_DESCRIPTION), ret);
		wndText += line;
	}
	m_info.SetWindowText(wndText);

	TCHAR bf[MAX_PATH];
	pSM->GetSkin().GetSkinPreviewPath(pSM->GetSkinPath(curSel), bf, MAX_PATH);
	if (PathFileExists(bf))
		m_screenShot.LoadFile(bf);
	else
		m_screenShot.ClearImage();
	GetDlgItem(IDC_CREATEPREVIEW)->EnableWindow(_tcsicmp(pSM->GetSkinPath(curSel), pSM->GetSkin().GetSkinPath()) == 0);
}


BOOL CSkinsPP::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	LoadOptions();
	PrgAPI* pAPI = PRGAPI();
	ApplyTranslation(*pAPI->GetTranslationManager());
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM) pAPI->GetFont(FONT_Dialogs)->m_hObject);
	//m_download.SetIcon(pAPI->GetIcon(ICO_Internet16));
	m_screenShot.SetBkColor(RGB(50,50,80));
	m_screenShot.GetDrawParams().zoomLockMode = GdiPlusPicDrawer::ZLM_FillArea;
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSkinsPP::OnChange()
{
	RefreshInfo();
	if (IsWindowVisible())
	{
		m_bNeedsUpdating = TRUE;
		SetModified(TRUE);
	}
}
void CSkinsPP::OnOK()
{
	SaveChanges();
	CPropertyPage::OnOK();
}

void CSkinsPP::SaveChanges()
{
	if (m_bNeedsUpdating)
	{
		m_bSomethingChanged = TRUE;
		SaveOptions();
		GetDlgItem(IDC_CREATEPREVIEW)->EnableWindow(TRUE);
	}
}

void CSkinsPP::LoadOptions()
{
	PrgAPI* pAPI = PRGAPI();
	SkinManager* pSM = pAPI->GetSkinManager();
	pSM->RefreshSkinsFolder();
	m_cmbSkins.ResetContent();
	//m_cmbSkins.AddItem(pAPI->GetString(IDS_DEFAULT));

	for (UINT i = 0; i < pSM->GetSkinsCount(); i++)
	{
		m_cmbSkins.AddItem(pSM->GetSkinInfo(i, ISkin::SI_Name), pSM->GetSkinInfo(i, ISkin::SI_Author));
		if (_tcsicmp(pSM->GetSkin().GetSkinPath(), pSM->GetSkinPath(i)) == 0)
			m_cmbSkins.SetCurSel(i);
	}

}


void CSkinsPP::SaveOptions()
{
	PrgAPI* pAPI = PRGAPI();
	SkinManager* pSM = pAPI->GetSkinManager();
	pSM->GetSkin().LoadSkin(pSM->GetSkinPath(m_cmbSkins.GetCurSel()));
	pSM->UpdateSkinnables(pSM->GetSkin());
}


//void CSkinsPP::OnBnClickedDownload()
//{
//	PRGAPI()->GoToSiteURL(PrgAPI::SA_Navigate, _T("download-skins"));
//}
//
//
//void CSkinsPP::OnBnClickedHelp()
//{
//	PRGAPI()->GoToSiteURL(PrgAPI::SA_Help, _T("help-skins"));
//}

void CSkinsPP::OnBnClickedCreateskin()
{
	PrgAPI* pAPI = PRGAPI();
	TSSkin& skin = pAPI->GetSkinManager()->GetSkin();
	CXFolderDialog dlg(pAPI->GetSkinManager()->GetSkinsFolder(), 0, this);
	pAPI->GetActionManager()->LocalizeXFolderDialog(dlg);
	if (dlg.DoModal() == IDOK)
	{
		if (skin.SaveSkin(dlg.GetPath()))
		{
			pAPI->GetActionManager()->OpenFolder(dlg.GetPath());
			LoadOptions();
		}
	}
}


void CSkinsPP::OnBnClickedUpdateSkin()
{
	PrgAPI* pAPI = PRGAPI();
	SkinManager* pSM = pAPI->GetSkinManager();
	LPCTSTR sPath = pSM->GetSkinPath(m_cmbSkins.GetCurSel());
	if (sPath == NULL)
		return;
	TCHAR skinPath[MAX_PATH];
	_tcsncpy(skinPath, sPath, MAX_PATH);

	TCHAR skinInfoPath[MAX_PATH];
	TSSkin::GetSkinInfoPath(skinPath, skinInfoPath, MAX_PATH);
	std::tstring fileContents;
	CTextFileRead fr(skinInfoPath);
	fr.Read(fileContents);
	fr.Close();
	if (fileContents.empty())
		return;
	
	CSimpleEditorDlg dlg(this);
	dlg.SetTitle(_T("Basic Skin Editor"));
	dlg.SetSubtitle(_T("You may copy this text to a different editor and then paste it back. Press 'save' when done to immediately see your changes."));
	dlg.SetText(fileContents.c_str());
	if (dlg.DoModal() == IDOK)
	{
		fileContents = dlg.GetText();
		CTextFileWrite fw(skinInfoPath, CTextFileBase::UNI16_LE);
		fw.Write(fileContents);
		fw.Close();
		pSM->LoadSkin(skinPath);
		LoadOptions();
		pSM->UpdateSkinnables();
		pAPI->MessageBox(m_hWnd, _T("You may share your creation by publishing it in the forum."), _T("Skin Updated"), MB_OK);
	}
}

void CSkinsPP::OnBnClickedOpenfolder()
{
	PrgAPI* pAPI = PRGAPI();
	pAPI->GetActionManager()->OpenFolder(pAPI->GetSkinManager()->GetSkinsFolder());
}


//void CSkinsPP::OnStnClickedScreenshot()
//{
//
//}

void CSkinsPP::OnBnClickedCreatepreview()
{
	SkinManager* pSM = PRGAPI()->GetSkinManager();
	TCHAR bf[MAX_PATH];
	pSM->GetSkin().GetSkinPreviewPath(pSM->GetSkin().GetSkinPath(), bf, MAX_PATH);

	BitmapSaver bs;
	bs.SetFormat(BitmapSaver::F_Png);
	if (bs.SaveScreenshot(bf, AfxGetMainWnd()->GetSafeHwnd(), GetParent()->GetSafeHwnd(), TRUE))
		m_screenShot.LoadFile(bf);
}
