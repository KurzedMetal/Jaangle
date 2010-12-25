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

#include "HotKeyCtrlAPI.h"
#include "afxwin.h"
#include "TSPropertyPage.h"

// CMediaPlayerPP dialog
class MediaPlayer;

class CGlobalHotkeysPP : public CTSPropertyPage
{
	DECLARE_DYNAMIC(CGlobalHotkeysPP)

public:
	CGlobalHotkeysPP(UINT nIDCaption);
	virtual ~CGlobalHotkeysPP();

// Dialog Data
	enum { IDD = IDD_PPGLOBALHOTKEYS };
	virtual LPCTSTR GetHelpID()						{return _T("global_hotkeys");}

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);


	DECLARE_MESSAGE_MAP()


public:
	BOOL IsSomethingChanged()	{return m_bSomethingChanged;}
	virtual void ApplyTranslation(ITranslation& skin);

private:
	afx_msg void OnChange();
	virtual void OnOK();
	void SaveChanges();

private:
	BOOL m_bSomethingChanged;
	BOOL m_bNeedsUpdating;

	void LoadGlobalHotkeysOptions();
	void SaveGlobalHotkeysOptions();
	CHotKeyCtrlAPI m_HKPlay;
	CHotKeyCtrlAPI m_HKStop;
	CHotKeyCtrlAPI m_HKPrevious;
	CHotKeyCtrlAPI m_HKNext;
	CHotKeyCtrlAPI m_HKMiniPlayer;
	CHotKeyCtrlAPI m_HKTooltip;
	CHotKeyCtrlAPI m_HKQuickSearch;

};
