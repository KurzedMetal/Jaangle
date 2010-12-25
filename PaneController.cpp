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
#include "PaneController.h"
#include "PrgAPI.h"
#include "AppSettings/AppSettings.h"
#include "AddPaneDlg.h"

//=== Internal Panes
#include "SectionsPaneCreator.h"
#include "TrackListPaneCreator.h"
#include "PlayListPaneCreator.h"
#include "InfoPaneCreator.h"
#include "VisualizationPaneCreator.h"
#include "VideoPaneCreator.h"
#include "WebPaneCreator.h"


#define CMD_ALTERNATIVELAYOUT 0x700


static const LPCTSTR cPaneSection = _T("Panes");
static const LPCTSTR cState = _T("State");
static const LPCTSTR cPaneState = _T("PaneState");


class TSPaneState:public IPaneState
{
public:
	TSPaneState(AppSettings* pAppSettings, LPCTSTR sectionName):
		m_pAppSettings(pAppSettings),
		m_sectionName(sectionName)
	{
		ASSERT(m_pAppSettings != NULL);
		ASSERT(!m_sectionName.empty());
	}
	virtual ~TSPaneState()					{}

	virtual INT GetIntSetting(LPCTSTR settingName)
	{
		ASSERT(settingName != NULL);
		INT val = 0;
		m_pAppSettings->Read(m_sectionName.c_str(), settingName, val, -1);
		return val;
	}
	virtual void SetIntSetting(LPCTSTR settingName, INT value)
	{
		ASSERT(settingName != NULL);
		m_pAppSettings->Write(m_sectionName.c_str(), settingName, value);
	}

	virtual LPCTSTR GetLPCTSTRSetting(LPCTSTR settingName)
	{
		ASSERT(settingName != NULL);
		m_tempValue.clear();
		m_pAppSettings->Read(m_sectionName.c_str(), settingName, m_tempValue, _T(""));
		return m_tempValue.c_str();
	}
	virtual void SetLPCTSTRSetting(LPCTSTR settingName, LPCTSTR value)
	{
		ASSERT(settingName != NULL && value != NULL);
		m_pAppSettings->Write(m_sectionName.c_str(), settingName, value);
	}


private:
	AppSettings* m_pAppSettings;
	std::tstring m_sectionName;
	std::tstring m_tempValue;
};


PaneController::PaneController():
m_pParent(NULL),
m_unusedPaneContainer(NULL),
m_pMaximizedPane(NULL),
m_pMaximizedPaneSplitter(NULL)
{
}

PaneController::~PaneController()
{
	delete m_unusedPaneContainer;
	m_unusedPaneContainer = NULL;
	{
		std::vector<TSPaneContainer*>::iterator it = m_paneContainers.begin();
		for (;it != m_paneContainers.end(); it++)
		{
			delete *it;
		}
	}
	{
		std::vector<CFastSplitter*>::iterator it = m_splitters.begin();
		for (;it != m_splitters.end(); it++)
		{
			delete *it;
		}
	}
	{
		std::vector<IPaneCreator*>::iterator it = m_paneCreators.begin();
		for (;it != m_paneCreators.end(); it++)
		{
			delete *it;
		}
	}
	//delete m_pmnuAdd;
}



void PaneController::Init(CWnd* pParent)
{
	m_pParent = pParent;

	//=== CREATE PANES
	AddPaneCreator(new SectionsPaneCreator);
	AddPaneCreator(new PlayListPaneCreator);
	AddPaneCreator(new TrackListPaneCreator);
	AddPaneCreator(new InfoPaneCreator);
	AddPaneCreator(new VisualizationPaneCreator);
	AddPaneCreator(new VideoPaneCreator);
	AddPaneCreator(new WebPaneCreator);

	//=== TODO. enum dll's and get the Panes from there

	PrgAPI* pAPI = PRGAPI();
	AppSettings* pAppSettings = pAPI->GetAppSettings();
	std::tstring setting;
	pAppSettings->Read(cPaneSection, cState, setting, L"");
	//TCHAR bf[5000];
	//pAPI->GetStringOption(SOPT_PANES_Sorting, bf, 5000);
	if (LoadState(setting.c_str()))
	{
		std::vector<TSPaneContainer*>::iterator it = m_paneContainers.begin();
		for (UINT i = 0; i < m_paneContainers.size(); i++)
		{
			IPane* pPane = m_paneContainers[i]->GetContainedPane();
			if (pPane != NULL)
			{
				TCHAR bf[100];
				_sntprintf(bf, 100, _T("%s#%d"), cPaneState, i);
				TSPaneState paneState(pAppSettings, bf);
				if (pPane->LoadState(paneState) == FALSE)
					TRACE(_T("PaneController::Init Load Pane (%d.%s) State failed\r\n"), i, pPane->GetTitle(0));
				pPane->Show();
			}
		}
	}
	else
	{
		TRACE(_T("PaneController::Init LoadState failed\r\n"));
		LoadDefaultState();
	}
}

BOOL PaneController::AddPane(TSPaneContainer& srcPaneContainer)
{
	CAddPaneDlg dlg(this, m_pParent);
	if (dlg.DoModal() == IDOK)
	{
		return AddPane(srcPaneContainer, *dlg.GetPaneCreator(), dlg.GetPosition(), -1);  
	}
	return FALSE;

}

