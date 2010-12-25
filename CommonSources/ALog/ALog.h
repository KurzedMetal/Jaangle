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
#ifndef _ALog_h_
#define _ALog_h_

//Install
//	1.	#define ALOG_ACTIVE (Active Logging)
//	2.	#include "ALog/ALog.h" in stdafx.h
//	3.	[Optionally] Call ALog::Shutdown to stop logging

//Comment
//	1. ALog class is NOT THREADSAFE. This means that you must configure it in a
//		thread safe way.
//	2. Each Viewer must be Thread safe though

//Configure
//	1.	Don't Do anything. It has defaults
//	2.	SetLogViewer: Manually Create and assign a viewer
//		Defaults: 
//			DEBUG MODE: ALogDebugger / Debug Output
//			RELEASE: log.me file usage
//	3.	SetLogDepth: How deep will be the logging (Def: DP_InfoL2)
//	4.	SetLogOptions: Use the flags

//Use
//	Its TRACE compatible. You may use some optional flags.
//	1.	'@0' means LogDepth Error
//			ex: TRACE(_T("@0This is an Error\r\n"));
//	2.	'@1' means LogDepth Warning
//	3.	'@2-@9' means LogDepth L1-L8 (Information)
//	4.	'@E' logs a Formatted GetLastError()
//			ex: TRACE(_T("@0@EFalse API Error\r\n"));
//	5.	'@M' starts a HDmeasure timer
//			ex: TRACE(_T("@1@M...test functions...\r\n"));
//	6.	'@N' stops the timer. - show the results
//			ex: TRACE(_T("@1@N...test functions finished...\r\n"));
//	You can also use the TRACEST macro. Traces the stack
//			ex: TRACEST(_T("CLogDebuggerDlg::OnBnClickedButton1\r\n"));
//	If ALOG_ACTIVE is not defined then nothing of these is compiled

//Enable this to Use Exact Timer Function
#define USE_CPUTICKER
//Enable this to let the option to use PrunnedFileViewer (limited file logger)
#define USE_PRUNNEDFILEVIEWER

#ifdef USE_CPUTICKER
#include "cputicker.h"
#endif


//=================================================
//=========UTILITIES
//=================================================
class CCritSection
{
public:
	CCritSection();
	~CCritSection();

	void Lock();
	void UnLock();
	BOOL IsLocked();

private:
	CRITICAL_SECTION m_CS;
};

class CThreadLock
{
public:
	CThreadLock(CCritSection* cs, bool InitialLock);
	~CThreadLock();
	void Lock();
private:
	CCritSection* m_cs;
};

//=================================================
//=========VIEWERS
//=================================================

class AViewer
{
public:
	virtual ~AViewer()	{}
	virtual BOOL Show(LPCTSTR description)	= 0;
	virtual LPCTSTR GetID()					= 0;
};

class DebugViewer: public AViewer
{
public:
	virtual BOOL Show(LPCTSTR description);
	virtual LPCTSTR GetID()					{ return _T("DEB");}
};
class FileViewer: public AViewer
{
public:
	FileViewer(LPCTSTR path);
	virtual ~FileViewer();
	virtual BOOL Show(LPCTSTR description);
	virtual LPCTSTR GetID()					{ return _T("FIL");}

private:
	HANDLE m_hFile;
};
#ifdef USE_PRUNNEDFILEVIEWER
#include <list>
#include <string>
class PrunnedFileViewer: public AViewer
{
public:
	PrunnedFileViewer(LPCTSTR path, UINT lines = 10);
	virtual ~PrunnedFileViewer();
	virtual BOOL Show(LPCTSTR description);
	virtual LPCTSTR GetID()					{ return _T("PRF");}
private:
	UINT m_maxLines;
	std::list<std::string> m_debugLines;
	//std::basic_string<TCHAR> m_path;
	CCritSection m_cs;
	HANDLE m_hFile;
	static const INT cbfLen = 2000;
	CHAR bf[cbfLen];
};
#endif

