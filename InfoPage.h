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
#ifndef _CInfoPage_h_
#define _CInfoPage_h_

#include "TSPropertyPage.h"
#include "XHTMLStatic.h"
#include "GuiControls/ColorEdit.h"

class CInfoPage : public CTSPropertyPage
{
	DECLARE_DYNAMIC(CInfoPage)

public:
	CInfoPage(UINT nIDCaption);
	virtual ~CInfoPage();

// Dialog Data
	enum { IDD = IDD_PPINFO };

	BOOL IsSomethingChanged()	{return m_bInitialized && (m_InfoAtStart != info);}
	CString info;

	void EnableFixedFont(BOOL bFixedFont)	{m_bFixedFont = bFixedFont;}
	void EnableReadOnlyMode(BOOL bReadOnly)	{m_bReadOnly = bReadOnly;}

	virtual BOOL OnSetActive();
protected:
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	LRESULT NotifyHyperlinkClicked(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	CXHTMLStatic m_Static;
	CString m_InfoAtStart;
	BOOL m_bInitialized;
	BOOL m_bFixedFont;
	BOOL m_bReadOnly;
	CColorEdit m_edit;

};

#endif
