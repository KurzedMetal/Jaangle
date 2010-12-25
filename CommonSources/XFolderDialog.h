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

#ifndef XFOLDERDIALOG_H
#define XFOLDERDIALOG_H

#ifdef USE_HISTORY_COMBO
//=== Carefully. History Combo Keeps history in an ini file in windows directory
#include "XHistoryCombo.h"
#endif

#include "XFileOpenListView.h"

// Windows 2000 version of OPENFILENAME.
// The new version has three extra members.
// This is copied from commdlg.h
//
struct OPENFILENAMEEX : public OPENFILENAME
{
	void *	pvReserved;
	DWORD	dwReserved;
	DWORD	FlagsEx;
};

///////////////////////////////////////////////////////////////////////////////
// CXFolderDialog: Encapsulate Windows-2000 style open dialog.
//
class CXFolderDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CXFolderDialog)

// Construction
public:
	CXFolderDialog(LPCTSTR lpszInitialFolder = NULL,
				   DWORD dwFlags = 0,
				   CWnd* pParentWnd = NULL);
	virtual ~CXFolderDialog();

// Dialog Data
	//{{AFX_DATA(CXFolderDialog)
#ifdef USE_HISTORY_COMBO
	CHistoryCombo	m_cmbRecentFolders;
#else
	CComboBox m_cmbRecentFolders;
#endif
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXFolderDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Operations
public:
	// override
	virtual int DoModal();

// Attributes
public:

	enum XFILEDIALOG_OS_VERSION
	{
		XFILEDIALOG_AUTO_DETECT_OS_VERSION = 0,
		XFILEDIALOG_OS_VERSION_4,
		XFILEDIALOG_OS_VERSION_5
	};

	CString GetPath() { return m_strPath; }
	enum XFILEDIALOG_OS_VERSION GetOsVersion() { return m_eOsVersion; }
	void SetOsVersion(enum XFILEDIALOG_OS_VERSION eOsVersion) { m_eOsVersion = eOsVersion; }
	void SetTitle(LPCTSTR lpszTitle) { m_strTitle = lpszTitle; }
	void SetOkTitle(LPCTSTR lpszTitle) { m_strOkTitle = lpszTitle; }
	void SetCancelTitle(LPCTSTR lpszTitle) { m_strCancelTitle = lpszTitle; }
	void SetFolderNameTitle(LPCTSTR lpszTitle) { m_strFolderNameTitle = lpszTitle; }
	void SetLookInTitle(LPCTSTR lpszTitle) { m_strLookInTitle = lpszTitle; }
	void EnableRegistry(BOOL bEnable) { m_bUseRegistry = bEnable; }
	int GetViewMode() { return m_nViewMode; }
	void SetViewMode(int cmd);

// Implementation
protected:
	OPENFILENAMEEX		m_ofnEx;	// new Windows 2000 version of OPENFILENAME
	UINT				m_nIdFileNameStatic;
	UINT				m_nIdFileNameCombo;
	UINT				m_nIdFilesOfTypeStatic;
	UINT				m_nIdFilesOfTypeCombo;
	UINT				m_nIdPlaceBar;
	UINT				m_nViewMode;
	int					m_nStaticLeftMargin;
	CString				m_strTitle;
	DWORD				m_dwFlags;
	CString				m_strInitialFolder;
	CString				m_strPath;
	CString				m_strOkTitle;
	CString				m_strCancelTitle;
	CString				m_strFolderNameTitle;
	CString				m_strLookInTitle;

	CXFileOpenListView	m_wndListView;
	BOOL				m_bUseRegistry;		// TRUE = read/write registry for list view mode
	BOOL				m_bPersist;			// TRUE = persist view
	BOOL				m_bFirstTime;

	enum XFILEDIALOG_OS_VERSION m_eOsVersion;

	CString GetPath(UINT nMessage);
	BOOL IsMinSize(int& cx, int& cy);
	BOOL IsWin2000();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnOK();

	//{{AFX_MSG(CXFolderDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelendokMruCombo();
	afx_msg void OnCbnKillfocusMruCombo();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	//afx_msg LRESULT OnPostInit(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()
};

#endif //XFOLDERDIALOG_H
