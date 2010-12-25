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
#include "XFileOpenListView.h"
#include "WinVersionHelper.h"

// definitions from commctrl.h
#ifndef LV_VIEW_ICON
#define LV_VIEW_ICON        0x0000
#define LV_VIEW_DETAILS     0x0001
#define LV_VIEW_SMALLICON   0x0002
#define LV_VIEW_LIST        0x0003
#define LV_VIEW_TILE        0x0004
#define LV_VIEW_MAX         0x0004
#endif

#ifndef LVM_GETVIEW
#define LVM_GETVIEW         (LVM_FIRST + 143)
#endif

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ((void)0)
#endif
#endif

#undef TRACE
#define TRACE __noop

//=============================================================================
// if you want to see the TRACE output, uncomment this line:
//#include "XTrace.h"
//=============================================================================

//=============================================================================
//=============================================================================
// CXFileOpenListView - Used to subclass the "SHELLDLL_DefView" window in the
// file open dialog. This window contain the list view and processses
// commands to set different view modes.
//=============================================================================
//=============================================================================


//=============================================================================
// Set the list view to desired mode.
BOOL CXFileOpenListView::SetViewMode(int cmd)
//=============================================================================
{
	TRACE(_T("in CXFileOpenListView::SetViewMode:  0x%X\n"), cmd);

	if (IsWindow(m_hWnd)) 
	{
		// SHELLDLL_DefView window found: send it the command.
		if (cmd == 0)
		{
			WindowsVersion verInfo;
			if (GetWindowsVersion(verInfo))
			{
				if (verInfo.osCode == VST_WinXP)	
					cmd = XLVM_XP_LIST;
				else if (verInfo.osCode >= VST_Vista)
					cmd = XLVM_VISTA_LIST;
			}
		}

		SendMessage(WM_COMMAND, cmd);

		// Send parent dialog a Refresh command (F5) to force repaint.
		// The command ID value was obtained by inspection using Spy++.
		const UINT CMD_REFRESH = 40966;
		GetParent()->SendMessage(WM_COMMAND, CMD_REFRESH);

		return TRUE;
	}
	else
	{
		TRACE(_T("ERROR - no m_hWnd\n"));
	}
	return FALSE;
}

//=============================================================================
// Get current list view mode in the form of WM_COMMAND code that can be
// passed to SetViewMode.
int CXFileOpenListView::GetViewMode()
//=============================================================================
{
	TRACE(_T("in CXFileOpenListView::GetViewMode\n"));

	int nViewMode = 0;

	if (IsWindow(m_hWnd))
	{
		// Get the child list view window.
		HWND hlc = ::FindWindowEx(m_hWnd, NULL, _T("SysListView32"), NULL);
		if (hlc)
		{
			CListCtrl* plc = (CListCtrl*)CWnd::FromHandle(hlc);
			if (plc)
			{
				long lView = (long)plc->SendMessage(LVM_GETVIEW);

				TRACE(_T("lView=%X\n"), lView);

				WindowsVersion verInfo;
				if (GetWindowsVersion(verInfo))
				{
					if (verInfo.osCode == VST_WinXP)	
						nViewMode = GetXpViewMode(lView, plc);
					else if (verInfo.osCode >= VST_Vista)
						nViewMode = GetVistaViewMode(lView, plc);
				}
			}
		}
	}

	return nViewMode;
}

//=============================================================================
int CXFileOpenListView::GetXpViewMode(long lView, CListCtrl* plc)
//=============================================================================
{
	int nViewMode = 0;

	if (lView == LV_VIEW_ICON) 
	{
		// If list view is in ICON mode, look at icon spacing to determine
		// whether Thumbnail or Icon mode.
		CSize sz(0, 0);
		if (plc && IsWindow(plc->m_hWnd))
			sz = CSize((DWORD)plc->SendMessage(LVM_GETITEMSPACING));
		TRACE(_T("sz.cx = %d\n"), sz.cx);

		if (sz.cx > GetSystemMetrics(SM_CXICONSPACING)) 
		{
			nViewMode = XLVM_XP_THUMBNAILS;
		} 
		else 
		{
			nViewMode = XLVM_XP_ICONS;
		}
	} 
	else if (lView == LV_VIEW_DETAILS) 
	{
		nViewMode = XLVM_XP_DETAILS;
	} 
	else if (lView == LV_VIEW_LIST) 
	{
		nViewMode = XLVM_XP_LIST;
	} 
	else if (lView == LV_VIEW_TILE) 
	{
		nViewMode = XLVM_XP_TILES;
	}

	return nViewMode;
}

//=============================================================================
int CXFileOpenListView::GetVistaViewMode(long lView, CListCtrl* plc)
//=============================================================================
{
	int nViewMode = 0;

	if (lView == LV_VIEW_SMALLICON)
	{
		nViewMode = XLVM_VISTA_SMALL_ICONS;
	}
	else if (lView == LV_VIEW_ICON) 
	{
		// If list view is in ICON mode, look at icon spacing to determine
		// whether Thumbnail or Icon mode.
		CSize sz(0, 0);
		if (plc && IsWindow(plc->m_hWnd))
			sz = CSize((DWORD)plc->SendMessage(LVM_GETITEMSPACING));
		int nIconSpacing = GetSystemMetrics(SM_CXICONSPACING);
		//CString s;
		//s.Format(_T("item spacing = (%d, %d), nIconSpacing = %d"), sz.cx, sz.cy, nIconSpacing);
		//::MessageBox(NULL, s, _T("item spacing"), MB_OK);

		if (sz.cx == nIconSpacing) 
		{
			nViewMode = XLVM_VISTA_MEDIUM_ICONS;
		}
		else if (sz.cx > nIconSpacing*2) 
		{
			nViewMode = XLVM_VISTA_EXTRA_LARGE_ICONS;
		} 
		else 
		{
			nViewMode = XLVM_VISTA_LARGE_ICONS;
		}
	} 
	else if (lView == LV_VIEW_DETAILS) 
	{
		nViewMode = XLVM_VISTA_DETAILS;

	} 
	else if (lView == LV_VIEW_LIST) 
	{
		nViewMode = XLVM_VISTA_LIST;

	} 
	else if (lView == LV_VIEW_TILE) 
	{
		nViewMode = XLVM_VISTA_TILES;
	}

	return nViewMode;
}

//=============================================================================
// Process window message from list view. Call default to bypass MFC
// entirely because MFC has all sorts of ASSERT checks that will bomb.
// The only message I care about is WM_DESTROY, to re-initialize myself.
//
// The original version in MSDN only overrode OnCommand; it's necessary to
// override WindowProc to completely bypass MFC.
//
LRESULT CXFileOpenListView::WindowProc(UINT msg, WPARAM, LPARAM)
//=============================================================================
{
	if (msg == WM_DESTROY) 
	{
		m_lastViewMode = GetViewMode();				// save current view mode
		UnsubclassWindow();							// unsubclass myself
	}
	return Default(); // all message: pass to default WndProc, avoid MFC/CWnd
}
