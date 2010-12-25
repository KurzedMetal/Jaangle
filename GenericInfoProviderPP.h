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


#include "TSPropertyPage.h"
#include "InformationProviders/InfoProvider.h"

class CGenericInfoProviderPP: public CTSPropertyPage
{
	DECLARE_DYNAMIC(CGenericInfoProviderPP)

public:
	CGenericInfoProviderPP(IInfoProvider& ip);
	virtual ~CGenericInfoProviderPP();

// Dialog Data
	enum { IDD = IDD_PPGENERIC };

	virtual LPCTSTR GetTitle();

public:
	BOOL IsSomethingChanged()	{return m_bSomethingChanged;}



protected:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
private:
	afx_msg void OnChange();
	virtual void OnOK();
	void SaveChanges();

private:
	BOOL m_bSomethingChanged;
	BOOL m_bNeedsUpdating;
	IInfoProvider& m_IP;

	void LoadOptions();
	void SaveOptions();
	CString m_title;
};
