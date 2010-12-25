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
#include "LanguagesPP.h"
#include "TranslationManager.h"
#include "PrgAPI.h"
#include "SimpleEditorDlg.h"
#include "TextFileDocument/textfile.h"
#include "ActionManager.h"
#include "InputBox.h"

IMPLEMENT_DYNAMIC(CLanguagesPP, CPropertyPage)

#define UNM_CALLBACK	(WM_APP + 0x100)


CLanguagesPP::CLanguagesPP(UINT nIDCaption)
	: CTSPropertyPage(CLanguagesPP::IDD, nIDCaption),
	m_bNeedsUpdating(FALSE),
	m_bSomethingChanged(FALSE)
{
}

CLanguagesPP::~CLanguagesPP()
{
}

void CLanguagesPP::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMBLANGUAGES, m_cmbTranslations);
	DDX_Control(pDX, IDC_LANGINFO, m_info);
	//DDX_Control(pDX, IDC_DOWNLOADLNG, m_download);
	//DDX_Control(pDX, IDC_CREATELNG, m_create);
	//DDX_Control(pDX, IDC_UPDATELNG, m_update);
	//DDX_Control(pDX, IDC_HELPLNG, m_help);
}


BEGIN_MESSAGE_MAP(CLanguagesPP, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_CMBLANGUAGES, &CLanguagesPP::OnChange)
	ON_BN_CLICKED(IDC_CREATELNG, &CLanguagesPP::OnBnClickedCreate)
	//ON_BN_CLICKED(IDC_DOWNLOADLNG, &CLanguagesPP::OnBnClickedDownload)
	//ON_BN_CLICKED(IDC_HELPLNG, &CLanguagesPP::OnBnClickedHelp)
	ON_BN_CLICKED(IDC_UPDATELNG, &CLanguagesPP::OnBnClickedUpdate)
	ON_BN_CLICKED(IDC_OPENFOLDER, &CLanguagesPP::OnBnClickedOpenfolder)
	ON_BN_CLICKED(IDC_DEBUG, &CLanguagesPP::OnBnClickedDebug)
END_MESSAGE_MAP()

void CLanguagesPP::ApplyTranslation(ITranslation& translation)
{
	if (m_hWnd)
	{
		PrgAPI* pLang = PRGAPI();

		SetDlgItemText(IDC_GRP_ACTIVE, pLang->GetString(IDS_ACTIVE));

		SetDlgItemText(IDC_GRP_OPTIONS, pLang->GetString(IDS_MOREOPTIONS));
		//m_download.SetWindowText(pLang->GetString(IDS_DOWNLOAD));
		SetDlgItemText(IDC_CREATELNG, pLang->GetString(IDS_CREATE));
		SetDlgItemText(IDC_UPDATELNG, pLang->GetString(IDS_UPDATE));
		//m_help.SetWindowText(pLang->GetString(IDS_HELP_S));
		SetDlgItemText(IDC_OPENFOLDER, pLang->GetString(IDS_OPENCONTAININGFOLDER));
		SetDlgItemText(IDC_DEBUG, pLang->GetString(IDS_DEBUGGING));
		RefreshInfo();
	}
}

//void CLanguagesPP::GetNameFromComboSelection(LPTSTR lngName, UINT bfLen)
//{
//	CString lngCode;
//	m_cmbLanguages.GetLBText(m_cmbLanguages.GetCurSel(), lngCode);
//
//	LPCTSTR fnd = _tcschr(lngCode, '/');
//	lngName[0] = 0;
//	if (fnd != NULL)
//	{
//		int copyChars = (fnd - lngCode) - 1;
//		if (copyChars > 0 && copyChars < (int)bfLen)
//		{
//			_tcsncpy(lngName, lngCode, copyChars);
//			lngName[(fnd - lngCode) - 1] = 0;
//		}
//	}
//	else
//		_tcsncpy(lngName, lngCode, bfLen);
//
//}