BOOL PaneController::AddPane(TSPaneContainer& srcPaneContainer, IPaneCreator& newPaneCreator, Position pos, INT splitterPos/* = -1*/)
{
	delete m_unusedPaneContainer;
	m_unusedPaneContainer = NULL;
	//=== Find in which splitter the splitPane belongs
	std::vector<CFastSplitter*>::iterator it = m_splitters.begin();
	CFastSplitter* pOwnerSplitter = NULL;
	BOOL bIsFirstPane = FALSE;
	for (;it != m_splitters.end(); it++)
	{
		if ((*it)->GetFirstPane()->GetSafeHwnd() == srcPaneContainer.GetSafeHwnd())
		{
			bIsFirstPane = TRUE;
			pOwnerSplitter = *it;
			break;
		}
		if ((*it)->GetSecondPane()->GetSafeHwnd() == srcPaneContainer.GetSafeHwnd())
		{
			bIsFirstPane = FALSE;
			pOwnerSplitter = *it;
			break;
		}
	}
	BOOL bVertical = TRUE;
	if (pos == POS_Down || pos == POS_Up)
		bVertical = FALSE;
	//===
	if (splitterPos == -1)
	{
		CRect rc;
		srcPaneContainer.GetClientRect(&rc);
		splitterPos = (bVertical ? rc.Width() : rc.Height()) / 2;
	}
	TSPaneContainer* pFirstPane = NULL, *pSecondPane = NULL;
	TSPaneContainer* pNewSlot = CreateContainerPane(&newPaneCreator);
	if (pNewSlot == NULL)
		return FALSE;
	if (pos == POS_Up || pos == POS_Left)
	{
		pFirstPane = pNewSlot;
		pSecondPane = &srcPaneContainer;
	}
	else
	{
		pFirstPane = &srcPaneContainer;
		pSecondPane = pNewSlot;
	}
	CFastSplitter* pSP = CreateSplitter(srcPaneContainer.GetParent(), pFirstPane, pSecondPane, bVertical, splitterPos);
	if (pOwnerSplitter != NULL)
	{
		if (bIsFirstPane)
			pOwnerSplitter->SetFirstPane(pSP);
		else
			pOwnerSplitter->SetSecondPane(pSP);
		pOwnerSplitter->SetupWindows();
	}
	else
	{
		pSP->MoveWindow(&m_clientRC);
	}
	return pNewSlot->GetContainedPane()->Show();
}

BOOL PaneController::FindPanePosition(PanePositinion& pos, HWND hwnd)
{
	for (UINT i = 0; i < m_splitters.size(); i++)
	{
		CFastSplitter* pSP = m_splitters[i];
		if (pSP->GetFirstPane()->GetSafeHwnd() == hwnd)
		{
			pos.ownerSplitterIndex = i;
			pos.bIsFirstPane = TRUE;
			return TRUE;
		}
		else if (pSP->GetSecondPane()->GetSafeHwnd() == hwnd)
		{
			pos.ownerSplitterIndex = i;
			pos.bIsFirstPane = FALSE;
			return TRUE;
		}
	}
	return FALSE;
}

void PaneController::MaxRestorePane(TSPaneContainer& srcPaneContainer)
{
	if (m_pMaximizedPane == NULL)
	{
		//=== Maximize Pane
		PanePositinion pos;
		if (FindPanePosition(pos, srcPaneContainer.GetSafeHwnd()))
		{
			m_pMaximizedPaneSplitter = m_splitters[pos.ownerSplitterIndex];
			srcPaneContainer.SetParent(m_pParent);
			srcPaneContainer.SetMaximizedState(TRUE);
			//srcPaneContainer.AddButton(SB_CONST_ALPHA)
			m_pMaximizedPane = &srcPaneContainer;
			m_pMaximizedPane->MoveWindow(m_clientRC, TRUE);
			if (m_splitters.size() > 0)
				m_splitters[0]->ShowWindow(SW_HIDE);
		}

		//m_pMaximizedPaneOriginalParent = srcPaneContainer.GetParent();
		//srcPaneContainer.SetParent(m_pParent);
		//m_pMaximizedPane = &srcPaneContainer;
		//m_pMaximizedPane->MoveWindow(m_clientRC, TRUE);
		//if (m_splitters.size() > 0)
		//	m_splitters[0]->ShowWindow(SW_HIDE);
	}
	else
	{
		if (&srcPaneContainer == m_pMaximizedPane)
		{
			//=== Restore Operation
			m_pMaximizedPane = NULL;
			srcPaneContainer.SetMaximizedState(FALSE);
			if (m_splitters.size() > 0)
			{
				m_splitters[0]->ShowWindow(SW_SHOW);
				SetPosition(m_clientRC.left, m_clientRC.top, m_clientRC.Width(), m_clientRC.Height());
				srcPaneContainer.SetParent(m_pMaximizedPaneSplitter);
				m_pMaximizedPaneSplitter->SetupWindows();

			}
		}
		else
		{
			//=== Restore the m_pMaximizedPane
			MaxRestorePane(*m_pMaximizedPane);
			//=== Maximize this pane
			MaxRestorePane(srcPaneContainer);
		}
	}


}


