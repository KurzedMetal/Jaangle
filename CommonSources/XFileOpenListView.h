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

#ifndef XFILEOPENLISTVIEW_H
#define XFILEOPENLISTVIEW_H

#define XLVM_XP_DO_NOT_PERSIST			(-1)
#define XLVM_XP_UNDEFINED				0
#define XLVM_XP_ICONS					0x7029
#define XLVM_XP_LIST					0x702B
#define XLVM_XP_DETAILS					0x702C
#define XLVM_XP_THUMBNAILS				0x702D
#define XLVM_XP_TILES					0x702E

#define XLVM_VISTA_DO_NOT_PERSIST		(-1)
#define XLVM_VISTA_UNDEFINED			0
#define XLVM_VISTA_DETAILS				0x704B
#define XLVM_VISTA_TILES				0x704C
#define XLVM_VISTA_EXTRA_LARGE_ICONS	0x704D
#define XLVM_VISTA_MEDIUM_ICONS			0x704E
#define XLVM_VISTA_LARGE_ICONS			0x704F
#define XLVM_VISTA_SMALL_ICONS			0x7050
#define XLVM_VISTA_LIST					0x7051

///////////////////////////////////////////////////////////////////////////////
// This class is used to subclass the "SHELLDLL_DefView" window in the file
// open dialog.
//
class CXFileOpenListView : public CWnd
{
public:
	CXFileOpenListView(CWnd * pDlg = NULL) : m_lastViewMode(0), m_pDialog(pDlg) {}
	void Init(CWnd * pWnd) { m_pDialog = pWnd; }
	~CXFileOpenListView() {}

	CWnd *	m_pDialog;
	DWORD	m_lastViewMode;
	BOOL	m_bRedraw;

	int  GetViewMode();
	BOOL SetViewMode(int cmd);

protected:
	int GetVistaViewMode(long lView, CListCtrl * plc);
	int GetXpViewMode(long lView, CListCtrl * plc);

	virtual LRESULT WindowProc(UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif //XFILEOPENLISTVIEW_H
