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
#include "TeenSpirit.h"
#include "ViewerPane.h"
#include "InfoCtrl.h"
#include "VisualizationContainer.h"
#include "MediaPlayer.h"
#include "PrgAPI.h"
#include "TSBrowserEnh.h"

CViewerPane::CViewerPane():
	m_pInfoCtrl(NULL),
	m_pVisCtrl(NULL),
	m_pBrowser(NULL),
	m_mod(MOD_None),
	m_bInitialized(FALSE),
//	m_bAutoViewerMode(TRUE),
	m_bSyncWithPlayer(TRUE)
{
	PrgAPI* pAPI = PRGAPI();
	//pAPI->RegisterReceiver(*this);
}

CViewerPane::~CViewerPane()
{
	PrgAPI* pAPI = PRGAPI();
	//pAPI->SetOption(OPT_VIEWER_Auto, m_bAutoViewerMode);
	pAPI->SetOption(OPT_VIEWER_Mode, m_mod);
	pAPI->SetOption(OPT_VIEWER_SyncWithPlayer, m_bSyncWithPlayer);
	//pAPI->UnRegisterReceiver(*this);
	if (m_pBrowser)
	{
		m_pBrowser->DestroyWindow();
		delete m_pBrowser;
		m_pBrowser = NULL;
	}

	if (m_pInfoCtrl != NULL)
	{
		m_pInfoCtrl->DestroyWindow();
		delete m_pInfoCtrl;
		m_pInfoCtrl = NULL;
	}
	if (m_pVisCtrl)
	{
		m_pVisCtrl->DestroyWindow();
		delete m_pVisCtrl;
		m_pVisCtrl = NULL;
	}
}

BOOL CViewerPane::Create(CWnd* pParent, UINT nID)
{
	if (CTeenSpiritPane::Create(pParent, nID))
	{
		SetButton(IDC_OPTIONS, NULL, PRGAPI()->GetIcon(ICO_Options16), 16, 16, TRUE);
		return TRUE;
	}
	return FALSE;
}

void CViewerPane::OnInit()
{
	PrgAPI* pAPI = PRGAPI();
	SetMode((MODES)pAPI->GetOption(OPT_VIEWER_Mode));
}

void CViewerPane::ApplyTranslation(ITranslation& translation)
{
	if (m_OptionsMenu.m_hMenu)
		m_OptionsMenu.DestroyMenu();
}

LPCTSTR CViewerPane::GetTitle(UINT idx)
{
	PrgAPI* pAPI = PRGAPI();
	switch (idx)
	{
	case 0:
		{
			INT resStr = NULL;
			switch (m_mod)
			{
			case MOD_Visualizations:
				resStr = IDS_VISUALIZATIONSVIDEO;
				break;
			case MOD_Browser:
				resStr = IDS_BROWSER;
				break;
			case MOD_NormalInfo:
			default:
				resStr = IDS_SPECIALINFO;
				break;
			}
			return pAPI->GetString(resStr);
		}
		break;
	case 1:
		_sntprintf(m_stringBuffer, 500, _T("%s: [F5]"), pAPI->GetString(IDS_FOCUS));
		return m_stringBuffer;
	}
	return NULL;
}

CInfoCtrl* CViewerPane::GetInfoControl()			
{
	if (m_pInfoCtrl == NULL)
	{
		//m_pInfoCtrl = PRGAPI()->GetInfoControl();

		m_pInfoCtrl = new CInfoCtrl;
		m_pInfoCtrl->Create(AfxGetMainWnd(), 900);
		m_pInfoCtrl->SetParent(this);
	}
	return m_pInfoCtrl;
}

void CViewerPane::SetMode(MODES mod)
{
	ASSERT(mod > MOD_None && mod < MOD_Last);
	if (!(mod > MOD_None && mod < MOD_Last))
		mod = MOD_NormalInfo;
	if (m_mod != mod)
	{
		PrgAPI* pAPI = PRGAPI();
		TRACEST(_T("CViewerPane::SetMode (Changing)"), mod);
		m_mod = mod;
		Invalidate(FALSE);
		CWnd* pMC = GetMainControl();
		if (pMC != NULL)
			pMC->ShowWindow(FALSE);
		switch (m_mod)
		{
		case MOD_NormalInfo:
			{
				CInfoCtrl* pIC = GetInfoControl();
				SetMainControl(pIC);
				pIC->ShowWindow(SW_SHOW);
				//pIC->OnStateChanged();
			}
			break;
		case MOD_Visualizations:
			SetMainControl(GetVisualizationCtrl());
			break;
		case MOD_Browser:
			SetMainControl(GetBrowserCtrl());
			break;
		default:
			ASSERT(0);
		}
		ApplyTranslation(*pAPI->GetTranslationManager());
	}
}

BOOL CViewerPane::OnStateChanged(UINT stateMessages)
{
	BOOL bHandled = FALSE;
	return bHandled;
}



