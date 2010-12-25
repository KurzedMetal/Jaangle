//
//	Class:		CPPMessageBox
//
//	Compiler:	Visual C++
//	Tested on:	Visual C++ 6.0
//				Visual C++ .NET 2003
//
//	Version:	1.0
//
//	Created:	01/February/2005
//	Updated:	09/February/2005
//
//	Author:		Eugene Pustovoyt	pustovoyt@mail.ru
//
//	Disclaimer
//	----------
//	THIS SOFTWARE AND THE ACCOMPANYING FILES ARE DISTRIBUTED "AS IS" AND WITHOUT
//	ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED. NO REPONSIBILITIES FOR POSSIBLE
//	DAMAGES OR EVEN FUNCTIONALITY CAN BE TAKEN. THE USER MUST ASSUME THE ENTIRE
//	RISK OF USING THIS SOFTWARE.
//
//	Terms of use
//	------------
//	THIS SOFTWARE IS FREE FOR PERSONAL USE OR FREEWARE APPLICATIONS.
//	IF YOU WISH TO THANK MY WORK, YOU MAY DONATE ANY SUM OF MONEY TO ME 
//  FOR SUPPORT OF DEVELOPMENT OF THIS CLASS.
//	IF YOU USE THIS SOFTWARE IN COMMERCIAL OR SHAREWARE APPLICATIONS YOU
//	ARE GENTLY ASKED TO DONATE ANY SUM OF MONEY TO THE AUTHOR:
//
//
//--- History ------------------------------ 
//

#ifndef _PPMESSAGEBOXCLASS_H_
#define _PPMESSAGEBOXCLASS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PPMessageBoxClass.h : header file
//

#include "PPMessageBox.h"


/////////////////////////////////////////////////////////////////////////////
// CPPMessageBox dialog

class CPPMessageBox 
{
// Construction
public:
	CPPMessageBox();   // standard constructor
	~CPPMessageBox();

	int MessageBox(LPCTSTR lpszText, LPCTSTR lpszCaption = NULL, UINT nStyle = MB_OK | MB_ICONEXCLAMATION, const PPMSGBOXPARAMS * pMsgBox = NULL);
	int MessageBoxIndirect(const PPMSGBOXPARAMS * pMsgBox = NULL);

	PPMSGBOXPARAMS * GetMessageBoxParams();
	void SetMessageBoxParams(const PPMSGBOXPARAMS * pMsgBoxParams);

	void SetTimeouts(int nAutoclick, int nDisable = 0, BOOL bGlobalDisable = FALSE);
	void SetCustomButtons(LPCTSTR lpszButtonNames = NULL);

	//Backgrounds
	void SetBackground(DWORD dwArea, PPMSGBOXAREA_BK * pAreaBk);
	void SetBackground(DWORD dwArea, int nEffectBk = -1, COLORREF crStartBk = -1, COLORREF crEndBk = -1, COLORREF crMidBk = -1);
	void SetSeparator(DWORD dwArea, int nSepType = PPMSGBOX_SEP_NONE, int nSepAlign = PPMSGBOX_ALIGN_LEFT, LPCTSTR lpszSepText = NULL);

	void SetNotify(DWORD dwReportMsgID) {m_MsgParam.dwReportMsgID = dwReportMsgID;};

	//Localization
	void ClearAllButtonsText();
	void SetButtonText(DWORD dwBtnID, LPCTSTR lpszText = NULL);
	LPCTSTR GetButtonText(DWORD dwBtnID);

protected:
	typedef std::map<DWORD, CString> mapBtnText;
	mapBtnText m_mapButtonText;
	mapLocalBtnText m_pLocalBtnText;

	PPMSGBOXPARAMS m_MsgParam;

	CString m_sHeaderSepText;
	CString m_sControlSepText;
	CString m_sMoreInfoSepText;
	CString m_sCustomButtonText;

	CString GetString(LPCTSTR lpszText);
	void PackText();
};

#endif //_PPMESSAGEBOXCLASS_H_
