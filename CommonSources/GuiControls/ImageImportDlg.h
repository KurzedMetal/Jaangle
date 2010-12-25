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
#include "GuiControls/PicWnd.h"

class CImageImportDlg;

// CImageImportDlg
class CImageImportDlg :	public CFileDialog
{
	class CHookShellWnd : public CWnd
	{
	public:
		void			SetOwner(CImageImportDlg *m_pOwner);

		virtual	BOOL	OnNotify(WPARAM	wParam,	LPARAM lParam, LRESULT*	pResult);
		virtual BOOL	OnCommand(WPARAM wp, LPARAM lp);

	private:
		CImageImportDlg	*m_pOwner;
	};

	DECLARE_DYNAMIC(CImageImportDlg)
public:
	CImageImportDlg(LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL, DWORD dwFlags =	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR szFilter = NULL, CWnd* pParentWnd = NULL);
	//CImageImportDlg(LPCTSTR	lpszDefExt = NULL, LPCTSTR lpszFileName	= NULL,	DWORD dwFlags =	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,	CWnd* pParentWnd = NULL);
	virtual			~CImageImportDlg();

	virtual	void	DoDataExchange(CDataExchange *pDX);
	virtual	BOOL	OnNotify(WPARAM	wParam,	LPARAM lParam, LRESULT*	pResult);
	virtual INT_PTR	DoModal();

	void			UpdatePreview(LPCTSTR szFilename);

protected:
	CPicWnd			m_preview;
	CString			m_csPreviewName;
	int				m_nPrevSelection;
	CHookShellWnd	m_wndHook;

	virtual	BOOL	OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
