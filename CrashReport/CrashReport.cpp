// XCrashReport.cpp  Version 1.0
//
// Author:  Hans Dietrich
//          hdietrich2@hotmail.com
//
// This software is released into the public domain.
// You are free to use it in any way you like, except
// that you may not sell this source code.
//
// This software is provided "as is" with no expressed
// or implied warranty.  I accept no liability for any
// damage or loss of business that this software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CrashReport.h"
#include "CrashReportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CApp

BEGIN_MESSAGE_MAP(CCrashReportApp, CWinApp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApp construction

CCrashReportApp::CCrashReportApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CApp object

CCrashReportApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CApp initialization

BOOL CCrashReportApp::InitInstance()
{
	CWinApp::InitInstance();
	if (_tcscmp(__targv[0], _T("start")) != 0)
		return FALSE;
	CCrashReportDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}
