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

//
// Change History:
//
// April 6, 2003		- Original release, and article posted at
//						  http://www.codeproject.com/useritems/SimpleBrowserForMFC.asp
//
// April 12, 2003		- Replaced NavigateString() with Write() and Clear().
//						- Added logic to Create() to wait for document ready.
//						- Added GetDocument() method.
//						- Added notification support.
//						- Added post data and headers to BeforeNavigate2 handling.
//

#if !defined(SimpleBrowser_defined)
#define SimpleBrowser_defined

#include "mshtml.h"



class SimpleBrowserNotifier
{
public:
	virtual void OnBeforeNavigate(LPCTSTR url)								= 0;
	virtual void OnDocumentComplete(LPCTSTR url)							= 0;
	virtual void OnDownloadBegin()											= 0;
	virtual void OnProgressChange(long progress,long progress_max)			= 0;
	virtual void OnDownloadComplete()										= 0;
	virtual void OnNavigateComplete(LPCTSTR url)							= 0;
	virtual void OnStatusTextChange(LPCTSTR text)							= 0;
	virtual void OnTitleChange(LPCTSTR text)								= 0;
	virtual void EnableBackButton(BOOL bEnable)								= 0;
	virtual void EnableNextButton(BOOL bEnable)								= 0;
};

class SimpleBrowser : public CWnd 
{
public:
	// construction and creation
    SimpleBrowser();
    virtual ~SimpleBrowser();

	// create browser directly
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	// controls
	BOOL Navigate(LPCTSTR URL);
	BOOL LoadHTML(LPCTSTR html);
	BOOL LoadFromResource(INT resID);

	// append string to current document; note that the WebBrowser control tolerates
	void Write(LPCTSTR string);				
	// clear current document
	void Clear();							
	
	void GoBack();
	void GoForward();
	void GoHome();
	void Refresh();
	void Stop();

	// start printing contents; uses same 'metacharacters' for header and
	// footer as Internet Explorer; see IE Page Setup dialog
    void Print(LPCTSTR header = _T("&w&b&b&p"), LPCTSTR footer = _T("&d &t"));
			   
    bool GetBusy();
	READYSTATE GetReadyState();

    CString GetLocationName();
    CString GetLocationURL();

	// get/set silent property (if true, dialog and message boxes may not be shown)
    bool GetSilent();
    void PutSilent(bool silent = false);

	void SetNotifier(SimpleBrowserNotifier* pNotifier)		{m_pNotifier = pNotifier;}

	BOOL ExecuteJavascript(LPCTSTR script);
	BOOL GetElementText(LPCTSTR elementName, LPTSTR bf, UINT bfLen);
 
// get document interface; returns NULL 
// if interface is not available 
// (which is the case if you've navigated to
//  something that's NOT an HTML document,
//  like an Excel spreadsheet, which the
//  WebBrowser control is perfectly willing
//  to host)
	IWebBrowser2* GetBrowser()				{return m_pBrowser;}
	IHTMLDocument2* GetDocument();			
protected:
	virtual void PostNcDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
	//-----------------Events
	// called before navigation begins; URL is destination, frame
	// is frame name ("" if none), post_data is HTTP POST data (NULL if none),
	// and headers are HTTP headers sent to server;
	// return true to cancel navigation, false to continue
	// navigation to document complete; URL is location
private:
	void OnBeforeNavigate2(LPDISPATCH lpDisp,
		VARIANT FAR *URL,
		VARIANT FAR *Flags,
		VARIANT FAR *TargetFrameName,
		VARIANT FAR *PostData,
		VARIANT FAR *Headers,
		VARIANT_BOOL *Cancel);
	void ReleaseObjects();
	void OnDocumentComplete(LPDISPATCH lpDisp,VARIANT FAR* URL);
	void OnDownloadBegin();
	void OnProgressChange(long progress,long progress_max);
	void OnDownloadComplete();
	void OnNavigateComplete2(LPDISPATCH lpDisp,VARIANT FAR* URL);
	void OnStatusTextChange(BSTR text);
	void OnTitleChange(BSTR bstrText);
	void OnCommandStateChange(long command, VARIANT_BOOL Enable);
public:
	IHTMLDocument3* GetDocument3();
private:
    CWnd* m_pBrowserWnd;			// browser window
	IWebBrowser2* m_pBrowser;		// browser control
    IDispatch* m_pBrowserDispatch;	// browser control dispatch interface
	SimpleBrowserNotifier* m_pNotifier;
};

#endif
