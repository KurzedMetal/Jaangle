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
#include "SliderDlg.h"

#define TIMER_AUTOHIDE 100


SliderDlg::SliderDlg()/*:
m_initialValue(0)*/
{
}

SliderDlg::~SliderDlg()
{
}

BOOL SliderDlg::Create(LPCTSTR title, INT minPos, INT maxPos, INT initialValue, INT x, INT y, INT cx, INT cy, CWnd* pParent)
{
	INT cMargin = 3;
	HWND hParent = pParent ? pParent->GetSafeHwnd() : NULL;
	if (CreateEx(WS_EX_TOOLWINDOW, 
		_T("#32770"), 
		title, 
		WS_POPUP | WS_BORDER | WS_CAPTION | WS_SYSMENU, 
		x, y, cx, cy, 
		hParent, 0, 0))
	{
		CRect rc;
		GetClientRect(&rc);
		if (rc.Height() != cy)
			SetWindowPos(0, 0, 0, cx, cy + (cy - rc.Height()), SWP_NOMOVE | SWP_NOZORDER);
		GetClientRect(&rc);
		rc.DeflateRect(cMargin, cMargin);

		
		//m_initialValue = initialValue;
		m_slider.Create(WS_VISIBLE | WS_CHILD | TBS_BOTH, rc, this, 0);
		//m_slider.SetFocus();
		m_slider.SetRange(minPos, maxPos);
		m_slider.SetPos(initialValue);

		SetTimer(TIMER_AUTOHIDE, 500, NULL);
		return TRUE;
	}
	return FALSE;


	//=== UTILITY TO CONVERT RESOURCE TO HEX STRING
	//HRSRC hrsrc = ::FindResource(0, MAKEINTRESOURCE(IDD_DIALOG4), RT_DIALOG);
	//if (hrsrc != NULL)
	//{
	//	HGLOBAL hglobalRsrc = ::LoadResource(0, hrsrc);
	//	BYTE* res = (BYTE*)::LockResource(hglobalRsrc);
	//	DWORD sz = ::SizeofResource(0, hrsrc);
	//	TCHAR bf[10];
	//	std::tstring strBA;
	//	for (INT i = 0; i < sz; i++)
	//	{
	//		_sntprintf(bf, 10, _T("%d,"), res[i]);
	//		strBA += bf;
	//	}
	//	INT a = 0;
	//
	//}


	//const BYTE emptyRes[] = {1,0,255,255,0,0,0,0,0,0,0,0,200,0,200,128,0,0,0,0,0,0,186,0,90,0,0,0,0,0,68,0,105,0,97,0,108,0,111,0,103,0,0,0,8,0,144,1,0,1,77,0,83,0,32,0,83,0,104,0,101,0,108,0,108,0,32,0,68,0,108,0,103,0,0, 0};
	//m_res = new BYTE[sizeof(emptyRes)];
	//memcpy(m_res, emptyRes, sizeof(emptyRes));
	//DLGTEMPLATE& dt = *((DLGTEMPLATE*)m_res);
	//dt.x = (SHORT)rc.left;
	//dt.y = rc.top;
	//dt.cx = rc.Width();
	//dt.cy = rc.Height();

	//return InitModalIndirect(&dt, pParent);
}


void SliderDlg::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_AUTOHIDE:
		if (IsWindowVisible())
		{
			if (::GetForegroundWindow() != m_hWnd)
				DestroyWindow();
			else
			{
				KillTimer(TIMER_AUTOHIDE);
				SetTimer(TIMER_AUTOHIDE, 500, 0);
			}
		}
		break;
	default:
		CWnd::OnTimer(nIDEvent);
		break;
	}
}

BEGIN_MESSAGE_MAP(SliderDlg, CWnd)
	ON_WM_HSCROLL()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void SliderDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar->m_hWnd == m_slider.m_hWnd)
	{
		if (m_pListener != NULL)
			m_pListener->OnChangeSlider(m_slider.GetPos());
	}
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL SliderDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	if (wParam == IDOK)
	{
		DestroyWindow();
		return TRUE;
	}
	else if (wParam == IDCANCEL)
	{
		//if (m_pListener)
		//	m_pListener->OnChangeSlider(m_initialValue);
		DestroyWindow();
		return TRUE;
	}
	return CWnd::OnCommand(wParam, lParam);
}
