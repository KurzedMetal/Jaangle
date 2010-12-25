// ExceptionHandler.h  Version 1.1
//
// Copyright © 1998 Bruce Dawson
//
// Author:       Bruce Dawson
//               brucedawson@cygnus-software.com
//
// Modified by:  Hans Dietrich
//               hdietrich2@hotmail.com
//
// A paper by the original author can be found at:
//     http://www.cygnus-software.com/papers/release_debugging.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef	EXCEPTIONHANDLER_H
#define	EXCEPTIONHANDLER_H

#include "VersionNo.h"

//=== Comment in/out if you want to use application monitor class (Application should use it also
#define USE_APPLICATIONMONITOR

//=== Set the application exe file name. Used:
//		(CrashReporter): Creation of the debug report.
#define CRASHREPORT_APPEXENAME	_T("jaangle.exe")


//=== Set the application name. Used:
//		(CrashReporter): Creation of the debug report.
//		(CrashReporter): DefaultInit of the ApplicationMonitor Object
#define CRASHREPORT_APPTITLE	_T("Jaangle")

//=== Set the Crash Report Name
//		(MainApp): Execute this when finished handling to start crash report
//		* The CrashReport Project must be linked to this file
#define CRASHREPORT_EXENAME		_T("CrashReport.exe")


//=== Set the service url that the data should be send. 
//		The first file must be the default log for the crash report.
//		The array is null termanting
//		(CrashReporter): 
#define CRASHREPORT_FILES		{_T("crashlog.txt"), _T("debug.log"), NULL}


//=== Set the service url that the data should be send
//		(CrashReporter): 
#define CRASHREPORT_REPORTURL		_T("http://services.artificialspirit.com/tsservice.php")
#define CRASHREPORT_REPORTURLPARAM		"data="


#define CRASHREPORT_INIMONITORSECTION _T("General")
#define CRASHREPORT_INIMONITORVALUE _T("AppMonitoring")


//=== Set the xml template of the string that the data should be formated
//		(CrashReporter): 
#define CRASHREPORT_BUILDVERSION		BUILD_VERSION




typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;

int __cdecl RecordExceptionInfo(PEXCEPTION_POINTERS data, const CHAR *Message);



/*
// Sample usage - put the code that used to be in main into HandledMain.
// To hook it in to an MFC app add ExceptionAttacher.cpp from the mfctest
// application into your project.
int main(int argc, char *argv[])
{
	int Result = -1;
	__try
	{
		Result = HandledMain(argc, argv);
	}
	__except(RecordExceptionInfo(GetExceptionInformation(), "main thread"))
	{
		// Do nothing here - RecordExceptionInfo() has already done
		// everything that is needed. Actually this code won't even
		// get called unless you return EXCEPTION_EXECUTE_HANDLER from
		// the __except clause.
	}
	return Result;
}
*/

#endif