#define MAX_VIEWERS 10
class MultiViewer: public AViewer
{
public:
	MultiViewer(BOOL bAutoDelete = TRUE);
	virtual ~MultiViewer();
	virtual BOOL Show(LPCTSTR description);

	BOOL AddViewer(AViewer* viewer);
	AViewer* GetViewer(INT ID);
	INT GetViewerCount()					{return m_viewersCount;}
	virtual LPCTSTR GetID()					{ return _T("MUL");}
private:
	BOOL m_bAutoDelete;
	AViewer* m_viewers[MAX_VIEWERS];
	INT m_viewersCount;
};

class CopyDataViewer: public AViewer
{
public:
	explicit CopyDataViewer(HWND target);
	virtual BOOL Show(LPCTSTR description);
	virtual LPCTSTR GetID()					{ return _T("COD");}

private:
	HWND m_TargetHwnd;
};


class WindowViewer: public AViewer
{
public:
	explicit WindowViewer(HWND target);
	virtual BOOL Show(LPCTSTR description);
	virtual LPCTSTR GetID()					{ return _T("WIN");}
protected:
	void SetTargetHwnd(HWND target);
	inline HWND GetTargetHwnd()	{return m_TargetHwnd;}
private:
	HWND m_TargetHwnd;
};

#define AM_EXITMSG  WM_APP + 0x50

class AutoWindowViewer: public AViewer
{
public:
	explicit AutoWindowViewer(HINSTANCE hInstance);
	virtual ~AutoWindowViewer();

	virtual BOOL Show(LPCTSTR description);
	virtual LPCTSTR GetID()					{ return _T("AUW");}
private:
	CWinThread* m_thread;
	HINSTANCE m_hInst;
private:
	static UINT InitializeST(LPVOID pParam);
	void Initialize();
	WindowViewer* m_pWinViewer;
	HWND m_TrgHwnd;
};

//=================================================
//=========VIEWERS END!!!
//=================================================

#ifdef ALOG_ACTIVE
	#undef TRACE
	#define TRACE ALog::Inst()->Log
	#define TRACEST ALogStackTracer st
	#define TRACEHWND ALog::Inst()->LogHWND
	#define TRACEBENCH ALogBench
#else
	#define TRACEST __noop
	#define TRACEHWND __noop
	$define TRACEBENCH __noop
#endif



class ALog
{
public:
	enum LogOptionEnum
	{
		LO_MaxAllowedDepth,
		LO_DefaultDepth,
		LO_ShowDepth,
		LO_ShowTimeFromStartup,
		LO_ShowThreadID,
		LO_ShowTimeDate,
		LO_AlwaysCheckForLastError,
		LO_Last
	};

	//---Creation - destruction---
	~ALog();
	static ALog* Inst();
	static void Shutdown();
	
	//---Configuration---
	void AddViewer(AViewer* pViewer);
	AViewer* GetViewer(INT idx);
	void SetOption(LogOptionEnum id, INT val);
	INT GetOption(LogOptionEnum id);

	//---Logging---
	BOOL LoadConfiguration(LPCTSTR path);
	
	void Log(LPCTSTR description);
	void Log(LPCTSTR description, LPCTSTR s1);
	void Log(LPCTSTR description, INT i1);
	void Log(LPCTSTR description, LPCTSTR s1, LPCTSTR s2);
	void Log(LPCTSTR description, INT i1, INT i2);
	void Log(LPCTSTR description, LPCTSTR s1, INT i2);
	void Log(LPCTSTR description, INT i1, LPCTSTR s2);

	void LogHWND(LPCTSTR description, HWND hwnd);

	void LogUnsecure(LPCTSTR description, ...);
private:
	ALog();
private:
	static AViewer* GetAutoViewer();
	AViewer* GetViewerByID(INT viewerID, LPCTSTR iniFilePath);
	static ALog* m_pLog;
	INT m_option[LO_Last];
	MultiViewer* m_pViewer;
	__int64 m_markedTime;
	SIZE_T m_markedMemSize;
	CCPUTicker* m_pTicker;
	DWORD m_MainThreadID;
	DWORD m_StartupTicks;
	//MultiViewer* m_viewer;

};

