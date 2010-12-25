// Dlg.cpp  Version 1.0
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
#include "MiniVersion.h"
#include "WebPageUtilities.h"
#include "..\ExceptionHandler.h"
#include "stlStringUtils.h"
#include "URLEncDecHelper.h"
#include "WinVersionHelper.h"

#ifdef USE_APPLICATIONMONITOR
//#include "ApplicationMonitor.h"
#include "AppMonitor.h"
#include "AppSettings/IniAppSettings.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef IDC_HAND
#define IDC_HAND MAKEINTRESOURCE(32649)   // From WINUSER.H
#endif

#define MAX_USER_COMMENTS_SIZE	(64 * 1024)

/////////////////////////////////////////////////////////////////////////////
// CCrashReportDlg dialog

BEGIN_MESSAGE_MAP(CCrashReportDlg, CDialog)
	//{{AFX_MSG_MAP(CCrashReportDlg)
	ON_BN_CLICKED(IDC_SEND_ERROR, OnSend)
	ON_BN_CLICKED(IDC_DO_NOT_SEND_ERROR, OnDoNotSend)
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP

	// this message is sent by XHyperLink
	ON_REGISTERED_MESSAGE(WM_XHYPERLINK_CLICKED, OnClickHere)

END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CCrashReportDlg::CCrashReportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCrashReportDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strZipFile  = _T("");
	m_nFilesInZip = 0;
	m_bOverIcon   = FALSE;
	m_hLinkCursor = NULL;
	m_hPrevCursor = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CCrashReportDlg::~CCrashReportDlg()
{
	if (m_hLinkCursor)
		DestroyCursor(m_hLinkCursor);
	m_hLinkCursor = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// DoDataExchange
void CCrashReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCrashReportDlg)
	DDX_Control(pDX, IDC_WHAT, m_What);
	DDX_Control(pDX, IDC_CLICK_HERE, m_ClickHere);
	DDX_Control(pDX, IDC_PLEASE_TELL_US, m_PleaseTellUs);
	DDX_Control(pDX, IDC_BANNER, m_Banner);
	DDX_Control(pDX, IDC_APP_ICON, m_Icon);
	//}}AFX_DATA_MAP
}

