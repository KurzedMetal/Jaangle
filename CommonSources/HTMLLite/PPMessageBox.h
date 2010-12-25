//
//	Class:		CPPMessageBox
//
//	Compiler:	Visual C++
//	Tested on:	Visual C++ 6.0
//				Visual C++ .NET 2003
//
//	Version:	1.0
//
//	Created:	25/November/2004
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
//	Thanks
//	------------
//		- for some ideas from XMessageBox class by Hans Dietrich 
//		(http://www.codeproject.com/dialog/xmessagebox.asp)
//
//	History
//	------------

#ifndef _PPMESSAGEBOX_H_
#define _PPMESSAGEBOX_H_

// MessageBox() Flags
/*
#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L
*/
#ifndef MB_CANCELTRYCONTINUE
#define MB_CANCELTRYCONTINUE        0x00000006L	// adds three buttons, "Cancel", "Try Again", "Continue"
#endif
#define MB_CONTINUEABORT			0x00000007L // adds two buttons, "Continue", "Abort"
#define MB_SKIPSKIPALLCANCEL		0x00000008L // adds three buttons, "Skip", "Skip All", "Cancel"
#define MB_IGNOREIGNOREALLCANCEL	0x00000009L // adds three buttons, "Ignore", "Ignore All", "Cancel"

#define MB_CHECKBOX					0x01000000L // add checkbox
#define MB_CHECKBOXCHECKED			0x02000000L // checkbox setted by default
#define MB_CHECKBOXUNDERBUTTONS		0x04000000L // place checkbox under the buttons

#define MB_YESTOALL					0x08000000L // must be used with either MB_YESNO or MB_YESNOCANCEL
#define MB_NOTOALL					0x10000000L // must be used with either MB_YESNO or MB_YESNOCANCEL

#define MB_NORESOURCE				0x20000000L	// do not try to load button strings from resources
#define MB_NOSOUND					0x40000000L // do not play sound when mb is displayed
#define MB_TIMEOUT			        0x80000000L	// returned if timeout expired

#define MB_DEFBUTTON5				0x00000400L
#define MB_DEFBUTTON6				0x00000500L


//#define MB_DONOTASKMASK				0x03000000L // mask of "DoNotAsk" checkbox

// Dialog Box Command IDs
/*
#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7
#if(WINVER >= 0x0400)
#define IDCLOSE             8
#define IDHELP              9
#endif
*/
#ifndef IDTRYAGAIN
#define IDTRYAGAIN          10
#endif
#ifndef IDCONTINUE
#define IDCONTINUE          11
#endif
#define IDSKIP				14
#define IDSKIPALL			15
#define IDIGNOREALL			16
#define IDYESTOALL			19
#define IDNOTOALL			20
#define IDREPORT			21
// following 4 ids MUST be sequential
#define IDCUSTOM1			23
#define IDCUSTOM2			24
#define IDCUSTOM3			25
#define IDCUSTOM4			26

#define IDMOREINFO_OPENED	30
#define IDMOREINFO_CLOSED	31


//Text align of the separator
#define PPMSGBOX_ALIGN_LEFT		0
#define PPMSGBOX_ALIGN_RIGHT	1
#define PPMSGBOX_ALIGN_CENTER	2

//The type of the separator line
#define PPMSGBOX_SEP_NONE		0
#define PPMSGBOX_SEP_ETCHED		1
#define PPMSGBOX_SEP_BLACK		2
#define PPMSGBOX_SEP_WHITE		3

//The index of the messagebox areas
#define PPMSGBOX_HEADER_AREA	0
#define PPMSGBOX_MESSAGE_AREA	1
#define PPMSGBOX_CONTROL_AREA	2
#define PPMSGBOX_MOREINFO_AREA	3


//Resource string ID 
#define IDS_OK				9001
#define IDS_CANCEL			9002
#define IDS_IGNORE			9003
#define IDS_RETRY			9004
#define IDS_ABORT			9005
#define IDS_HELP			9006
#define IDS_YES				9007
#define IDS_NO				9008
#define IDS_CONTINUE		9009
#define IDS_MOREINFO_CLOSED	9010
#define IDS_MOREINFO_OPENED	9011
#define IDS_DONOTSHOWAGAIN	9012
#define IDS_YESTOALL		9013
#define IDS_NOTOALL			9014
#define IDS_TRYAGAIN		9015
#define IDS_REPORT			9016
#define IDS_IGNOREALL		9017
#define IDS_SKIP			9018
#define IDS_SKIPALL			9019
#define IDS_GLOBALDISABLE	9020

