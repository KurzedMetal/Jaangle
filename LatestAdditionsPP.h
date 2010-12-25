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
#pragma once


#include "afxdtctl.h"
#include "SearchPropertyPages.h"

class CLatestAdditionsPP : public CSearchPropertyPages
{
	DECLARE_DYNAMIC(CLatestAdditionsPP)

public:
	CLatestAdditionsPP();
	virtual ~CLatestAdditionsPP();

// Dialog Data
	enum { IDD = IDD_PPLATESTADDITIONS };

	void GetFirstDate(SYSTEMTIME* fDate);
	void SetFirstDate(SYSTEMTIME* fDate);
	//void SetRealParent(CWnd* pParent){m_pParent = pParent;}
	//BOOL IsActive();
	//void SetActive(BOOL bActive);
	virtual void ApplyTranslation(ITranslation& skin); //ICtrlStrings

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnDtnDatetimechangeDatepicker(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnChange();

	DECLARE_MESSAGE_MAP()

private:
	SYSTEMTIME m_st;
	CDateTimeCtrl m_Date;
};
