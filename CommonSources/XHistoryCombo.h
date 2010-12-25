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

#ifndef HISTORYCOMBO_H
#define HISTORYCOMBO_H

#ifndef __AFXADV_H__
#include "afxadv.h" // needed for CRecentFileList
#endif // ! __AFXADV_H__

/////////////////////////////////////////////////////////////////////////////
// CHistoryCombo window

class CHistoryCombo : public CComboBox
{
// Construction
public:
	CHistoryCombo(BOOL bAllowSortStyle = FALSE);

// Attributes
public:

// Operations
public:
	int AddString(LPCTSTR lpszString);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistoryCombo)
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void	StoreValue(BOOL bIgnoreIfEmpty = TRUE);
	void	ClearHistory(BOOL bDeleteRegistryEntries = TRUE);
	void	SetMaxHistoryItems(int nMaxItems);
	void	SaveHistory(BOOL bAddCurrentItemtoHistory = TRUE);
	CString	LoadHistory(LPCTSTR lpszSection, 
						LPCTSTR lpszKeyPrefix, 
						BOOL bSaveRestoreLastCurrent = TRUE, 
						LPCTSTR lpszKeyCurItem = NULL);
	CString	LoadHistory(CRecentFileList* pListMRU, BOOL bSelectMostRecent = TRUE);

	int		GetMaxHistoryItems() { return m_nMaxHistoryItems; }						//+++hd
	void	SetCheckAccess(BOOL bCheckAccess) { m_bCheckAccess = bCheckAccess; }	//+++hd
	BOOL	GetCheckAccess() { return m_bCheckAccess; }								//+++hd
	void	SetDropSize(int nDropSize) { m_nDropSize = nDropSize; }					//+++hd
	int		GetDropSize() { return m_nDropSize; }									//+++hd

	// Generated message map functions
protected:
	CString m_sSection;
	CString m_sKeyPrefix;
	CString m_sKeyCurItem;
	BOOL m_bSaveRestoreLastCurrent;
	int m_nMaxHistoryItems;
	BOOL m_bAllowSortStyle;
	BOOL m_bCheckAccess;	// TRUE = check if the strings are file paths	//+++hd
	int m_nDropSize;		// no. of items in dropdown list				//+++hd

	//{{AFX_MSG(CHistoryCombo)
	afx_msg void OnDropdown();		//+++hd
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //HISTORYCOMBO_H