void PaneController::ClosePane(TSPaneContainer& srcPane)
{
	delete m_unusedPaneContainer;
	m_unusedPaneContainer = NULL;
	if (&srcPane == m_pMaximizedPane)
		MaxRestorePane(srcPane);
	PanePositinion pos;
	if (FindPanePosition(pos, srcPane.GetSafeHwnd()))
	{
		CFastSplitter* pParentSplitter = NULL;
		CFastSplitter* pSplitter = m_splitters[pos.ownerSplitterIndex];
		CWnd* pOtherPane = pSplitter->GetFirstPane();
		if (pos.bIsFirstPane)
			pOtherPane = pSplitter->GetSecondPane();

		//=== 1. Change the splitter associations 
		//			(Set the "other" pane in the place of the parent's splitter pane
		PanePositinion parentPos;
		if (FindPanePosition(parentPos, pSplitter->GetSafeHwnd()))
		{
			//=== Set the "other" pane
			pParentSplitter = m_splitters[parentPos.ownerSplitterIndex];
			if (parentPos.bIsFirstPane)
				pParentSplitter->SetFirstPane(pOtherPane);
			else
				pParentSplitter->SetSecondPane(pOtherPane);
			pOtherPane->SetParent(pParentSplitter);
			pParentSplitter->SetSplitterPos(pParentSplitter->GetSplitterPos());
		}
		else
		{
			//=== There is not parent splitter
			if (pOtherPane != NULL)
				pOtherPane->SetParent(m_pParent);
		}
		//=== Remove pane from the m_paneContainers and add it to m_unusedPaneContainers for latet destruction
		//===	You need this because sometimes you call it from the same object (e.g ClosePane(*this)); 
		srcPane.SetContainedPane(NULL);
		if (srcPane.GetSafeHwnd() != 0)
			srcPane.DestroyWindow();
		m_unusedPaneContainer = &srcPane;
		std::vector<TSPaneContainer*>::iterator it = std::find(m_paneContainers.begin(), m_paneContainers.end(), &srcPane);
		if (it != m_paneContainers.end())
			m_paneContainers.erase(it);

		//=== 3. Delete the splitter  & remove it from the splitter collection
		m_splitters.erase(m_splitters.begin() + pos.ownerSplitterIndex);
		pSplitter->SetFirstPane(NULL);
		pSplitter->SetSecondPane(NULL);
		pSplitter->DestroyWindow();
		delete pSplitter;

		//=== 4. Fix "Close" button
		m_paneContainers[0]->EnableCloseButton(m_paneContainers.size() > 1);

		SetPosition(m_clientRC.left, m_clientRC.top, m_clientRC.Width(), m_clientRC.Height());
		if (pParentSplitter != NULL)
			pParentSplitter->SetupWindows();




	}
	else
	{
		//=== The current configuration appears to have no splitters at all
		ASSERT(m_splitters.empty());
		srcPane.SetContainedPane(NULL);
	}

}


struct SplitterInfo
{
	SplitterInfo()
	{
		memset(this, 0, sizeof(SplitterInfo));
	}
	BOOL bVertical;
	UINT splitterPos;

	BOOL FPaneSplitter;
	UINT FPaneIndex;
	BOOL SPaneSplitter;
	UINT SPaneIndex;
};


BOOL PaneController::GetToken(LPCTSTR& posStart, TCHAR* bf, UINT bfLen, WCHAR divider)
{
	if (posStart == 0)
		return FALSE;
	UINT len = 0;
	LPCTSTR posEnd = _tcschr(posStart, divider);
	LPCTSTR nextStart = NULL;
	if (posEnd != NULL)
	{
		len = posEnd - posStart;
		nextStart = posEnd + 1;
	}
	else
	{
		len = _tcslen(posStart);
		nextStart = &posStart[len];
	}
	if (len < bfLen)
	{
		_tcsncpy(bf, posStart, len);
		bf[len] = 0;
		posStart = nextStart;
		return TRUE;
	}

	return FALSE;

}

void PaneController::LoadDefaultState()
{
	//=== Close All Panes
	TRACE(_T("PaneController::LoadDefaultState Clearing all panes\r\n"));
	{
		std::vector<TSPaneContainer*>::iterator it = m_paneContainers.begin();
		for (;it != m_paneContainers.end(); it++)
		{
			delete *it;
		}
		m_paneContainers.clear();
	}
	{
		std::vector<CFastSplitter*>::iterator it = m_splitters.begin();
		for (;it != m_splitters.end(); it++)
		{
			delete *it;
		}
		m_splitters.clear();
	}
	

	TRACE(_T("PaneController::LoadDefaultState Creating\r\n"));
	//=== Add Initial Slot
	IPaneCreator* pSectionsPaneCreator = GetPaneCreatorByName(_T("SectionsPane"));
	IPaneCreator* pTrackListPaneCreator = GetPaneCreatorByName(_T("TrackListPane"));
	IPaneCreator* pPlayListPaneCreator = GetPaneCreatorByName(_T("PlayListPane"));
	IPaneCreator* pInfoPaneCreator = GetPaneCreatorByName(_T("InfoPane"));

	//=== Test #1
	//TSPaneContainer* pSlot = CreateContainerPane(pSectionsPane);
	//SplitPane(*pSlot, *pPlayListPane, TRUE, 400);

	////=== Test Add some Panes
	TSPaneContainer* pSlot = CreateContainerPane(pSectionsPaneCreator);
	AddPane(*m_paneContainers[0], *pTrackListPaneCreator, POS_Right, 300);
	AddPane(*m_paneContainers[0], *pPlayListPaneCreator, POS_Down, 300);
	AddPane(*m_paneContainers[1], *pInfoPaneCreator, POS_Down, 300);

	std::vector<TSPaneContainer*>::iterator it = m_paneContainers.begin();
	for (; it != m_paneContainers.end(); it++)
		(*it)->GetContainedPane()->Show();

}

