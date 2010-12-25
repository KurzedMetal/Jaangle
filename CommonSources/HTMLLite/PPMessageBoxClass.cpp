// PPMessageBoxMFC.cpp : implementation file
//

#include "stdafx.h"
#include "PPMessageBoxClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPPMessageBox::CPPMessageBox()
{
}

int CPPMessageBox::MessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption /*= NULL*/, UINT nStyle /*= MB_OK | MB_ICONEXCLAMATION*/, const PPMSGBOXPARAMS * pMsgBox /*= NULL*/)
{
	if (NULL != pMsgBox)
		return PPMessageBox(NULL, lpszText, lpszCaption, nStyle, pMsgBox);

	PackText();
	return PPMessageBox(NULL, lpszText, lpszCaption, nStyle, &m_MsgParam);
}

int CPPMessageBox::MessageBoxIndirect(const PPMSGBOXPARAMS * pMsgBox /*= NULL*/)
{
	if (NULL != pMsgBox)
		return PPMessageBoxIndirect(pMsgBox);
	
	PackText();
	return PPMessageBoxIndirect(&m_MsgParam);
}

CPPMessageBox::~CPPMessageBox()
{
}

void CPPMessageBox::SetTimeouts(int nAutoclick, int nDisable /*= 0*/, BOOL bGlobalDisable /*= FALSE*/)
{
	m_MsgParam.bDisableAllCtrls = bGlobalDisable;
	m_MsgParam.nDisabledSeconds = nDisable;
	m_MsgParam.nTimeoutSeconds = nAutoclick;
}

void CPPMessageBox::SetCustomButtons(LPCTSTR lpszButtonNames /*= NULL*/)
{
	m_sCustomButtonText = GetString(lpszButtonNames);
}

PPMSGBOXPARAMS * CPPMessageBox::GetMessageBoxParams()
{
	PackText();
	return &m_MsgParam;
} //End of GetMessageBoxParams

void CPPMessageBox::SetMessageBoxParams(const PPMSGBOXPARAMS * pMsgBoxParams)
{
	m_MsgParam = *pMsgBoxParams;
} //End of SetMessageBoxParams

void CPPMessageBox::SetBackground(DWORD dwArea, PPMSGBOXAREA_BK * pAreaBk)
{
	switch (dwArea)
	{
	case PPMSGBOX_HEADER_AREA:
		m_MsgParam.pHeaderBk = *pAreaBk;
		m_sHeaderSepText = GetString(pAreaBk->lpszSepText);
		break;
	case PPMSGBOX_MESSAGE_AREA:
		m_MsgParam.pMsgBoxBk = *pAreaBk;
		break;
	case PPMSGBOX_CONTROL_AREA:
		m_MsgParam.pControlBk = *pAreaBk;
		m_sControlSepText = GetString(pAreaBk->lpszSepText);
		break;
	case PPMSGBOX_MOREINFO_AREA:
		m_MsgParam.pMoreInfoBk = *pAreaBk;
		m_sMoreInfoSepText = GetString(pAreaBk->lpszSepText);
		break;
	} //switch
} //End of SetBackground

void CPPMessageBox::SetBackground(DWORD dwArea, int nEffectBk /*= -1*/, COLORREF crStartBk /*= -1*/, COLORREF crEndBk /*= -1*/, COLORREF crMidBk /*= -1*/)
{
	if (crStartBk < 0) crStartBk = ::GetSysColor(COLOR_3DFACE);
	if (crEndBk < 0) crEndBk = crStartBk;
	if (crMidBk < 0) crMidBk = crStartBk;

	switch (dwArea)
	{
	case PPMSGBOX_HEADER_AREA:
		m_MsgParam.pHeaderBk.nEffectBk = nEffectBk;
		m_MsgParam.pHeaderBk.crStartBk = crStartBk;
		m_MsgParam.pHeaderBk.crEndBk = crEndBk;
		m_MsgParam.pHeaderBk.crMidBk = crMidBk;
		break;
	case PPMSGBOX_MESSAGE_AREA:
		m_MsgParam.pMsgBoxBk.nEffectBk = nEffectBk;
		m_MsgParam.pMsgBoxBk.crStartBk = crStartBk;
		m_MsgParam.pMsgBoxBk.crEndBk = crEndBk;
		m_MsgParam.pMsgBoxBk.crMidBk = crMidBk;
		break;
	case PPMSGBOX_CONTROL_AREA:
		m_MsgParam.pControlBk.crStartBk = crStartBk;
		m_MsgParam.pControlBk.crEndBk = crEndBk;
		m_MsgParam.pControlBk.crMidBk = crMidBk;
		break;
	case PPMSGBOX_MOREINFO_AREA:
		m_MsgParam.pMoreInfoBk.crStartBk = crStartBk;
		m_MsgParam.pMoreInfoBk.crEndBk = crEndBk;
		m_MsgParam.pMoreInfoBk.crMidBk = crMidBk;
		break;
	} //switch
} //End of SetBackground