void CLanguagesPP::RefreshInfo()
{
	//langInfo.Name = _T("English");
	//langInfo.Description = _T("Default Language");
	//LanguageInfo* pInfo = &langInfo;//&pLM->GetDefaultLanguage()->GetLanguageInfo();
	int curSel = m_cmbTranslations.GetCurSel();
	GetDlgItem(IDC_UPDATELNG)->EnableWindow(curSel > 0);
	GetDlgItem(IDC_DEBUG)->EnableWindow(curSel > 0);
	PrgAPI* pAPI = PRGAPI();
	if (curSel == 0)
	{
		m_info.SetWindowText(pAPI->GetString(IDS_DEFAULT));
		return;
	}


	//if (_tcscmp(key, LNG_Name) == 0)
	//	ti = ITranslation::TI_Name;
	//else if (_tcscmp(key, LNG_LocalizedName) == 0)
	//	ti = ITranslation::TI_LocalizedName;
	//else if (_tcscmp(key, LNG_AuthorName) == 0)
	//	ti = ITranslation::TI_Author;
	//else if (_tcscmp(key, LNG_Email) == 0)
	//	ti = ITranslation::TI_Email;
	//else if (_tcscmp(key, LNG_Description) == 0)
	//	ti = ITranslation::TI_Description;
	//else if (_tcscmp(key, LNG_WebSite) == 0)
	//	ti = ITranslation::TI_WebSite;
	//else if (_tcscmp(key, LNG_Version) == 0)
	//	ti = ITranslation::TI_Version;
	//else if (_tcscmp(key, LNG_AutoUpdateURL) == 0)
	//	ti = ITranslation::TI_AutoUpdateURL;
	//else if (_tcscmp(key, LNG_AppVersion) == 0)
	//	ti = ITranslation::TI_AppVersion;


	INT idx = curSel - 1;
	TranslationManager* pLM = pAPI->GetTranslationManager();
	CString wndText;
	CString line;
	LPCTSTR ret = pLM->GetTranslationInfo(idx, ITranslation::TI_Author);
	if (ret != NULL && ret[0] != 0)
	{
		line.Format(_T("<b>%s</b>: %s<br>"), pAPI->GetString(IDS_AUTHOR), ret);
		wndText += line;
	}
	ret = pLM->GetTranslationInfo(idx, ITranslation::TI_Email);
	if (ret != NULL && ret[0] != 0)
	{
		line.Format(_T("<b>%s</b>: <u><font color=\"#0000ff\">%s</font></u><br>"), 
			pAPI->GetString(IDS_EMAIL), ret);
		wndText += line;
	}
	ret = pLM->GetTranslationInfo(idx, ITranslation::TI_WebSite);
	if (ret != NULL && ret[0] != 0)
	{
		line.Format(_T("<b>%s</b>: <a href=\"%s\"><u><font color=\"#0000ff\">%s</font></u></a><br>"), 
			pAPI->GetString(IDS_WEBSITE),
			ret, ret);
		wndText += line;
	}
	ret = pLM->GetTranslationInfo(idx, ITranslation::TI_Version);
	if (ret != NULL && ret[0] != 0)
	{
		line.Format(_T("<b>%s</b>: %s<br>"), pAPI->GetString(IDS_VERSION), ret);
		wndText += line;
	}
	ret = pLM->GetTranslationInfo(idx, ITranslation::TI_AppVersion);
	if (ret != NULL && ret[0] != 0)
	{
		line.Format(_T("<b>%s</b>: %s<br>"), pAPI->GetString(IDS_APPLICATION), ret);
		wndText += line;
	}
	ret = pLM->GetTranslationInfo(idx, ITranslation::TI_Description);
	if (ret != NULL && ret[0] != 0)
	{
		line.Format(_T("<b>%s</b>: %s<br>"), pAPI->GetString(IDS_DESCRIPTION), ret);
		wndText += line;
	}
	if (curSel > 0)
	{
		const TranslationComparison* pComp = pLM->GetTranslationComparison(curSel - 1);
		if (pComp != NULL)
		{
			line.Format(_T("<b>%s</b>: %d%% (%d / %d)"), 
				pAPI->GetString(IDS_STATISTICS), 
				(pComp->firstCount - pComp->notInSecond) * 100 / pComp->firstCount, 
				pComp->firstCount - pComp->notInSecond, 
				pComp->firstCount);
			wndText += line;
		}
	}
	m_info.SetWindowText(wndText);
}


BOOL CLanguagesPP::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	LoadOptions();
	ApplyTranslation(*PRGAPI()->GetTranslationManager());
	m_info.SetBkColor(GetSysColor(COLOR_WINDOW));
	PrgAPI* pAPI = PRGAPI();
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM) pAPI->GetFont(FONT_Dialogs)->m_hObject);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CLanguagesPP::OnChange()
{
	RefreshInfo();
	if (IsWindowVisible())
	{
		m_bNeedsUpdating = TRUE;
		SetModified(TRUE);
	}
}
void CLanguagesPP::OnOK()
{
	SaveChanges();
	CPropertyPage::OnOK();
}

void CLanguagesPP::SaveChanges()
{
	if (m_bNeedsUpdating)
	{
		m_bSomethingChanged = TRUE;
		SaveOptions();
	}
}

void CLanguagesPP::LoadOptions()
{
	PrgAPI* pAPI = PRGAPI();
	TranslationManager* pLM = pAPI->GetTranslationManager();
	pLM->RefreshTranslationsFolder();
	m_cmbTranslations.ResetContent();
	m_cmbTranslations.AddItem(_T("English"), _T("(System default)"));
	m_cmbTranslations.SetCurSel(0);

	for (UINT i = 0; i < pLM->GetTranslationsCount(); i++)
	{
		TCHAR bf[500];
		LPCTSTR ret = pLM->GetTranslationInfo(i, ITranslation::TI_LocalizedName);
		if (ret != NULL && ret[0] != 0)
		{
			_sntprintf(bf, 500, _T("%s / %s"), 
				pLM->GetTranslationInfo(i, ITranslation::TI_Name), 
				pLM->GetTranslationInfo(i, ITranslation::TI_LocalizedName));
			ret = bf;
		}
		else
			ret = pLM->GetTranslationInfo(i, ITranslation::TI_Name);
		m_cmbTranslations.AddItem(ret, pLM->GetTranslationInfo(i, ITranslation::TI_Author));
		if (_tcsicmp(pLM->GetTranslationPath(), pLM->GetTranslationPath(i)) == 0)
			m_cmbTranslations.SetCurSel(i + 1);
	}
}


