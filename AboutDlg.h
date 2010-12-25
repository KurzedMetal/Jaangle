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
#include "afxwin.h"
#include "XHTMLStatic.h"

// CAboutDlg dialog

class CAboutDlg : public CDialog
{
	DECLARE_DYNAMIC(CAboutDlg)

public:
	CAboutDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	LRESULT CheckForUpdates(WPARAM wParam, LPARAM lParam);
	LRESULT GoToWebsite(WPARAM wParam, LPARAM lParam);
	LRESULT GoToForum(WPARAM wParam, LPARAM lParam);
	LRESULT GoToDonate(WPARAM wParam, LPARAM lParam);
	//LRESULT CreateLangPack(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
	CXHTMLStatic m_VersionInfo;
};
