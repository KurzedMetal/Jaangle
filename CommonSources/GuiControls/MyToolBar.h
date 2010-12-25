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
#ifndef _MYTOOLBAR_H_
#define _MYTOOLBAR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CTrueColorToolBar

class CMyToolBar : public CToolBar
{
// Construction
public:
	CMyToolBar();

// Attributes
private:
	BOOL m_bDropDown;

	struct stDropDownInfo {
	public:
		UINT  uButtonID;
		UINT  uMenuID;
		CWnd* pParent;
	};
	
	CArray <stDropDownInfo, stDropDownInfo&> m_lstDropDownButton;
	
// Operations
public:
	BOOL LoadTrueColorToolBar(int  nBtnWidth,
							  UINT uToolBar,
							  UINT uToolBarHot		= 0,
							  UINT uToolBarDisabled = 0,
							  COLORREF rgbMask = RGB(255,0,255)
							  );

	void AddDropDownButton(CWnd* pParent, UINT uButtonID, UINT uMenuID);

private:
	BOOL SetTrueColorToolBar(UINT uToolBarType,
		                     UINT uToolBar,
						     int  nBtnWidth,
							 COLORREF rgbMask = RGB(255,0,255));

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrueColorToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTrueColorToolBar)
	afx_msg void OnToolbarDropDown(NMHDR* _pnmtb, LRESULT *plr);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	//afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
