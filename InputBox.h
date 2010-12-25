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
#ifndef _InputBox_h_
#define _InputBox_h_
#include "LabelEx.h"

class CInputBox: public CDialog
{
public:
	CInputBox(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInputBox();

	enum { IDD = IDD_INPUTBOX };

	void SetTexts(LPCTSTR Title, LPCTSTR SubTitle, LPCTSTR DefaultString);
	void SetButtonTexts(LPCTSTR OkCaption, LPCTSTR CancelCaption);
	LPCTSTR GetInputString();
private:
	void DoDataExchange(CDataExchange* pDX);
	CString m_Title, m_SubTitle, m_InputString, m_OkCaption, m_CancelCaption;

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
};

#endif
