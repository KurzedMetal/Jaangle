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
#ifndef _LanguagesPP_h_
#define _LanguagesPP_h_

#pragma once

#include "TSPropertyPage.h"
#include "XHTMLStatic.h"
#include "FlexButton.h"
#include "GuiControls/DualTextComboBox.h"

class CLanguagesPP : public CTSPropertyPage
{
	DECLARE_DYNAMIC(CLanguagesPP)

public:
	CLanguagesPP(UINT nIDCaption);
	virtual ~CLanguagesPP();

// Dialog Data
	enum { IDD = IDD_PPLANGUAGES };

	BOOL IsSomethingChanged()							{return m_bSomethingChanged;}
	virtual void ApplyTranslation(ITranslation& skin);//
	virtual LPCTSTR GetHelpID()						{return _T("language");}

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
private:
//	void GetNameFromComboSelection(LPTSTR lngName, UINT bfLen);
	afx_msg void OnChange();
	void SaveChanges();
	void CreateLangFile(BOOL bUpdateCurrent);
	void LoadOptions();
	void SaveOptions();
	void RefreshInfo();
	afx_msg void OnBnClickedCreate();
	//afx_msg void OnBnClickedDownload();
	//afx_msg void OnBnClickedHelp();
	afx_msg void OnBnClickedUpdate();
	afx_msg void OnBnClickedOpenfolder();
	afx_msg void OnBnClickedDebug();

private:
	BOOL m_bSomethingChanged;
	BOOL m_bNeedsUpdating;
	CDualTextComboBox m_cmbTranslations;
	CXHTMLStatic m_info;
	//CButton m_download;
	//CButton m_create;
	//CButton m_update;
	//CButton m_help;
};

#endif
