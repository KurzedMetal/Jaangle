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
class CTeenSpiritDlg;

class CMediaPlayerPP : public CTSPropertyPage
{
	DECLARE_DYNAMIC(CMediaPlayerPP)

public:
	CMediaPlayerPP(UINT nIDCaption);
	virtual ~CMediaPlayerPP();

// Dialog Data
	enum { IDD = IDD_PPPLAYEROPTIONS };
	virtual LPCTSTR GetHelpID()						{return _T("media_player");}

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);


	DECLARE_MESSAGE_MAP()


public:
	BOOL IsSomethingChanged()	{return m_bSomethingChanged;}
	void SetObjectsToConfigure(MediaPlayer* pPlayer)
	{
		m_pPlayer = pPlayer;
	}

	virtual void ApplyTranslation(ITranslation& skin);

private:
	afx_msg void OnChange();
	virtual void OnOK();
	void SaveChanges();

private:
	BOOL m_bSomethingChanged;
	BOOL m_bNeedsUpdating;

	//CTeenSpiritDlg* m_pTSDlg;
	MediaPlayer* m_pPlayer;
	void LoadMPCtrlOptions();
	void SaveMPCtrlOptions();
	CHotKeyCtrlAPI m_HKPlay;
	CHotKeyCtrlAPI m_HKStop;
	CHotKeyCtrlAPI m_HKPrevious;
	CHotKeyCtrlAPI m_HKNext;
	CComboBox m_cmbPlayControl;
	CComboBox m_cmbDefaultPlayerAction;
	//CSliderCtrl m_SldStart;
	CSliderCtrl m_SldEnd;

};
