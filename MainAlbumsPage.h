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
#ifndef _CMainAlbumsPage_h_
#define _CMainAlbumsPage_h_

#include "TSPropertyPage.h"

class CMainAlbumsPage : public CTSPropertyPage
{
	DECLARE_DYNAMIC(CMainAlbumsPage)

public:
	CMainAlbumsPage(UINT nIDCaption);
	virtual ~CMainAlbumsPage();
// Dialog Data
	enum { IDD = IDD_PPALBUMS };

	BOOL IsSomethingChanged()	{return m_SomethingChanged;}
	CString Album;
	CString Artist;
	UINT Year;
	INT Rating;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnChange();
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

private:
	//LPCTSTR m_TextInfo;
	BOOL m_SomethingChanged;
	CComboBox m_cmbRating;

};

#endif
