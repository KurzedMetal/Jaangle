// Dlg.h  Version 1.0
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

#ifndef _CrashReportDlg_h_
#define _CrashReportDlg_h_

#include "XColorStatic.h"
#include "XHyperLink.h"
#include "DetailsDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CDlg dialog

class CCrashReportDlg : public CDialog
{
// Construction
public:
	CCrashReportDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CCrashReportDlg();

// Dialog Data
	//{{AFX_DATA(CDlg)
	enum { IDD = IDD_XCRASHREPORT_DIALOG };
	CEdit			m_What;
	CXHyperLink		m_ClickHere;
	CXColorStatic	m_PleaseTellUs;
	CXColorStatic	m_Banner;
	CXColorStatic	m_Icon;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON		m_hIcon;
	CDWordArray	m_aFileDetails;
	CString		m_strZipFile;
	int			m_nFilesInZip;
	BOOL		m_bOverIcon;
    HCURSOR		m_hLinkCursor;
	HCURSOR		m_hPrevCursor;

	virtual BOOL OnInitDialog();
	afx_msg void OnSend();
	afx_msg void OnDoNotSend();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT OnClickHere(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

private:
	UINT m_appID;
	CString m_debugInfo;
	CString CollectInformation(LPCTSTR exePath);
	void InitializeDisplay(LPCTSTR appName, LPCTSTR appPath);
	void LoadHandCursor();
	void RestartIfNeeded();

};

#endif 