class ALogAutoShurdownHelper
{
public:
	ALogAutoShurdownHelper()
	{
#ifdef ALOG_ACTIVE
//		ALog::Inst();
#endif
	}
	~ALogAutoShurdownHelper()
	{
#ifdef ALOG_ACTIVE
		ALog::Shutdown();
#endif
	}
};


#ifdef USE_CPUTICKER
class ALogBench
{
	static const INT maxLen = 200;
public:
	ALogBench(LPCTSTR funcName, BOOL bAutoStart)
	{
		start = 0;
		bStopped = FALSE;
		_tcsncpy(m_funcName, funcName, maxLen);
		m_funcName[maxLen - 1] = 0;
		if (bAutoStart)
			Start();
	}
	void Start()
	{
		start = m_tick.Measure(); 
	}
	void Stop()
	{
		if (start == 0)
			TRACE(_T("ALogBench::Stop. You need to start it first."));
		else
		{
			__int64 passedTime = m_tick.Measure() - start;
			ALog::Inst()->LogUnsecure(_T("^^^[%6.5f ms] to compete '%s'\r\n"), (passedTime * 1000.0) / (DOUBLE)CCPUTicker::GetCachedCPUFrequency(), m_funcName);
			bStopped = TRUE;
		}
	}
	~ALogBench()
	{
		if (!bStopped)
			Stop();
	}
private:
	CCPUTicker m_tick;
	TCHAR m_funcName[maxLen];
	__int64 start; 
	BOOL bStopped;
};
#else
class ALogBench
{
	static const INT maxLen = 200;
public:
	ALogBench(LPCTSTR funcName, BOOL bAutoStart)
	{
		start = 0;
		bStopped = FALSE;
		_tcsncpy(m_funcName, funcName, maxLen);
		m_funcName[maxLen - 1] = 0;
		if (bAutoStart)
			Start();
	}
	void Start()
	{
		start = GetTickCount(); 
	}
	void Stop()
	{
		if (start == 0)
			TRACE(_T("ALogBench::Stop. You need to start it first."));
		else
		{
			DWORD passedTime = GetTickCount() - start;
			ALog::Inst()->LogUnsecure(_T("^^^[%6.5f ms] to compete '%s'\r\n"), (DOUBLE)passedTime, m_funcName);
			bStopped = TRUE;
		}
	}
	~ALogBench()
	{
		if (!bStopped)
			Stop();
	}
private:
	TCHAR m_funcName[maxLen];
	__int64 start; 
	BOOL bStopped;
};
#endif



class ALogStackTracer
{
	static const INT maxLen = 500;
public:
	ALogStackTracer(LPCTSTR funcName, LPCTSTR param)
	{
		_sntprintf(bf, maxLen, _T("%s p:'%s'"), funcName, param);
		bf[maxLen - 1] = 0;
		TRACE(_T("[IN] %s\r\n"), bf);
		//m_startTick = GetTickCount();
	}
	ALogStackTracer(LPCTSTR funcName, INT param)
	{
		_sntprintf(bf, maxLen, _T("%s p: %d"), funcName, param);
		bf[maxLen - 1] = 0;
		TRACE(_T("[IN] %s\r\n"), bf);
		//m_startTick = GetTickCount();
	}
	ALogStackTracer(LPCTSTR funcName)
	{
		_tcsncpy(bf, funcName, maxLen);
		bf[maxLen - 1] = 0;
		TRACE(_T("[IN] %s\r\n"), bf);
		//m_startTick = GetTickCount();
	}
	~ALogStackTracer()
	{
		TRACE(_T("[OUT] %s\r\n"), bf);
		//TRACE(_T("[OUT %d] %s\r\n"), GetTickCount() - m_startTick, bf);
	}
private:
	TCHAR bf[maxLen];
	//DWORD m_startTick;
};



#endif