BOOL PaneController::LoadState(LPCTSTR state)
{
	if (state == NULL)
		return FALSE;
	TCHAR bf[200];
	LPCTSTR posStart = state;
	//=== Get number of panes
	if (GetToken(posStart, bf, 200, L',') == FALSE)
		return FALSE;
	INT panes = _ttoi(bf);
	if (panes <= 0)
		return FALSE;
	//=== Create Panes
	for (INT i = 0; i < panes; i++)
	{
		if (GetToken(posStart, bf, 200, L',') == FALSE)
			return FALSE;
		IPaneCreator* pPaneCreator = GetPaneCreatorByName(bf);
		if (pPaneCreator == NULL)
			return FALSE;
		//if (GetToken(paneStateStart, paneState, 1000, L'|'))
		//	pPane->LoadState(paneState);
		TSPaneContainer* pCPane = CreateContainerPane(pPaneCreator);
	}

	//=== Collect Splitter info
	INT splitters = panes - 1;
	if (splitters <= 0)
		return TRUE;
	std::vector<SplitterInfo> spInfo;
	spInfo.resize(splitters);

	for (INT i = 0; i < splitters; i++)
	{
		//=== Splitters must have textual representation like "v345,s1,p1"
		//=== Vertical property
		if (GetToken(posStart, bf, 200, L',') == FALSE)
			return FALSE;
		switch (bf[0])
		{
		case L'v':
			spInfo[i].bVertical = TRUE;
			break;
		case L'h':
			spInfo[i].bVertical = FALSE;
			break;
		default:
			return FALSE;
		}

		//=== Splitter Pos
		INT splitterPos = _ttoi(&bf[1]);
		if (splitterPos <0 || splitterPos >=2000)
			return FALSE;
		spInfo[i].splitterPos = splitterPos;

		if (GetToken(posStart, bf, 200, L',') == FALSE)
			return FALSE;

		INT idx = _ttoi(&bf[1]);
		if (bf[0] == L'p')
		{
			if (idx < 0 || idx >= panes)
				return FALSE;
			spInfo[i].FPaneSplitter = FALSE;
		}
		else if (bf[0] == L's')
		{
			if (idx < 0 || idx >= splitters)
				return FALSE;
			spInfo[i].FPaneSplitter = TRUE;
		}
		else
			return FALSE;
		spInfo[i].FPaneIndex = idx;

		if (GetToken(posStart, bf, 200, L',') == FALSE)
			return FALSE;

		idx = _ttoi(&bf[1]);
		if (bf[0] == L'p')
		{
			if (idx < 0 || idx >= panes)
				return FALSE;
			spInfo[i].SPaneSplitter = FALSE;
		}
		else if (bf[0] == L's')
		{
			if (idx < 0 || idx >= splitters)
				return FALSE;
			spInfo[i].SPaneSplitter = TRUE;
		}
		else
			return FALSE;
		spInfo[i].SPaneIndex = idx;
	}

	//=== Create Splitters
	m_splitters.resize(splitters);
	UINT createdPanes = 0;
	INT j = 0;
	TSSkin& skin = PRGAPI()->GetSkinManager()->GetSkin();
	while (createdPanes < spInfo.size())
	{
		if (m_splitters[j] == NULL)
		{
			CWnd* pFPane = NULL;
			if (spInfo[j].FPaneSplitter)
				pFPane = m_splitters[spInfo[j].FPaneIndex];
			else
				pFPane = m_paneContainers[spInfo[j].FPaneIndex];
			CWnd* pSPane = NULL;
			if (spInfo[j].SPaneSplitter)
				pSPane = m_splitters[spInfo[j].SPaneIndex];
			else
				pSPane = m_paneContainers[spInfo[j].SPaneIndex];

			if (pFPane != NULL && pSPane != NULL)
			{
				CFastSplitter* pSP = new CFastSplitter();
				pSP->Create(WS_VISIBLE, m_pParent, pFPane, pSPane, 511, 100, 100, spInfo[j].bVertical, skin.GetIntParameter(CSEC_Splitters, IPARAM_Size));
				pSP->SetSplitterPos(spInfo[j].splitterPos);
				ApplySkinToSplitters(*pSP, skin);
				m_splitters[j] = pSP;
				createdPanes++;
			}
		}
		j++;
		if (j == spInfo.size())
			j = 0;
	}
	return TRUE;
}

BOOL PaneController::AppendPaneState(std::tstring& state, HWND hwndPane)
{
	TCHAR bf[50];
	INT idx = GetContainerIndexByHWND(hwndPane);
	if (idx != -1)
	{
		_sntprintf(bf, 50, _T("p%d"), idx);
		state += bf;
	}
	else
	{
		idx = GetSplitterIndexByHWND(hwndPane);
		if (idx == -1)
			return FALSE;
		_sntprintf(bf, 50, _T("s%d"), idx);
		state += bf;
	}
	return TRUE;
}