void CPPMessageBox::SetSeparator(DWORD dwArea, int nSepType /*= PPMSGBOX_SEP_NONE*/, int nSepAlign /*= PPMSGBOX_ALIGN_LEFT*/, LPCTSTR lpszSepText /*= NULL*/)
{
	switch (dwArea)
	{
	case PPMSGBOX_HEADER_AREA:
		m_MsgParam.pHeaderBk.nSepType = nSepType;
		m_MsgParam.pHeaderBk.nSepAlign = nSepAlign;
		m_sHeaderSepText = GetString(lpszSepText);
		break;
	case PPMSGBOX_CONTROL_AREA:
		m_MsgParam.pControlBk.nSepType = nSepType;
		m_MsgParam.pControlBk.nSepAlign = nSepAlign;
		m_sControlSepText = GetString(lpszSepText);
		break;
	case PPMSGBOX_MOREINFO_AREA:
		m_MsgParam.pMoreInfoBk.nSepType = nSepType;
		m_MsgParam.pMoreInfoBk.nSepAlign = nSepAlign;
		m_sMoreInfoSepText = GetString(lpszSepText);
		break;
	} //switch
} //End of SetSeparator

void CPPMessageBox::ClearAllButtonsText()
{
	m_mapButtonText.clear();
} //End of ClearAllButtonsText

void CPPMessageBox::SetButtonText(DWORD dwBtnID, LPCTSTR lpszText /*= NULL*/)
{
	mapBtnText::iterator iter = m_mapButtonText.find(dwBtnID);
	CString sText = GetString(lpszText);
	if (iter == m_mapButtonText.end())
	{
		if (!sText.IsEmpty())
			m_mapButtonText.insert(std::make_pair(dwBtnID, sText));
	}
	else
	{
		if (sText.IsEmpty())
			m_mapButtonText.erase(iter);
		else
			iter->second = sText;
	}
} //End of SetButtonText

LPCTSTR CPPMessageBox::GetButtonText(DWORD dwBtnID)
{
	mapBtnText::iterator iter = m_mapButtonText.find(dwBtnID);
	if (iter != m_mapButtonText.end())
		return (LPCTSTR)iter->second;
	return NULL;
} //End of GetButtonText

CString CPPMessageBox::GetString(LPCTSTR lpszText)
{
	if ((NULL == lpszText) || (0 == lpszText [0]))
		return _T("");

	CString sText = _T("");
	if (0 == HIWORD(lpszText)) 
		sText.LoadString(LOWORD(lpszText));
	else
		sText = (CString)lpszText;
	return sText;
} //End of GetString

void CPPMessageBox::PackText()
{
	//Packing a localization of the button text
	m_pLocalBtnText.clear();
	mapBtnText::iterator iter;
	for (iter = m_mapButtonText.begin(); iter != m_mapButtonText.end(); ++iter)
		m_pLocalBtnText.insert(std::make_pair(iter->first, (LPCTSTR)iter->second));
	m_MsgParam.pLocalBtnText = &m_pLocalBtnText;

	//Packing a separator text
	m_MsgParam.pHeaderBk.lpszSepText = (LPCTSTR)m_sHeaderSepText;
	m_MsgParam.pControlBk.lpszSepText = (LPCTSTR)m_sControlSepText;
	m_MsgParam.pMoreInfoBk.lpszSepText = (LPCTSTR)m_sMoreInfoSepText;

	m_MsgParam.lpszCustomButtons = (LPCTSTR)m_sCustomButtonText;

} //End of PackText