CVisualizationContainer* CViewerPane::GetVisualizationCtrl()
{
	ASSERT(m_hWnd != 0);
	if (m_pVisCtrl == NULL)
	{
		TRACEST(_T("CViewerPane::GetVisualizationCtrl"));
		m_pVisCtrl = new CVisualizationContainer;
		m_pVisCtrl->Create(
			NULL,//_T("Button"),//_T("Edit"),//_T("#32770"),	AfxRegisterWndClass(CS_DBLCLKS  | CS_PARENTDC| CS_GLOBALCLASS), 
			NULL, 
			WS_VISIBLE | WS_CHILD,
			CRect(0,0,0,0), this, 910, NULL);
		m_pVisCtrl->SetMediaPlayer(PRGAPI()->GetMediaPlayer());
	}
	return m_pVisCtrl;
}

TSBrowserEnh* CViewerPane::GetBrowserCtrl()
{
	ASSERT(m_hWnd != 0);
	if (m_pBrowser == NULL)
	{
		TRACEST(_T("CViewerPane::GetBrowserCtrl"));
		m_pBrowser = new TSBrowserEnh;
		m_pBrowser->Create(NULL, NULL, 
			WS_VISIBLE | WS_CHILD, 
			CRect(0,0,0,0), 
			this, 
			920, 
			NULL);
	}
	return m_pBrowser;
}




CViewerPane::MODES CViewerPane::GetMode()
{
	return m_mod;
}


BOOL CViewerPane::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CViewerPane::OnCommand"), wParam);
	PrgAPI* pAPI = PRGAPI();
	switch (wParam)
	{
	case IDC_OPTIONS:
		{
			if (m_OptionsMenu.m_hMenu == 0)
			{
				m_OptionsMenu.CreatePopupMenu();
				m_OptionsMenu.SetFont(pAPI->GetFont(FONT_Dialogs));

				m_OptionsMenu.AppendMenu(MF_STRING, IDS_SPECIALINFO, (LPTSTR)pAPI->GetString(IDS_SPECIALINFO));
				m_OptionsMenu.AppendMenu(MF_STRING, IDS_VISUALIZATIONSVIDEO, (LPTSTR)pAPI->GetString(IDS_VISUALIZATIONSVIDEO));
				m_OptionsMenu.AppendMenu(MF_STRING, IDS_BROWSER, (LPTSTR)pAPI->GetString(IDS_BROWSER));
				m_OptionsMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
				m_OptionsMenu.AppendMenu(MF_STRING, IDS_AUTO, (LPTSTR)pAPI->GetString(IDS_AUTO));
				m_OptionsMenu.AppendMenu(MF_SEPARATOR, NULL, NULL);
				m_OptionsMenu.AppendMenu(MF_STRING, IDS_SYNCWITHPLAYER, (LPTSTR)pAPI->GetString(IDS_SYNCWITHPLAYER));
			}
			m_OptionsMenu.CheckMenuItem(IDS_SPECIALINFO, m_mod == MOD_NormalInfo ? MF_CHECKED : MF_UNCHECKED);
			m_OptionsMenu.CheckMenuItem(IDS_VISUALIZATIONSVIDEO, m_mod == MOD_Visualizations ? MF_CHECKED : MF_UNCHECKED);
			m_OptionsMenu.CheckMenuItem(IDS_BROWSER, m_mod == MOD_Browser ? MF_CHECKED : MF_UNCHECKED);
			m_OptionsMenu.CheckMenuItem(IDS_AUTO, (pAPI->GetOption(OPT_VIEWER_Auto) != 0) ? MF_CHECKED : MF_UNCHECKED);
			m_OptionsMenu.CheckMenuItem(IDS_SYNCWITHPLAYER, m_bSyncWithPlayer ? MF_CHECKED : MF_UNCHECKED);
			RECT rc;
			GetDlgItem(wParam)->GetWindowRect(&rc);
			m_OptionsMenu.TrackPopupMenu(TPM_LEFTALIGN, rc.left + ((rc.right - rc.left) / 2), rc.bottom, this);
			return 0;
		}
		break;
	case IDS_SPECIALINFO:
		SetMode(MOD_NormalInfo);
		break;
	case IDS_VISUALIZATIONSVIDEO:
		SetMode(MOD_Visualizations);
		break;
	case IDS_BROWSER:
		SetMode(MOD_Browser);
		break;
	case IDS_AUTO:
		pAPI->SetOption(OPT_VIEWER_Auto, pAPI->GetOption(OPT_VIEWER_Auto) ? 0 : 1);
		break;
	case IDS_SYNCWITHPLAYER:
		m_bSyncWithPlayer = !m_bSyncWithPlayer;
		break;
	default:
		return CTeenSpiritPane::OnCommand(wParam, lParam);
	}
	return 0;
}

BEGIN_MESSAGE_MAP(CViewerPane, CTeenSpiritPane)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CViewerPane::OnSize(UINT nType, int cx, int cy)
{
	CTeenSpiritPane::OnSize(nType, cx, cy);
	if (m_hWnd != 0 && m_bInitialized == FALSE )
	{
		m_bInitialized = TRUE;
		OnInit();
	}
}