void CLanguagesPP::SaveOptions()
{
	PrgAPI* pAPI = PRGAPI();
	TranslationManager* pLM = pAPI->GetTranslationManager();
	if (m_cmbTranslations.GetCurSel() == 0)
		pLM->UnloadTranslation();
	else
		pLM->LoadTranslation(m_cmbTranslations.GetCurSel() - 1);
	pLM->UpdateTranslatables(*pLM);
}



void CLanguagesPP::OnBnClickedCreate()
{
	CreateLangFile(FALSE);
}

//void CLanguagesPP::OnBnClickedDownload()
//{
//	PRGAPI()->GoToSiteURL(PrgAPI::SA_Navigate, _T("download-Translations"));
//}


//void CLanguagesPP::OnBnClickedHelp()
//{
//	PRGAPI()->GoToSiteURL(PrgAPI::SA_Help, _T("Translations"));
//}

void CLanguagesPP::OnBnClickedUpdate()
{
	CreateLangFile(TRUE);
}

void CLanguagesPP::CreateLangFile(BOOL bUpdateCurrent)
{
	//CFileDialog fd(FALSE, _T(".lng"), _T("NewLanguage.lng"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
	//	_T("Language Files (*.lng)|*.lng|All Files (*.*)|*.*||"), this);
	//BOOL ret = FALSE;
	//if (fd.DoModal() == IDOK)
	//{
	//	LanguageManager* pLM = PRGAPI()->GetLanguageManager();
	//	int curSel = 0;
	//	if (bUpdateCurrent)
	//		curSel = m_cmbLanguages.GetCurSel();
	//	pLM->ExportLanguageFile(curSel, fd.GetPathName());
	//}

	PrgAPI* pAPI = PRGAPI();
	TranslationManager* pLM = pAPI->GetTranslationManager();
	std::tstring pathName;
	if (bUpdateCurrent)
	{
		LPCTSTR destFileName = pLM->GetTranslationPath(m_cmbTranslations.GetCurSel() - 1);
		if (destFileName == NULL)
			return;
		pathName = destFileName;
		if (!pLM->ExportTranslationFile(m_cmbTranslations.GetCurSel() - 1, pathName.c_str()))
			return;
	}
	else
	{
		pathName = pLM->GetTranslationsFolder();
		CInputBox inp;
		inp.SetTexts(_T("New Translation"), _T("Please type the name of the new translation. If a translation with the same name exists it will be overwritten."), _T("testname"));
		if (inp.DoModal() == IDCANCEL)
			return;
		pathName += inp.GetInputString();
		pathName += _T(".lng");
		if (!pLM->ExportTranslationFile(-1, pathName.c_str()))
			return;
	}
	std::tstring fileContents;
	CTextFileRead fr(pathName.c_str());
	fr.Read(fileContents);
	fr.Close();
	
	CSimpleEditorDlg dlg(this);
	dlg.SetTitle(_T("Translation Editor"));
	dlg.SetSubtitle(_T("You may copy this text to a different editor and then paste it back. Press 'save' when done to immediately see your changes."));
	dlg.SetText(fileContents.c_str());
	if (dlg.DoModal() == IDOK)
	{
		fileContents = dlg.GetText();
		CTextFileWrite fw(pathName.c_str(), CTextFileBase::UNI16_LE);
		fw.Write(fileContents);
		fw.Close();
		pLM->UnloadTranslation();
		pLM->LoadTranslation(pathName.c_str());
		pLM->UpdateTranslatables(*pLM);
		LoadOptions();
		pAPI->MessageBox(m_hWnd, _T("Gui has been updated. You may share your work by publishing it in the forum."), _T("Translation Updated"), MB_OK);
	}
}

void CLanguagesPP::OnBnClickedOpenfolder()
{
	PrgAPI* pAPI = PRGAPI();
	pAPI->GetActionManager()->OpenFolder(pAPI->GetTranslationManager()->GetTranslationsFolder());
}

void CLanguagesPP::OnBnClickedDebug()
{
	INT curSel = m_cmbTranslations.GetCurSel();
	if (curSel == 0)
		return;

	LPCTSTR log = PRGAPI()->GetTranslationManager()->GetTranslationComparisonLog(curSel - 1);
	if (log != NULL)
	{
		TCHAR bf[MAX_PATH];
		GetTempPath(MAX_PATH, bf);
		_tcscat(bf, _T("tmp~lngdbg.txt"));
		CTextFileWrite fw(bf);
		fw.Write(log);
		fw.Close();
		ShellExecute(m_hWnd, 0, bf, 0,0, SW_SHOWDEFAULT);
	}
}
