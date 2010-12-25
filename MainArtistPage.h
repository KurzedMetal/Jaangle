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
#ifndef _CMainArtistPage_h_
#define _CMainArtistPage_h_

#include "TSPropertyPage.h"

class CMainArtistPage : public CTSPropertyPage
{
	DECLARE_DYNAMIC(CMainArtistPage)

public:
	CMainArtistPage(UINT nIDCaption);
	virtual ~CMainArtistPage();

// Dialog Data
	enum { IDD = IDD_PPARTISTS };
	BOOL IsSomethingChanged()	{return m_SomethingChanged;}
	BOOL SaveChanges();
	CString Artist;
	CString Genre;
	INT Rating;

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
private:
	afx_msg void OnChange();
private:
	BOOL m_SomethingChanged;
	CComboBox m_cmbRating;
	//static const int cMaxPath = 100;
	//TCHAR ArtistName[cMaxPath];
	//TCHAR GenreName[cMaxPath];
	//int Rating;
};

#endif
