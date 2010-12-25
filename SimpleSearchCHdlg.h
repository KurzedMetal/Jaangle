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
#ifndef _CSimpleSearchCHdlg_h_
#define _CSimpleSearchCHdlg_h_

// CSimpleSearchCHdlg dialog
#include "SearchPropertyPages.h"

class CSimpleSearchCHdlg : public CSearchPropertyPages
{
	DECLARE_DYNAMIC(CSimpleSearchCHdlg)

public:
	CSimpleSearchCHdlg();
	virtual ~CSimpleSearchCHdlg();

	enum FILETYPE_OPTIONS
	{
		FTO_First,
		FTO_All,
		FTO_Audio,
		FTO_Video,
		FTO_Last
	};
// Dialog Data
	enum { IDD = IDD_PPSIMPLESEARCH };

	void GetSimpleSearchText(LPTSTR bf, UINT len);
	void SetSimpleSearchText(LPCTSTR bf);
	FILETYPE_OPTIONS GetFileTypeOption();
	void SetFileTypeOption(FILETYPE_OPTIONS fto);

	//void SetRealParent(CWnd* pParent){m_pParent = pParent;}
	virtual void ApplyTranslation(ITranslation& skin); //ICtrlStrings
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	//void ShowAllCheckChanged();
	FILETYPE_OPTIONS m_fto;
public:
	afx_msg void OnBnClickedSearch();
};

#endif