BOOL PaneController::SaveState(LPTSTR state, UINT stateLen)
{
	//=== Save the number of visible panes
	TCHAR bf[200];
	_sntprintf(bf, 200, _T("%d"), m_paneContainers.size());
	std::tstring s(bf);


	std::vector<TSPaneContainer*>::iterator it = m_paneContainers.begin();
	for (; it != m_paneContainers.end(); it++)
	{
		s += L",";
		IPane* pPane = (*it)->GetContainedPane();
		if (pPane != NULL)
			s += pPane->GetPaneInfo().Name;
	}



	//=== Save the Splitter Information
	std::vector<CFastSplitter*>::iterator sit = m_splitters.begin();
	for (;sit != m_splitters.end(); sit++)
	{
		s += _T(",");
		//=== Every Splitter will have textual representations like "v120,s1,p2"
		CFastSplitter* pSp = *sit;
		_sntprintf(bf, 200, _T("%s%d,"), 
			pSp->IsVerticalSplitter() ? _T("v") : _T("h"), 
			pSp->GetSplitterPos());
		s += bf;
		if (AppendPaneState(s, pSp->GetFirstPane()->GetSafeHwnd()) == FALSE)
			return FALSE;
		s += _T(",");
		if (AppendPaneState(s, pSp->GetSecondPane()->GetSafeHwnd()) == FALSE)
			return FALSE;
	}
	if (stateLen <= s.size())
		return FALSE;
	_tcscpy(state, s.c_str());
	return TRUE;
}

INT PaneController::GetContainerIndexByHWND(HWND hwnd)
{
	for (UINT i = 0; i < m_paneContainers.size(); i++)
	{
		if (m_paneContainers[i]->GetSafeHwnd() == hwnd)
			return (INT)i;
	}
	return -1;
}
INT PaneController::GetSplitterIndexByHWND(HWND hwnd)
{
	for (UINT i = 0; i < m_splitters.size(); i++)
	{
		if (m_splitters[i]->GetSafeHwnd() == hwnd)
			return (INT)i;
	}
	return -1;
}

void PaneController::AddPaneCreator(IPaneCreator* pPane)
{
	m_paneCreators.push_back(pPane);
}

//CFastSplitter* PaneController::GetSplitterByPaneContainer(TSPaneContainer* pPane)
//{
//	HWND hwnd = pPane->GetSafeHwnd();
//	std::vector<CFastSplitter*>::iterator it= m_splitters.begin();
//	for (;it != m_splitters.end(); it++)
//	{
//		if ((*it)->GetFirstPane()->GetSafeHwnd() == hwnd ||
//			(*it)->GetSecondPane()->GetSafeHwnd() == hwnd)
//			return *it;
//	}
//	return NULL;
//}



TSPaneContainer* PaneController::CreateContainerPane(IPaneCreator* pPaneCreator)
{
	ASSERT(pPaneCreator != NULL);
	TSPaneContainer* pSlot = new TSPaneContainer(this);
	m_paneContainers.push_back(pSlot);
	pSlot->Create(m_pParent, 200 + m_paneContainers.size());
	m_paneContainers[0]->EnableCloseButton(m_paneContainers.size() > 1);

	if (pPaneCreator != NULL)
	{
		pSlot->SetContainedPane(pPaneCreator->CreateInstance());
		ApplySkinToPanes(*pSlot, PRGAPI()->GetSkinManager()->GetSkin());
	}
	return pSlot;
}

CFastSplitter* PaneController::CreateSplitter(CWnd* pParent, CWnd* pFPane, CWnd* pSPane, BOOL bVertical, UINT splitterPos)
{
	TSSkin& skin =  PRGAPI()->GetSkinManager()->GetSkin();
	CFastSplitter* pSP = new CFastSplitter();
	m_splitters.push_back(pSP);
	pSP->Create(WS_VISIBLE, pParent, pFPane, pSPane, 511, 100, 100, bVertical, skin.GetIntParameter(CSEC_Splitters, IPARAM_Size));
	pSP->SetSplitterPos(splitterPos);
	ApplySkinToSplitters(*pSP, skin);
	return pSP;
}

//BOOL PaneController::SplitPane(TSPaneContainer* pOriginalPane, TSPaneContainer* pNewPane, BOOL bVertical, INT splitterPos)
//{
//	//=== Check if this is the first pane. No real need to split anything
//	if (splitPane.GetContainedPane() == NULL)
//	{
//		splitPane.SetContainedPane(&newPane);
//		return TRUE;
//	}
//
//	//=== Find in which splitter the splitPane belongs
//	std::vector<CFastSplitter*>::iterator it = m_splitters.begin();
//	CFastSplitter* pOwnerSplitter = NULL;
//	BOOL bIsFirstPane = FALSE;
//	for (;it != m_splitters.end(); it++)
//	{
//		if ((*it)->GetFirstPane()->GetSafeHwnd() == splitPane.GetSafeHwnd())
//		{
//			bIsFirstPane = TRUE;
//			pOwnerSplitter = *it;
//			break;
//		}
//		if ((*it)->GetSecondPane()->GetSafeHwnd() == splitPane.GetSafeHwnd())
//		{
//			bIsFirstPane = FALSE;
//			pOwnerSplitter = *it;
//			break;
//		}
//	}
//
//	//===
//
//	TSPaneContainer* pSlot = CreateContainerPane(&newPane);
//	CFastSplitter* pSP = CreateSplitter(splitPane.GetParent(), &splitPane, pSlot, bVertical, splitterPos);
//	if (pOwnerSplitter != NULL)
//	{
//		if (bIsFirstPane)
//			pOwnerSplitter->SetFirstPane(pSP);
//		else
//			pOwnerSplitter->SetSecondPane(pSP);
//	}
//	return TRUE;
//}

