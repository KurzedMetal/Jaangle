// Details.cpp  Version 1.0
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
#include "DetailsDlg.h"
//#include "strcvt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CDetailsDlg, CDialog)
END_MESSAGE_MAP()

CDetailsDlg::CDetailsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDetailsDlg::IDD, pParent)
{
}

CDetailsDlg::~CDetailsDlg()
{
	if (m_FileContentsFont.GetSafeHandle())
		m_FileContentsFont.DeleteObject();
}

void CDetailsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CDetailsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	LOGFONT lf;
	GetFont()->GetLogFont(&lf);
	_tcscpy_s(lf.lfFaceName, _T("Courier New"));
	m_FileContentsFont.CreateFontIndirect(&lf);
	GetDlgItem(IDC_CONTENTS)->SetFont(&m_FileContentsFont);
	SetDlgItemText(IDC_CONTENTS, m_FileContents);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

