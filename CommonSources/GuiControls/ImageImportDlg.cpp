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

#include "stdafx.h"
#include "Resource.h"
#include "imageimportdlg.h"
#include <dlgs.h>

// CImageImportDlg
IMPLEMENT_DYNAMIC(CImageImportDlg, CFileDialog)

void CImageImportDlg::CHookShellWnd::SetOwner(CImageImportDlg *pOwner)
{
	ASSERT(pOwner);
	ASSERT_KINDOF(CImageImportDlg, pOwner);

	m_pOwner = pOwner;
}

BOOL CImageImportDlg::CHookShellWnd::OnCommand(WPARAM /*wParam*/, LPARAM lParam)
{
//	TRACE("wp(%d), lp(%x)\n", wParam, lParam);
	return CWnd::OnCommand(0, lParam);
}

BOOL CImageImportDlg::CHookShellWnd::OnNotify(WPARAM, LPARAM lParam, LRESULT *pResult)
{
	LPNMLISTVIEW pLVHdr	= reinterpret_cast<LPNMLISTVIEW>(lParam);

	if (pLVHdr->hdr.code ==	LVN_ITEMCHANGED	&& (pLVHdr->uChanged & LVIF_STATE))
	{
		if (pLVHdr->iItem != -1 && pLVHdr->uNewState == 3)
		{
			//	It's a valid listview index	so we attach the
			//	the	handle of the window that sent the message
			//	to a local CListCtrl object	for	easy access.
			CListCtrl	  ctl;
			LPCITEMIDLIST pidl;
			TCHAR		  tszBuffer[_MAX_PATH],
						  tszFileName[_MAX_PATH],
						  tszExtension[_MAX_EXT];
			CString		  csTemp;

			ctl.Attach(pLVHdr->hdr.hwndFrom);
			pidl = (LPCITEMIDLIST) ctl.GetItemData(pLVHdr->iItem);
			SHGetPathFromIDList(pidl, tszBuffer);
			_tsplitpath(tszBuffer, NULL, NULL, tszFileName, tszExtension);
			csTemp.Format(_T("%s%s"), tszFileName, tszExtension);

			//	Update our parent window
			if (m_pOwner->m_nPrevSelection != pLVHdr->iItem)
				m_pOwner->UpdatePreview(csTemp);

			//	Be certain we detach the handle	before the CListCtrl
			//	object goes	out	of scope (else the underlying List View
			//	will be	deleted, which is NOT what we want).
			ctl.Detach();
		}
		else
			m_pOwner->UpdatePreview(_T(""));
	}

	return *pResult	= 0;
}

CImageImportDlg::CImageImportDlg(LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR szFilter, CWnd* pParentWnd) : 
CFileDialog(TRUE, lpszDefExt, lpszFileName, dwFlags, szFilter, pParentWnd)
{
	m_ofn.Flags |= OFN_ENABLETEMPLATE /*| OFN_ALLOWMULTISELECT*/ | OFN_ENABLESIZING | OFN_EXPLORER;
	m_ofn.hInstance = AfxGetInstanceHandle();
	m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_IMAGEPREVIEWDLG);

	//	Provide a big buffer for returned filenames
	m_ofn.lpstrFile = new TCHAR[10000];
	m_ofn.nMaxFile = 10000;
	memset(m_ofn.lpstrFile, 0, countof(m_ofn.lpstrFile));

	m_nPrevSelection = -1;
}

CImageImportDlg::~CImageImportDlg()
{
	delete m_ofn.lpstrFile;
}

BEGIN_MESSAGE_MAP(CImageImportDlg, CFileDialog)
END_MESSAGE_MAP()

INT_PTR	CImageImportDlg::DoModal()
{
	INT_PTR status = CFileDialog::DoModal();

	//m_preview.Close();
	return status;
}

BOOL CImageImportDlg::OnNotify(WPARAM, LPARAM lp, LRESULT *pResult)
{
	LPOFNOTIFY of =	(LPOFNOTIFY) lp;
	CString	   csTemp;
	TCHAR	   tszBuffer[_MAX_PATH];

	switch (of->hdr.code)
	{
	case CDN_SELCHANGE:
		if (CommDlg_OpenSave_GetSpec(GetParent()->GetSafeHwnd(), tszBuffer, countof(tszBuffer)) > 0)
			TRACE(_T("@4 CImageImportDlg::OnNotify CDN_SELCHANGE '%s'\n"), tszBuffer);
		break;

	case CDN_FOLDERCHANGE:
		//	Once we	get	this notification our old subclassing of
		//	the	SHELL window is	lost, so we	have to
		//	subclass it	again. (Changing the folder	causes a 
		//	destroy	and	recreate of	the	SHELL window).
		if (m_wndHook.GetSafeHwnd()	!= HWND(NULL))
			m_wndHook.UnsubclassWindow();

		m_wndHook.SubclassWindow(GetParent()->GetDlgItem(lst2)->GetSafeHwnd());
		UpdatePreview(_T(""));
		break;
	}

	*pResult = 0;
	return FALSE;
}

void CImageImportDlg::DoDataExchange(CDataExchange *pDX)
{
	CFileDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGEPREVIEW, m_preview);
	DDX_Text(pDX, IDC_PREVIEWNAME, m_csPreviewName);
}

// CImageImportDlg message handlers
BOOL CImageImportDlg::OnInitDialog()
{
	CFileDialog::OnInitDialog();
	m_wndHook.SetOwner(this);
	GdiPlusPicDrawer::DrawParams& params = m_preview.GetDrawParams();
	params.zoomLockMode = GdiPlusPicDrawer::ZLM_FillArea;
	return TRUE;
}

void CImageImportDlg::UpdatePreview(LPCTSTR szFilename)
{
	ASSERT(szFilename);
	ASSERT(AfxIsValidString(szFilename));
	m_csPreviewName = szFilename;
	if (szFilename[0] != 0)
	{
		if (m_preview.LoadFile(GetFolderPath() + _T("\\") + m_csPreviewName))
		{
			CSize sz = m_preview.GetPictureSize();
			TCHAR bf[200];
			_sntprintf(bf, 200, _T("\r\n[%d x %d] pixels"), sz.cx, sz.cy);
			m_csPreviewName += bf;
		}
		
	}
	else
		m_preview.ClearImage();
	UpdateData(FALSE);
}