IPaneCreator* PaneController::GetPaneCreatorByName(LPCTSTR name)
{
	std::vector<IPaneCreator*>::iterator it = m_paneCreators.begin();
	for (;it != m_paneCreators.end(); it++)
	{
		if (_tcsicmp(name, (*it)->GetPaneInfo().Name) == 0)
			return *it;
	}
	return NULL;
}


IPaneCreator* PaneController::GetPaneCreator(UINT idx)
{
	ASSERT(idx < m_paneCreators.size());
	if (idx < m_paneCreators.size())
		return m_paneCreators[idx];
	return NULL;
}


//TSPaneContainer* PaneController::AddSlot(UINT paneIndex)
//{
//	TSPaneContainer* pSlot = new TSPaneContainer;
//	pSlot->SetContainedPane(GetPane(paneIndex));
//	pSlot->Init(this, m_pParent->m_hWnd);
//	m_paneContainers.push_back(pSlot);
//	ApplySkinToPanes(*pSlot, PRGAPI()->GetSkinManager()->GetSkin());
//	return pSlot;
//}
//
//void PaneController::AddPane(IPane* pPane)
//{
//	std::vector<IPane*>::iterator it = std::find(m_panes.begin(), m_panes.end(), pPane);
//	if (it == m_panes.end())
//		m_panes.push_back(pPane);
//}
//
//UINT PaneController::GetPaneCount()
//{
//	return m_panes.size();
//}
//
//IPane* PaneController::GetPane(UINT idx)
//{
//	if (idx < m_panes.size())
//		return m_panes[idx];
//	return NULL;
//}
//
//CFastSplitter* PaneController::AddSplitter(TSPaneContainer* pParentSlot, UINT paneIndex, BOOL bVertical)
//{
//	TSPaneContainer* pSlot = AddSlot(paneIndex);
//
//	CFastSplitter* pSplitter = new CFastSplitter;
//	m_splitters.push_back(pSplitter);
//	pSplitter->Create(0, pParentSlot->GetParent(), pParentSlot, pSlot, 100, 100, 100, bVertical, 5);
//	pParentSlot->SetParent(pSplitter);
//	pSlot->SetParent(pSplitter);
//	pSlot->Show();
//	pSplitter->ShowWindow(SW_SHOW);
//	return pSplitter;
//}
//
//CFastSplitter* PaneController::GetSplitter(UINT idx)
//{
//	if (idx < m_splitters.size())
//		return m_splitters[idx];
//	return NULL;
//}
//
//UINT PaneController::GetSplitterCount()
//{
//	return m_splitters.size();
//}

//INT PaneController::GetPanePos(IPane* pPane)
//{
//	std::vector<TSPaneContainer*>::iterator it = m_paneContainers.begin();
//	INT ret = 0;
//	for (;it != m_paneContainers.end(); it++)
//	{
//		if (pPane == (*it)->GetContainedPane())
//			return ret;
//		ret++;
//	}
//	return -1;
//
//}
//
//void PaneController::SetPanePos(IPane* pPane, INT pos)
//{
//	if (pos >= (INT)m_paneContainers.size())
//	{
//		ASSERT(0);//=== Illegal slot
//		return;
//	}
//	INT oldPos = GetPanePos(pPane);
//	if (oldPos != -1)
//	{
//		//=== This pane already exists in a different slot. We will switch them
//		m_paneContainers[oldPos]->SetContainedPane(m_paneContainers[pos]->GetContainedPane());
//	}
//	m_paneContainers[pos]->SetContainedPane(pPane);
//}



//void PaneController::SetPaneState(IPane& pane, PaneState ps)
//{
//	FlexButton* pMax = (FlexButton*)pane.GetDlgItem(SB_Max);
//	if (pMax != NULL)
//	{
//		switch (ps)
//		{
//		case PS_Normal:
//			pMax->SetIcon(PRGAPI()->GetIcon(ICO_Maximize16),16,16);
//			break;
//		case PS_Maximized:
//			pMax->SetIcon(PRGAPI()->GetIcon(ICO_Restore16),16,16);
//			break;
//		}
//	}
//	else
//		ASSERT(0);
//}
//void PaneController::SetSplitterMode(CFastSplitter::MODE mode, CFastSplitter& splitter)
//{
//	IPane* pFPane = (IPane*) splitter.GetFirstPane();
//	IPane* pSPane = (IPane*) splitter.GetSecondPane();
//	switch (mode)
//	{
//	case CFastSplitter::MOD_FirstMaximized:
//		SetPaneState(*pFPane, PS_Maximized);
//		break;
//	case CFastSplitter::MOD_SecondMaximized:
//		SetPaneState(*pSPane, PS_Maximized);
//		break;
//	case CFastSplitter::MOD_Normal:
//		SetPaneState(*pFPane, PS_Normal);
//		SetPaneState(*pSPane, PS_Normal);
//		break;
//	}
//	splitter.SetMode(mode);
//}


