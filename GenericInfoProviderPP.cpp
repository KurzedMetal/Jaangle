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
#include "GenericInfoProviderPP.h"
#include "PrgAPI.h"

IMPLEMENT_DYNAMIC(CGenericInfoProviderPP, CTSPropertyPage)

CGenericInfoProviderPP::CGenericInfoProviderPP(IInfoProvider& ip):
CTSPropertyPage(IDD, NULL),
m_bSomethingChanged(FALSE),
m_IP(ip)
{
}
CGenericInfoProviderPP::~CGenericInfoProviderPP()
{
}


BEGIN_MESSAGE_MAP(CGenericInfoProviderPP, CPropertyPage)
END_MESSAGE_MAP()


BOOL CGenericInfoProviderPP::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	LoadOptions();
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM) PRGAPI()->GetFont(FONT_Dialogs)->m_hObject);
	return TRUE;  // return TRUE unless you set the focus to a control
}
void CGenericInfoProviderPP::OnChange()
{
	if (IsWindowVisible())
	{
		m_bNeedsUpdating = TRUE;
		SetModified(TRUE);
	}
}

void CGenericInfoProviderPP::OnOK()
{
	SaveOptions();
	CPropertyPage::OnOK();
}

INT cLineHeight = 22;
INT cStaticLineHeight = 16;
INT cMargin = 20;
INT cInsideMargin = 2;
INT cControlIDOffset = 1000;
void CGenericInfoProviderPP::LoadOptions()
{
	CRect rc;
	GetClientRect(&rc);
	INT lineWidth = rc.Width() - 2 * cMargin;
	//PrgAPI* pAPI = PRGAPI();
	INT curY = cMargin;
	INT idx = 0;
	IConfigurable::SettingInfo setting;
	while (m_IP.GetSettingInfo(idx, setting))
	{
		switch (setting.type)
		{
		case IConfigurable::COVT_Bool:
			{
				HWND hwnd = CreateWindowEx(0, 
					_T("BUTTON"), 
					0, 
					WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
					cMargin,
					curY,
					lineWidth,
					cLineHeight,
					m_hWnd,
					(HMENU) (cControlIDOffset + idx),
					0,0);
				::SendMessage(hwnd, BM_SETCHECK, m_IP.GetIntSetting(idx) ? BST_CHECKED : BST_UNCHECKED, 0);
			}
			break;
		case IConfigurable::COVT_Int:
			{
				CreateWindowEx(NULL, _T("STATIC"), setting.name, WS_CHILD | WS_VISIBLE | SS_ENDELLIPSIS, cMargin, curY, lineWidth, cLineHeight, m_hWnd, NULL, NULL, NULL);
				curY += cStaticLineHeight;
				if (setting.availableValues != NULL)
				{
					HWND hwnd = CreateWindowEx(0, 
						_T("COMBOBOX"), 
						0, 
						WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
						cMargin,
						curY,
						lineWidth,
						cLineHeight,
						m_hWnd,
						(HMENU) (cControlIDOffset + idx),
						0,0);
					INT cmbIdx = 0;

					while (setting.availableValues[cmbIdx])
					{
						::SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)(LPCTSTR)setting.availableValues[cmbIdx]);
						cmbIdx++;
					}
					::SendMessage(hwnd, CB_SETCURSEL, (WPARAM)m_IP.GetIntSetting(idx), 0);
				}
				else
				{
					HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, 
						_T("EDIT"), 
						0, 
						WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_AUTOHSCROLL,
						cMargin,
						curY,
						lineWidth,
						cLineHeight,
						m_hWnd,
						(HMENU) (cControlIDOffset + idx),
						0,0);
					TCHAR bf[20];
					::SetWindowText(hwnd, _itot(m_IP.GetIntSetting(idx), bf, 10));
				}
			}
			break;
		case IConfigurable::COVT_LPCTSTR:
			{
				CreateWindowEx(NULL, _T("STATIC"), setting.name, WS_CHILD | WS_VISIBLE | SS_ENDELLIPSIS, cMargin, curY, lineWidth, cLineHeight, m_hWnd, NULL, NULL, NULL);
				curY += cStaticLineHeight;
				HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, 
					_T("EDIT"), 
					0, 
					WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
					cMargin,
					curY,
					lineWidth,
					cLineHeight,
					m_hWnd,
					(HMENU) (cControlIDOffset + idx),
					0,0);
				::SetWindowText(hwnd, m_IP.GetLPCTSTRSetting(idx));
			}
			break;
		}
		curY += cInsideMargin + cLineHeight;
		idx++;
	}
	
}

void CGenericInfoProviderPP::SaveOptions()
{
	CRect rc;
	GetClientRect(&rc);
	INT lineWidth = rc.Width() - 2 * cMargin;
	//PrgAPI* pAPI = PRGAPI();
	INT curY = cMargin;
	INT idx = 0;
	IConfigurable::SettingInfo setting;
	while (m_IP.GetSettingInfo(idx, setting))
	{
		switch (setting.type)
		{
		case IConfigurable::COVT_Bool:
			m_IP.SetIntSetting(idx, IsDlgButtonChecked(cControlIDOffset + idx) ? 1 : 0);
			break;
		case IConfigurable::COVT_Int:
			if (setting.availableValues != NULL)
				//m_IP.SetIntSetting(idx, ((CComboBox*)GetDlgItem(cControlIDOffset + idx))->GetCurSel());
				m_IP.SetIntSetting(idx, GetDlgItem(cControlIDOffset + idx)->SendMessage(CB_GETCURSEL));
			else
			{
				TCHAR bf[20];
				GetDlgItemText(cControlIDOffset + idx, bf, 20);
				m_IP.SetIntSetting(idx, _ttoi(bf));
			}
			break;
		case IConfigurable::COVT_LPCTSTR:
			{
				TCHAR bf[1000];
				GetDlgItemText(cControlIDOffset + idx, bf, 1000);
				bf[999] = 0;
				m_IP.SetLPCTSTRSetting(idx, bf);
			}
			break;
		}
		curY += cInsideMargin + cLineHeight;
		idx++;
	}

}

LPCTSTR CGenericInfoProviderPP::GetTitle()
{
	CTSPropertyPage* par = (CTSPropertyPage*)GetPPParent();
	if (par == NULL)
		return m_IP.GetModuleInfo(IInfoProvider::IPI_Name);
	m_title.Format(_T("%s::%s"), par->GetTitle(), m_IP.GetModuleInfo(IInfoProvider::IPI_Name));
	return m_title;
}