BOOL ReadFileContents(LPCTSTR fileName, CStringA& fileContents)
{
	//if (_taccess(fileName, 0) != 0)
	//	::MessageBox(NULL, fileName, _T("Does not exist"), MB_OK);
	//if (_taccess(fileName, 4) != 0)
	//	::MessageBox(NULL, fileName, _T("Not Read only"), MB_OK);

	CFile f;
	for (INT i = 0; i < 20; i++)
	{
		if (f.Open(fileName, CFile::modeRead, 0))
		{
			const UINT bfSize = 8001;
			CHAR bf[bfSize];
			while (TRUE)
			{
				UINT readBytes = f.Read(bf, bfSize - 1);
				if (readBytes == 0)
					break;
				bf[readBytes] = 0;
				fileContents += bf;
			}
			return TRUE;
		}
		Sleep(50);

	}
	

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// OnInitDialog
BOOL CCrashReportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// get our path name - we assume that crashed exe is in same directory
	TCHAR appPath[_MAX_PATH];
	appPath[0]=0;
	::GetModuleFileName(NULL, appPath, _MAX_PATH);
	TCHAR *cp = _tcsrchr(appPath, _T('\\'));
	if (cp != NULL)
	{
		cp++;
		*cp = 0;
	}
	TCHAR crashedAppicationPath[MAX_PATH];
	_sntprintf(crashedAppicationPath, MAX_PATH, _T("%s%s"), appPath, CRASHREPORT_APPEXENAME);
	//=== Initialize the gui with the Crashed Title / icon
	InitializeDisplay(CRASHREPORT_APPTITLE, crashedAppicationPath);
	//=== Create the native report
	m_debugInfo += CollectInformation(crashedAppicationPath);


	//=== Application Monitor Data
#ifdef USE_APPLICATIONMONITOR
	TCHAR bf[MAX_PATH];
	GetModuleFileName(NULL, bf, MAX_PATH);
	LPTSTR pos = _tcsrchr(bf, '\\');
	pos[1] = 0;
	_tcscat(bf, CRASHREPORT_APPEXENAME);
	pos = _tcsrchr(bf, '.');
	pos[1] = 0;
	_tcscat(bf, _T("ini"));

	IniAppSettings settings;
	settings.Load(bf);
	std::basic_string<TCHAR> stateString;
	if (settings.Read(CRASHREPORT_INIMONITORSECTION, CRASHREPORT_INIMONITORVALUE, stateString, _T("")))
	{
		AppMonitor appMon;
		if (appMon.Load(stateString.c_str()))
		{
			const AppMonitor::State& state = appMon.GetState();
			m_appID = state.AppUniqueID;
			CString line;
			line.Format(_T("AppUniqueID: %u\r\n"), state.AppUniqueID);
			m_debugInfo += line;
			line.Format(_T("IsAfterCrashStatus: %d\r\n"), appMon.IsAfterCrash());
			m_debugInfo += line;
			line.Format(_T("Statistics: %d/%d - %d/%d\r\n"), 
				state.Crashes, 
				state.Runs, 
				state.TotalCrashes, 
				state.TotalRuns
				);
			m_debugInfo += line;
		}

	}

#endif


	//=== Add the rest of the files to the report
	INT fileNum = 0;
	LPCTSTR files[] = CRASHREPORT_FILES;
	while (files[fileNum] != 0)
	{
		TCHAR bf[MAX_PATH];
		_sntprintf(bf, MAX_PATH, _T("%s%s"), appPath, files[fileNum]);
		CStringA fileContents;
		if (ReadFileContents(bf, fileContents))
		{
			m_debugInfo += _T("\r\n@===");
			m_debugInfo += files[fileNum];
			m_debugInfo += _T("\r\n");
			m_debugInfo += CA2CT(fileContents);
		}
		fileNum++;
	}
	LoadHandCursor();
	SetTimer(1, 80, NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

	///////////////////////////////////////////////////////////////////////////////
	// DumpSystemInformation
	// Record information about the user's system, such as processor type, amount
	// of memory, etc.
CString CCrashReportDlg::CollectInformation(LPCTSTR exePath)
{
	CString sysInfo;
	CString line;
	const UINT bfLen = 1000;
	TCHAR bf[bfLen];

	//=== Write Exe Path
	line.Format(_T("AppPath: %s\r\n"), exePath);
	sysInfo += line;

	//=== Time
	SYSTEMTIME st;
	GetSystemTime(&st);
	line.Format(_T("Error occurred at: %0.2d/%0.2d/%d %0.2d:%0.2d:%0.2d\r\n"), st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
	sysInfo += line;

	//=== User
	DWORD size = bfLen;
	if (GetUserName(bf, &size))
	{
		line.Format(_T("User: %s\r\n"), bf);
		sysInfo += line;
	}

	//=== Operating system
	WindowsVersion verInfo;
	if (GetWindowsVersion(verInfo))
	{
		TCHAR bf[500];
		FormatWindowsVersion(verInfo, bf, 500);
		line.Format(_T("OS: %s\r\n"), bf);
		sysInfo += line;
	}

	//=== SystemInfo
	SYSTEM_INFO	SystemInfo;
	GetSystemInfo(&SystemInfo);
	line.Format(_T("System: %d processor(s), type %d.\r\n"), SystemInfo.dwNumberOfProcessors, SystemInfo.dwProcessorType);
	sysInfo += line;

	//=== Memory
	MEMORYSTATUS MemInfo;
	MemInfo.dwLength = sizeof(MemInfo);
	GlobalMemoryStatus(&MemInfo);
	line.Format(_T("%d%% memory in use.\r\n"), MemInfo.dwMemoryLoad);
	sysInfo += line;
	line.Format(_T("%d MBytes physical memory.\r\n"), MemInfo.dwTotalPhys / (1024 * 1024));
	sysInfo += line;
	line.Format(_T("%d MBytes user address space free.\r\n"), MemInfo.dwAvailVirtual / (1024*1024));
	sysInfo += line;

	// get version resource
	CMiniVersion ver(exePath);

	// try to get description
	if (ver.GetFileDescription(bf, bfLen))
	{
		line.Format(_T("FileDescription: %s\r\n"), bf);
		sysInfo += line;
	}
	if (ver.GetProductName(bf, bfLen))
	{
		line.Format(_T("ProductName: %s\r\n"), bf);
		sysInfo += line;
	}
	WORD fv[4];
	if (ver.GetFileVersion(fv))
	{
		line.Format(_T("FileVersion: %d.%d.%d.%d\r\n"), fv[0], fv[1], fv[2], fv[3]);
		sysInfo += line;
	}

	ver.Release();

	return sysInfo;

}

///////////////////////////////////////////////////////////////////////////////
// InitializeDisplay
void CCrashReportDlg::InitializeDisplay(LPCTSTR appName, LPCTSTR appPath)
{
	SetWindowText(_T("Crash reporting tool"));
	m_Banner.SetTextColor(RGB(0,0,0));
	m_Banner.SetBackgroundColor(RGB(255,255,255));
	m_Banner.SetBold(TRUE);

	CString strBanner;
	strBanner.Format(_T("%s has encountered a problem and needs to close.  ")
					 _T("We are sorry for the inconvenience."),
					 appName);

	m_Banner.SetWindowText(strBanner);
	m_Banner.SetMargins(22, 8);

	// get icon of crashed app
	HICON hIcon = ::ExtractIcon(AfxGetInstanceHandle(), appPath, 0);

	m_Icon.SetBackgroundColor(RGB(255,255,255), FALSE);
	if (hIcon)
	{
		m_Icon.SetIcon(hIcon, FALSE);
		SetIcon(hIcon, TRUE);
		SetIcon(hIcon, FALSE);
	}

	m_PleaseTellUs.SetBold(TRUE);

	m_What.SetLimitText(128*1024);	// set edit box to 128 KB

    m_ClickHere.SetColours(m_ClickHere.GetLinkColour(),
						   RGB(0,0,255), 
						   m_ClickHere.GetHoverColour());
	m_ClickHere.EnableURL(FALSE);
	m_ClickHere.EnableTooltip(FALSE);
	m_ClickHere.SetNotifyParent(TRUE);
}



///////////////////////////////////////////////////////////////////////////////
// OnClickHere
LRESULT CCrashReportDlg::OnClickHere(WPARAM, LPARAM)
{
	CDetailsDlg dlg(this);
	dlg.m_FileContents = m_debugInfo;
	dlg.DoModal();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// OnSend
void CCrashReportDlg::OnSend() 
{
	CString userComments;
	m_What.GetWindowText(userComments);
	if (userComments.IsEmpty() == FALSE)
	{
		m_debugInfo += _T("\r\n@===User Comments\r\n");
		m_debugInfo += userComments;
	}

	CHAR bf[500];

	const INT cMaxString = 30000;
	CHAR utf8bf[cMaxString];
	INT len = WideCharToMultiByte(CP_UTF8, 0, m_debugInfo, -1, utf8bf, cMaxString, 0, 0);
	if (len == 0)
	{
		TRACE(_T("CCrashReportDlg::OnSend WideCharToMultiByte Failed\r\n"));
		return;
	}
	utf8bf[len] = 0;
	std::string xmlstring = utf8bf;
	replace(xmlstring, "&", "&amp;");
	replace(xmlstring, "<", "&lt;");
	replace(xmlstring, ">", "&gt;");
	replace(xmlstring, "'", "&apos;");
	replace(xmlstring, "\"", "&quot;");

	std::string request = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	request += "<request>";
	request += "<info uid=\"";
	_snprintf_s(bf, 500, "CR1:%u:%u:%u", m_appID, GetVersion(), CRASHREPORT_BUILDVERSION);
	request += bf;
	request += "\" />";

	request += "<operation name=\"senddebuglog\"><log>";
	request += xmlstring;
	request += "</log></operation>";
	request += "</request>";


	//static LPCTSTR const sWebServiceURL = _T("http://localhost/services/tsservice.php");



	INT dataSize = request.size();
	LPSTR encData = URLEncDecHelper::EncodeData((LPBYTE)request.c_str(), dataSize, TRUE, TRUE);

	std::string params;
	params += CRASHREPORT_REPORTURLPARAM;
	params += encData;
	delete[] encData;


	HINTERNET hNet = InternetOpen(CRASHREPORT_APPTITLE, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	std::string page;
	if (DownloadWebPagePost(page, hNet, CRASHREPORT_REPORTURL, params.c_str()))
	{
		LPCSTR pos = page.c_str();
		if (strncmp(pos, "OK", 2) == 0)
		{
			if (page.size() > 2)
			{
				MessageBox(CA2CT(&page[2]), _T("Thank you"));
			}
			else
				MessageBox(_T("Your report has been sent."), _T("Thank you"));
		}

	}
	InternetCloseHandle(hNet);
	RestartIfNeeded();
	CDialog::OnOK();
}

void CCrashReportDlg::RestartIfNeeded()
{
	if (IsDlgButtonChecked(IDC_CHK_RESTART) == BST_CHECKED)
	{
		TCHAR appPath[_MAX_PATH];
		appPath[0]=0;
		::GetModuleFileName(NULL, appPath, _MAX_PATH);
		TCHAR *cp = _tcsrchr(appPath, _T('\\'));
		if (cp != NULL)
		{
			cp++;
			*cp = 0;
		}
		TCHAR crashedAppicationPath[MAX_PATH];
		_sntprintf(crashedAppicationPath, MAX_PATH, _T("%s%s"), appPath, CRASHREPORT_APPEXENAME);
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW;

		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));

		CreateProcess(
			crashedAppicationPath,				// name of executable module
			NULL,				// command line string
			NULL,					// process attributes
			NULL,					// thread attributes
			FALSE,					// handle inheritance option
			0,						// creation flags
			NULL,					// new environment block
			NULL,					// current directory name
			&si,					// startup information
			&pi);					// process information

	}
}

///////////////////////////////////////////////////////////////////////////////
// OnDoNotSend
void CCrashReportDlg::OnDoNotSend() 
{
	RestartIfNeeded();
	CDialog::OnCancel();
}

///////////////////////////////////////////////////////////////////////////////
// OnCancel
void CCrashReportDlg::OnCancel() 
{
	// do not let Cancel close the dialog	
	//CDialog::OnCancel();
}

///////////////////////////////////////////////////////////////////////////////
// OnOK
void CCrashReportDlg::OnOK() 
{
	// do not let Enter close the dialog	
	//CDialog::OnOK();
}


///////////////////////////////////////////////////////////////////////////////
// OnTimer
void CCrashReportDlg::OnTimer(UINT /*nIDEvent*/) 
{
	CPoint point(GetMessagePos());
	ScreenToClient(&point);

	CRect rect;
	m_Icon.GetWindowRect(&rect);
	ScreenToClient(&rect);

	// check if cursor is over icon
	if (!m_bOverIcon && rect.PtInRect(point))
	{
		m_bOverIcon = TRUE;
		if (m_hLinkCursor)
			m_hPrevCursor = ::SetCursor(m_hLinkCursor);
	}
	else if (m_bOverIcon && !rect.PtInRect(point))
	{
		m_bOverIcon = FALSE;
		if (m_hPrevCursor)
			::SetCursor(m_hPrevCursor);
	}
}

///////////////////////////////////////////////////////////////////////////////
// OnSetCursor
BOOL CCrashReportDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_bOverIcon && m_hLinkCursor)
	{
		::SetCursor(m_hLinkCursor);
		return TRUE;
	}
	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

///////////////////////////////////////////////////////////////////////////////
// LoadHandCursor
void CCrashReportDlg::LoadHandCursor()
{
	if (m_hLinkCursor == NULL)				// No cursor handle - try to load one
	{
		// First try to load the Win98 / Windows 2000 hand cursor

		TRACE(_T("loading from IDC_HAND\n"));
		m_hLinkCursor = AfxGetApp()->LoadStandardCursor(IDC_HAND);

		if (m_hLinkCursor == NULL)			// Still no cursor handle - 
											// load the WinHelp hand cursor
		{
			// The following appeared in Paul DiLascia's Jan 1998 MSJ articles.
			// It loads a "hand" cursor from the winhlp32.exe module.

			TRACE(_T("loading from winhlp32\n"));

			// Get the windows directory
			CString strWndDir;
			GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
			strWndDir.ReleaseBuffer();

			strWndDir += _T("\\winhlp32.exe");

			// This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
			HMODULE hModule = LoadLibrary(strWndDir);
			if (hModule) 
			{
				HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
				if (hHandCursor)
					m_hLinkCursor = CopyCursor(hHandCursor);
				FreeLibrary(hModule);
			}
		}
	}
}