#define SPLITTER_START_ID			300
#define SPLITTER_CONSTRAINT_SIZE	100
LPCTSTR PaneController::SettingsCategory = _T("MAINDLG");


//BOOL PaneController::SetupWindows()
//{
//	if (m_pParent == NULL)
//		return FALSE;
//	if (m_panes.size() < 4)
//		return FALSE;
//	//BOOL bFirstTime = (m_splitter[SPL_Main].m_hWnd == 0);
//	//if (bFirstTime)
//	//{
//	//	m_splitter[SPL_Main].Create(WS_CHILD|WS_VISIBLE, m_pParent, SPLITTER_START_ID + SPL_Main);
//	//	m_splitter[SPL_First].Create(WS_CHILD|WS_VISIBLE, &m_splitter[SPL_Main], SPLITTER_START_ID + SPL_First);
//	//	m_splitter[SPL_Second].Create(WS_CHILD|WS_VISIBLE, &m_splitter[SPL_Main], SPLITTER_START_ID + SPL_Second);
//	//	m_splitter[SPL_Main].SetConstraintSize(SPLITTER_CONSTRAINT_SIZE);
//	//	m_splitter[SPL_First].SetConstraintSize(SPLITTER_CONSTRAINT_SIZE);
//	//	m_splitter[SPL_Second].SetConstraintSize(SPLITTER_CONSTRAINT_SIZE);
//
//	//}
//	//m_splitter[SPL_Main].SetVerticalSplitter(!m_bHorizontal);
//	//m_splitter[SPL_Main].SetFirstPane(&m_splitter[SPL_First]);
//	//m_splitter[SPL_Main].SetSecondPane(&m_splitter[SPL_Second]);
//	//m_splitter[SPL_First].SetVerticalSplitter(m_bHorizontal);
//	//m_splitter[SPL_First].SetFirstPane(m_panes[0]);
//	//m_splitter[SPL_First].SetSecondPane(m_panes[1]);
//	//m_splitter[SPL_Second].SetVerticalSplitter(m_bHorizontal);
//	//m_splitter[SPL_Second].SetFirstPane(m_panes[2]);
//	//m_splitter[SPL_Second].SetSecondPane(m_panes[3]);
//	//m_splitter[SPL_Main].SetSplitterPos(m_splitter[SPL_Main].GetSplitterPos());
//	//m_splitter[SPL_First].SetSplitterPos(m_splitter[SPL_First].GetSplitterPos());
//	//m_splitter[SPL_Second].SetSplitterPos(m_splitter[SPL_Second].GetSplitterPos());
//
//	//if (bFirstTime)
//	//{
//	//	PrgAPI* pAPI = PRGAPI();
//	//	INT value = 0;
//	//	AppSettings* appSettings = pAPI->GetAppSettings();
//	//	appSettings->Read(SettingsCategory, _T("MainSplitter"), value, 300);
//	//	m_splitter[SPL_Main].SetSplitterPos(value);
//	//	appSettings->Read(SettingsCategory, _T("TreeSplitter"), value, 300);
//	//	m_splitter[SPL_First].SetSplitterPos(value);
//	//	appSettings->Read(SettingsCategory, _T("TreeSplitterMode"), value, CFastSplitter::MOD_Normal);
//	//	//m_splitter[SPL_First].SetMode(CFastSplitter::MODE(value));
//	//	SetSplitterMode(CFastSplitter::MODE(value), m_splitter[SPL_First]);
//	//	appSettings->Read(SettingsCategory, _T("ListSplitter"), value, 300);
//	//	m_splitter[SPL_Second].SetSplitterPos(value);
//	//	appSettings->Read(SettingsCategory, _T("ListSplitterMode"), value, CFastSplitter::MOD_Normal);
//	//	//m_splitter[SPL_Second].SetMode(CFastSplitter::MODE(value));
//	//	SetSplitterMode(CFastSplitter::MODE(value), m_splitter[SPL_Second]);
//	//	ApplySkin(pAPI->GetSkinManager()->GetSkin());
//	//}
//
//	return TRUE;
//	
//}



void PaneController::SetPosition(INT x, INT y, INT cx, INT cy)
{
	m_clientRC = CRect(x, y , x + cx, y + cy);
	if (m_pMaximizedPane != NULL)
	{
		m_pMaximizedPane->MoveWindow(x, y, cx, cy, FALSE);
	}
	else
	{
		if (m_splitters.size() > 0)
			m_splitters[0]->MoveWindow(x, y, cx, cy, FALSE);
		else if (m_paneContainers.size() > 0)
		{
			if (m_paneContainers[0]->GetSafeHwnd() != 0)
				m_paneContainers[0]->MoveWindow(x, y, cx, cy, FALSE);
		}
	}
}