// disable warning C4786: symbol greater than 255 character, okay to ignore
#pragma warning(disable : 4786)
#include <map>
typedef std::map<DWORD, LPCTSTR> mapLocalBtnText;

#pragma pack(1)
typedef struct PPMSGBOXAREA_BK
{
	PPMSGBOXAREA_BK ()
	{
		nSepType = PPMSGBOX_SEP_NONE;
		nSepAlign = PPMSGBOX_ALIGN_LEFT;
		lpszSepText = NULL;
		nEffectBk = -1;
		crStartBk = crMidBk = crEndBk = ::GetSysColor(COLOR_3DFACE);
	}
	
	int nSepType;			// Type of the separator
	int nSepAlign;			// Separator's text align if a text available
	LPCTSTR lpszSepText;	// Separator's text
	int nEffectBk;			// Style to fill a background
	COLORREF crStartBk;		// COLORREF value for filling a background
	COLORREF crMidBk;		// COLORREF value for filling a background
	COLORREF crEndBk;		// COLORREF value for filling a background
} PPMSGBOXAREA_BK;
#pragma pack()


#pragma pack(1)
typedef struct PPMSGBOXPARAMS
{
	PPMSGBOXPARAMS ()
	{
		//For all messagebox
		hParentWnd = NULL;
		hInstanceStrings = NULL;
		hInstanceIcons = NULL;
		lpszCaption = NULL;
		lpszModuleName = NULL;
		lpszCompanyName = NULL;
		nLine = 0;
		dwReportMsgID = 0;
		
		//For header area
		nHeaderHeight = 0;
		lpszHeaderText = NULL;

		//For message area
		lpszText = NULL;

		//For control area
		nControlsAlign = PPMSGBOX_ALIGN_CENTER;
		dwStyle = MB_OK | MB_ICONEXCLAMATION;
		lpszCustomButtons = NULL;
		lpszCheckBoxText = NULL;

		//For moreinfo area
		lpszMoreInfo = NULL;
		PPMSGBOXAREA_BK pMoreInfoBk;

		nTimeoutSeconds = nDisabledSeconds = 0;
		bDisableAllCtrls = FALSE;

		dwContextHelpID = 0;
		lpszIcon = NULL;

		pLocalBtnText = NULL;
	}
	
	//MessageBox window
	HWND hParentWnd;			//
	HINSTANCE hInstanceStrings;
	HINSTANCE hInstanceIcons;
	LPCTSTR lpszCaption;		// Caption of the message box
	LPCTSTR lpszModuleName;		// Module name (for saving DoNotAsk state)
	LPCTSTR lpszCompanyName;	// Company name (for saving DoNotAsk state)
	int nLine;					// Line number (for saving DoNotAsk state)
	DWORD dwReportMsgID;		// ID of the Report button message
	PPMSGBOXAREA_BK pMsgBoxBk;

	//Header area
	int nHeaderHeight;			// Minimal height of the header area (0 = a header disabled)
	LPCTSTR lpszHeaderText;		// Header text
	PPMSGBOXAREA_BK pHeaderBk;

	//Message area
	LPCTSTR lpszText;			// Text of the message box

	//Control area
	int nControlsAlign;			//
	DWORD dwStyle;				//
	PPMSGBOXAREA_BK pControlBk;

	//MoreInfo area
	LPCTSTR lpszMoreInfo;		// Text for more information
	PPMSGBOXAREA_BK pMoreInfoBk;

	//
	int nTimeoutSeconds;		// Timeout to auto-press a default button (in seconds)
	int nDisabledSeconds;		// Time to enable a default button (in seconds)
	BOOL bDisableAllCtrls;		// TRUE to disable all controls

	DWORD dwContextHelpID;		// Identifies a help context
	LPCTSTR lpszIcon;			// Identifies an icon resource

	LPCTSTR lpszCustomButtons;	// List of the custom buttons name separated with '\n' char
	LPCTSTR lpszCheckBoxText;	// Checkbox text

	mapLocalBtnText * pLocalBtnText;

} PPMSGBOXPARAMS;
#pragma pack()

//----------------------------------------------------
int PPMessageBox(HWND hwnd, 
				LPCTSTR lpszMessage,
				LPCTSTR lpszCaption = NULL, 
				DWORD dwStyle = MB_OK | MB_ICONEXCLAMATION,
				const PPMSGBOXPARAMS * pMsgBox = NULL);

//----------------------------------------------------
int PPMessageBoxIndirect(const PPMSGBOXPARAMS * pMsgBox);

#endif //_PPMESSAGEBOX_H_