BOOL PaneController::SaveSettings()
{
	PrgAPI* pAPI = PRGAPI();
	AppSettings* pAppSettings = pAPI->GetAppSettings();
	for (UINT i = 0; i < m_paneContainers.size(); i++)
	{
		IPane* pPane = m_paneContainers[i]->GetContainedPane();
		if (pPane != NULL)
		{
			TCHAR bf[100];
			_sntprintf(bf, 100, _T("%s#%d"), cPaneState, i);
			TSPaneState paneState(pAppSettings, bf);
			if (pPane->SaveState(paneState) == FALSE)
				TRACE(_T("PaneController::Init Save Pane State (%d.%s) State failed\r\n"), i, pPane->GetPaneInfo().friendlyName);
		}
	}

	TCHAR bf[1000];
	if (SaveState(bf, 1000))
		pAppSettings->Write(cPaneSection, cState, bf);


	//AppSettings* pAppSettings = PRGAPI()->GetAppSettings();
	//pAppSettings->Write(SettingsCategory, _T("TreeSplitter"), m_splitter[SPL_First].GetSplitterPos());
	//pAppSettings->Write(SettingsCategory, _T("ListSplitter"), m_splitter[SPL_Second].GetSplitterPos());
	//pAppSettings->Write(SettingsCategory, _T("MainSplitter"), m_splitter[SPL_Main].GetSplitterPos());
	//pAppSettings->Write(SettingsCategory, _T("TreeSplitterMode"), m_splitter[SPL_First].GetMode());
	//pAppSettings->Write(SettingsCategory, _T("ListSplitterMode"), m_splitter[SPL_Second].GetMode());

	//PaneCollection::iterator it = m_panes.begin();
	//for (;it != m_panes.end(); it++)
	//	(*it)->SaveSettings();


	//PrgAPI* pAPI = PRGAPI();
	//pAPI->SetOption(OPT_PANES_PaneLayoutStyle, IsMainSplitHorizontal() ? 1 : 0);
	//std::tstring paneSorting;
	//for (UINT i = 0; i < GetPaneCount(); i++)
	//{
	//	if (paneSorting.empty() == FALSE)
	//		paneSorting += _T(",");
	//	INT paneID = GetPane(i)->GetID();
	//	TCHAR tmp[20];
	//	paneSorting += _itot(paneID, tmp, 10);
	//}
	//pAPI->SetStringOption(SOPT_PANES_Sorting, paneSorting.c_str());


	return TRUE;

}

void PaneController::ApplySkinToPanes(TSPaneContainer& pane, TSSkin& skin)
{
	pane.SetColor(TSPaneContainer::COL_Bk, skin.GetColor(CSEC_Panes, CVAL_Bk));
	pane.SetColor(TSPaneContainer::COL_Text, skin.GetColor(CSEC_Panes, CVAL_Text));
	pane.SetColor(TSPaneContainer::COL_CaptionBk, skin.GetColor(CSEC_Panes, CVAL_CaptionBk));
	pane.SetColor(TSPaneContainer::COL_CaptionText, skin.GetColor(CSEC_Panes, CVAL_CaptionText));
	pane.SetColor(TSPaneContainer::COL_CaptionOutline, skin.GetColor(CSEC_Panes, CVAL_CaptionOutline));

	pane.SetFont(skin.GetFont(CSEC_Panes, FSTY_Normal), FALSE);
	pane.SetCaptionHeight(skin.GetIntParameter(CSEC_Panes, IPARAM_Height) + skin.GetRelativeFontSize());

}
void PaneController::ApplySkinToSplitters(CFastSplitter& splitter, TSSkin& skin)
{
	splitter.SetColor(CFastSplitter::COL_Normal, skin.GetColor(CSEC_Splitters, CVAL_Bk));
	splitter.SetColor(CFastSplitter::COL_Disabled, skin.GetColor(CSEC_Splitters, CVAL_DisabledBk));
	splitter.SetColor(CFastSplitter::COL_Tracking, skin.GetColor(CSEC_Splitters, CVAL_OverBk));
	splitter.SetSplitterSize(skin.GetIntParameter(CSEC_Splitters, IPARAM_Size));
}


void PaneController::ApplySkin(ISkin& _skin)
{
	TSSkin& skin = (TSSkin&) _skin;

	//=== Colors - Panes
	//COLORREF clrSkinBk = RGB(0,0,0);
	//COLORREF clrSkinCaptionBk = RGB(0,0,0);
	//COLORREF clrSkinCaptionText = RGB(192,192,192);
	//pAPI->GetSkinColor(cPaneSection, _T("Bk"), clrSkinBk);
	//if (!pAPI->GetSkinColor(cPaneSection, _T("CaptionBk"), clrSkinCaptionBk))
	//	clrSkinCaptionBk = clrSkinBk;
	//pAPI->GetSkinColor(cPaneSection, _T("CaptionText"), clrSkinCaptionText);

	std::vector<TSPaneContainer*>::iterator it = m_paneContainers.begin();
	for (;it != m_paneContainers.end(); it++)
		ApplySkinToPanes(*(*it), skin);


	std::vector<CFastSplitter*>::iterator itSpl = m_splitters.begin();
	for (;itSpl != m_splitters.end(); itSpl++)
		ApplySkinToSplitters(*(*itSpl), skin);




}

void PaneController::ApplyTranslation(ITranslation& translation)
{
}




