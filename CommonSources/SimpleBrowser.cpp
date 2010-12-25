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
#include "comdef.h"
#include "mshtml.h"
#include "mshtmcid.h"
#include "mshtmhst.h"
#include "exdispid.h"

#include "SimpleBrowser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define Unused(parameter) parameter					// avoid compile warnings
													// about unused parameters

/////////////////////////////////////////////////////////////////////////////
// Construction and creation
/////////////////////////////////////////////////////////////////////////////

SimpleBrowser::SimpleBrowser()
	: m_pBrowser(NULL),
	  m_pBrowserDispatch(NULL),
	  m_pBrowserWnd(NULL),
	  m_pNotifier(NULL)
{
}


SimpleBrowser::~SimpleBrowser()
{
    ReleaseObjects();
}

void SimpleBrowser::ReleaseObjects()
{

	if (m_pBrowser != NULL)
	{
		m_pBrowser->Release();
		m_pBrowser = NULL;
	}
	if (m_pBrowserDispatch != NULL) 
	{
		m_pBrowserDispatch->Release();
		m_pBrowserDispatch = NULL;
	}
	delete m_pBrowserWnd;
	m_pBrowserWnd = NULL;
}

// Standard create

BOOL SimpleBrowser::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    // create browser control window as child of this window; 
	// this window sinks events from control
	//BOOL results = CWnd::Create(AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW),
	//                         NULL,
	//						 dwStyle,
	//						 rect,
	//						 pParentWnd,
	//						 nID);
	BOOL results = CWnd::Create(_T("#32770"),
		NULL,
		dwStyle,
		rect,
		pParentWnd,
		nID);
    if (!results) 
		return FALSE;
	//create the original window
	m_pBrowserWnd = new CWnd;
    results = m_pBrowserWnd->CreateControl(CLSID_WebBrowser, NULL, (WS_VISIBLE | WS_TABSTOP),
		CRect(0,0,rect.right - rect.left,rect.bottom - rect.top), this, AFX_IDW_PANE_FIRST);
    if (!results) 
	{
        DestroyWindow();
        return FALSE;
    }
    // get control interfaces
	m_pBrowser = NULL;
    LPUNKNOWN pUnknown = m_pBrowserWnd->GetControlUnknown();
	HRESULT hr = S_FALSE;
	if (pUnknown != NULL)
	{
		hr = pUnknown->QueryInterface(IID_IWebBrowser2,(void **)&m_pBrowser);
		if (SUCCEEDED(hr)) 
			hr = pUnknown->QueryInterface(IID_IDispatch,(void **)&m_pBrowserDispatch);
	}
    if (!SUCCEEDED(hr)) {
        m_pBrowserWnd->DestroyWindow();
		delete m_pBrowserWnd;
		m_pBrowserWnd = NULL;
        DestroyWindow();
        return FALSE;        
    }
    return TRUE;
}

void SimpleBrowser::PostNcDestroy()
{
	ReleaseObjects();
	CWnd::PostNcDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// Controls
/////////////////////////////////////////////////////////////////////////////

// Navigate to URL

BOOL SimpleBrowser::Navigate(LPCTSTR URL)
{
	ASSERT(m_pBrowser != NULL);
	if (m_pBrowser != NULL) 
	{
		CString		url(URL);
		_variant_t	flags(0L,VT_I4);
		_variant_t	target_frame_name(_T(""));
		_variant_t	post_data(_T(""));
		_variant_t	headers(_T(""));
		return m_pBrowser->Navigate(url.AllocSysString(), &flags, &target_frame_name, &post_data, &headers) == S_OK;
	}
	return FALSE;
}

// Navigate to HTML document in resource

BOOL SimpleBrowser::LoadHTML(LPCTSTR html)
{
	Clear();
	Write(html);
	return TRUE;
}

BOOL SimpleBrowser::LoadFromResource(INT resID)
{
	ASSERT(m_pBrowser != NULL);
	if (m_pBrowser != NULL) 
	{
		CString resource_string;
		// load HTML document from resource
		HRSRC resource_handle = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(resID), RT_HTML);
		if (resource_handle != NULL) 
		{
			HGLOBAL resource = LoadResource(AfxGetResourceHandle(),	resource_handle);
			if (resource != NULL) 
			{
				LPVOID resource_memory = LockResource(resource);
				if (resource_memory != NULL) 
				{
					DWORD resource_size = SizeofResource(AfxGetResourceHandle(), resource_handle);
					// identify the resource document as MBCS (e.g. ANSI) or UNICODE
					bool     UNICODE_document = false;
					wchar_t *UNICODE_memory   = (wchar_t *)resource_memory;
					int      UNICODE_size     = resource_size / sizeof(wchar_t);
					if (UNICODE_size >= 1) 
					{
						// check for UNICODE byte order mark
						if (*UNICODE_memory == L'\xFEFF') 
						{
							UNICODE_document = true;
							UNICODE_memory  += 1;
							UNICODE_size    -= 1;
						}
						// otherwise, check for UNICODE leading tag

						else if (UNICODE_size >= 5) 
						{
							if ((UNICODE_memory[0]           == L'<') &&
							    (towupper(UNICODE_memory[1]) == L'H') &&
							    (towupper(UNICODE_memory[2]) == L'T') &&
							    (towupper(UNICODE_memory[3]) == L'M') &&
							    (towupper(UNICODE_memory[4]) == L'L')) 
							{
								UNICODE_document = true;
							}
						}

						// Note: This logic assumes that the UNICODE resource document is 
						//       in little-endian byte order, which would be typical for 
						//       any HTML document used as a resource in a Windows application.

					}
					// convert resource document if required
#if !defined(UNICODE)
					if (UNICODE_document) 
					{
						char *MBCS_buffer = resource_string.GetBufferSetLength(resource_size + 1);
						int MBCS_length = ::WideCharToMultiByte(CP_ACP,
							0, UNICODE_memory, UNICODE_size, MBCS_buffer, resource_size + 1, NULL, NULL);
						resource_string.ReleaseBuffer(MBCS_length);
					}
					else 
						resource_string = CString((char *)resource_memory,resource_size);
#else
					if (UNICODE_document) 
						resource_string = CString(UNICODE_memory,UNICODE_size);
					else 
					{
						wchar_t *UNICODE_buffer = resource_string.GetBufferSetLength(resource_size + 1);
						int UNICODE_length = ::MultiByteToWideChar(CP_ACP,
							0, (const char *)resource_memory, resource_size, UNICODE_buffer, (resource_size + 1));
						resource_string.ReleaseBuffer(UNICODE_length);
					}
#endif
				}
			}
		}
		return LoadHTML(resource_string);
	}
	return FALSE;
}

// Append string to current document

void SimpleBrowser::Write(LPCTSTR string)
{
	if (m_pBrowser!= NULL) 
	{
		// get document interface
		IHTMLDocument2 *document = GetDocument();
		if (document != NULL) 
		{
			// construct text to be written to browser as SAFEARRAY
			SAFEARRAY *safe_array = SafeArrayCreateVector(VT_VARIANT,0,1);
			VARIANT	*variant;
			SafeArrayAccessData(safe_array,(LPVOID *)&variant);
			variant->vt      = VT_BSTR;
			variant->bstrVal = CString(string).AllocSysString();
			SafeArrayUnaccessData(safe_array);
			// write SAFEARRAY to browser document
			document->write(safe_array);
			document->Release();
			document = NULL;
		}
	}
}

void SimpleBrowser::Clear()
{
	IHTMLDocument2* document = GetDocument();
	if (document != NULL) 
	{
		HRESULT hr = S_OK;
		document->close();
		VARIANT		open_name;
		VARIANT		open_features;
		VARIANT		open_replace;
		IDispatch	*open_window	= NULL;
		::VariantInit(&open_name);
		open_name.vt      = VT_BSTR;
		open_name.bstrVal = ::SysAllocString(L"_self");
		::VariantInit(&open_features);
		::VariantInit(&open_replace);
		hr = document->open(::SysAllocString(L"text/html"), 
			open_name, open_features, open_replace, &open_window);
		if (hr == S_OK) 
			Refresh();
		if (open_window != NULL) 
			open_window->Release();
	}
}

// Navigation operations

void SimpleBrowser::GoBack()
{
	if (m_pBrowser != NULL)
		m_pBrowser->GoBack();
}

void SimpleBrowser::GoForward()
{
	if (m_pBrowser != NULL)
		m_pBrowser->GoForward();
}

void SimpleBrowser::GoHome()
{
	if (m_pBrowser != NULL)
		m_pBrowser->GoHome();
}

void SimpleBrowser::Refresh()
{
	if (m_pBrowser != NULL)
		m_pBrowser->Refresh();
}

void SimpleBrowser::Stop()
{
	if (m_pBrowser != NULL)
		m_pBrowser->Stop();
}

// Print contents

void SimpleBrowser::Print(LPCTSTR header,LPCTSTR footer)
{
	if (m_pBrowser != NULL) {
		// construct two element SAFEARRAY;
		// first element is header string, second element is footer string
		HRESULT hr;
		VARIANT		header_variant;
		VariantInit(&header_variant);
		V_VT(&header_variant)   = VT_BSTR;
		V_BSTR(&header_variant) = CString(header).AllocSysString();
		VARIANT		footer_variant;
		VariantInit(&footer_variant);
		V_VT(&footer_variant)   = VT_BSTR;
		V_BSTR(&footer_variant) = CString(footer).AllocSysString();
		long index;
		SAFEARRAYBOUND	parameter_array_bound[1];
		SAFEARRAY		*parameter_array = NULL;

		parameter_array_bound[0].cElements = 2;
		parameter_array_bound[0].lLbound   = 0;

		parameter_array = SafeArrayCreate(VT_VARIANT,1,parameter_array_bound);

		index = 0;
		hr    = SafeArrayPutElement(parameter_array,&index,&header_variant);

		index = 1;
		hr    = SafeArrayPutElement(parameter_array,&index,&footer_variant);

		VARIANT	parameter;
		VariantInit(&parameter);
		V_VT(&parameter)    = VT_ARRAY | VT_BYREF;
		V_ARRAY(&parameter) = parameter_array;

		// start printing browser contents

		hr = m_pBrowser->ExecWB(OLECMDID_PRINT,OLECMDEXECOPT_DODEFAULT,&parameter,NULL);

			// Note: There is no simple way to determine that printing has completed. 
			//       In fact, if the browser is destroyed while printing is in progress, 
			//       only part of the contents will be printed.

		// release SAFEARRAY

		if (!SUCCEEDED(hr)) {
			VariantClear(&header_variant);
			VariantClear(&footer_variant);
			if (parameter_array != NULL) {
				SafeArrayDestroy(parameter_array);
			}
		}

	}
}

// Miscellaneous
bool SimpleBrowser::GetBusy()
{
    bool busy = false;
	if (m_pBrowser != NULL) 
	{
		VARIANT_BOOL    variant_bool;
		HRESULT hr = m_pBrowser->get_Busy(&variant_bool);
		if (SUCCEEDED(hr)) 
			busy = (variant_bool == VARIANT_TRUE);    

	}

    return busy;
}

CString SimpleBrowser::GetLocationName()
{
    CString location_name = _T("");

	if (m_pBrowser != NULL) 
	{
		BSTR location_name_BSTR = NULL;
		HRESULT hr = m_pBrowser->get_LocationName(&location_name_BSTR);
		if (SUCCEEDED(hr))
			location_name = location_name_BSTR;
		::SysFreeString(location_name_BSTR);
	}
    return location_name;
}

CString SimpleBrowser::GetLocationURL()
{
    CString location_URL = _T("");
	if (m_pBrowser != NULL) 
	{
		BSTR location_URL_BSTR = NULL;
		HRESULT hr = m_pBrowser->get_LocationURL(&location_URL_BSTR);
		if (SUCCEEDED(hr)) 
			location_URL = location_URL_BSTR;
		::SysFreeString(location_URL_BSTR);
	}
    return location_URL;
}

READYSTATE SimpleBrowser::GetReadyState()
{
    READYSTATE readystate = READYSTATE_UNINITIALIZED;
    if (m_pBrowser != NULL)
		m_pBrowser->get_ReadyState(&readystate);
    return readystate;
}

bool SimpleBrowser::GetSilent()
{
    bool silent = false;
	if (m_pBrowser != NULL) 
	{
		VARIANT_BOOL silent_variant;
		HRESULT hr = m_pBrowser->get_Silent(&silent_variant);
		if (SUCCEEDED(hr)) 
			silent = (silent_variant == VARIANT_TRUE);
	}
    return silent;
}

void SimpleBrowser::PutSilent(bool silent)
{
	if (m_pBrowser != NULL) 
	{
		VARIANT_BOOL silent_variant;
		if (silent) silent_variant = VARIANT_TRUE;
		else        silent_variant = VARIANT_FALSE;
		m_pBrowser->put_Silent(silent_variant);
	}
}

IHTMLDocument2* SimpleBrowser::GetDocument()
{
	IHTMLDocument2* document = NULL;
	if (m_pBrowser == NULL)
	{
		TRACE(_T("SimpleBrowser::GetDocument Browser object does not exist\r\n"));
		return NULL;
	}
	//=== Get browser document's dispatch interface. Wait 1000ms
	IDispatch* document_dispatch = NULL;
	HRESULT hr = m_pBrowser->get_Document(&document_dispatch);
	if (document_dispatch == NULL)
	{
		TRACE(_T("SimpleBrowser::GetDocument Trying to load about blank\r\n"));
		Navigate(_T("about:blank"));
		INT tick = GetTickCount();
		while (document_dispatch == NULL)
		{
			if (GetTickCount() - tick > 2000)
			{
				TRACE(_T("SimpleBrowser::GetDocument GetDispatch timeout\r\n"));
				return NULL;
			}
			Sleep(10);
			hr = m_pBrowser->get_Document(&document_dispatch);
		}
	}
	hr = document_dispatch->QueryInterface(IID_IHTMLDocument2, (void **)&document);
	if (hr != S_OK)
		TRACE(_T("SimpleBrowser::GetDocument QueryInterface Failure\r\n"));
	document_dispatch->Release();
	return document;
}

IHTMLDocument3* SimpleBrowser::GetDocument3()
{
	IHTMLDocument3* document = NULL;
	if (m_pBrowser == NULL)
	{
		TRACE(_T("SimpleBrowser::GetDocument Browser object does not exist\r\n"));
		return NULL;
	}
	//=== Get browser document's dispatch interface. Wait 1000ms
	IDispatch* document_dispatch = NULL;
	HRESULT hr = m_pBrowser->get_Document(&document_dispatch);
	if (document_dispatch == NULL)
	{
		TRACE(_T("SimpleBrowser::GetDocument Trying to load about blank\r\n"));
		Navigate(_T("about:blank"));
		INT tick = GetTickCount();
		while (document_dispatch == NULL)
		{
			if (GetTickCount() - tick > 2000)
			{
				TRACE(_T("SimpleBrowser::GetDocument GetDispatch timeout\r\n"));
				return NULL;
			}
			Sleep(10);
			hr = m_pBrowser->get_Document(&document_dispatch);
		}
	}
	hr = document_dispatch->QueryInterface(IID_IHTMLDocument3, (void **)&document);
	if (hr != S_OK)
		TRACE(_T("SimpleBrowser::GetDocument QueryInterface Failure\r\n"));
	document_dispatch->Release();
	return document;
}


/////////////////////////////////////////////////////////////////////////////
// Message handlers
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(SimpleBrowser,CWnd)
    //{{AFX_MSG_MAP(SimpleBrowser)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// Resize control window as this window is resized

void SimpleBrowser::OnSize(UINT nType, int cx, int cy)
{

	if (m_pBrowser != NULL)
	{
        CRect rect(0,0,cx,cy);	
		m_pBrowserWnd->MoveWindow(&rect, TRUE);
		//m_pBrowserWnd->SetWindowPos(&wndTop, 0,0,0,0,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOREPOSITION|SWP_NOSIZE);
	}
	CWnd::OnSize(nType, cx, cy);
}

/////////////////////////////////////////////////////////////////////////////
// Event handlers
/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENTSINK_MAP(SimpleBrowser,CWnd)
    ON_EVENT(SimpleBrowser,AFX_IDW_PANE_FIRST,
	         DISPID_BEFORENAVIGATE2,
             OnBeforeNavigate2, 
			 VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
    ON_EVENT(SimpleBrowser,AFX_IDW_PANE_FIRST,
	         DISPID_DOCUMENTCOMPLETE,
             OnDocumentComplete, 
			 VTS_DISPATCH VTS_PVARIANT)
    ON_EVENT(SimpleBrowser,AFX_IDW_PANE_FIRST,
	         DISPID_DOWNLOADBEGIN,
             OnDownloadBegin, 
			 VTS_NONE)
    ON_EVENT(SimpleBrowser,AFX_IDW_PANE_FIRST,
	         DISPID_PROGRESSCHANGE,
             OnProgressChange, 
			 VTS_I4 VTS_I4)
    ON_EVENT(SimpleBrowser,AFX_IDW_PANE_FIRST,
	         DISPID_DOWNLOADCOMPLETE,
             OnDownloadComplete, 
			 VTS_NONE)
    ON_EVENT(SimpleBrowser,AFX_IDW_PANE_FIRST,
	         DISPID_NAVIGATECOMPLETE2,
             OnNavigateComplete2, 
			 VTS_DISPATCH VTS_PVARIANT)
    ON_EVENT(SimpleBrowser,AFX_IDW_PANE_FIRST,
	         DISPID_STATUSTEXTCHANGE,
             OnStatusTextChange, 
			 VTS_BSTR)
    ON_EVENT(SimpleBrowser,AFX_IDW_PANE_FIRST,
	         DISPID_TITLECHANGE,
             OnTitleChange, 
			 VTS_BSTR)
	ON_EVENT(SimpleBrowser,AFX_IDW_PANE_FIRST,
			 DISPID_COMMANDSTATECHANGE,
			 OnCommandStateChange, 
			 VTS_I4 VTS_BOOL)


			 
END_EVENTSINK_MAP()

BOOL SimpleBrowser::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void SimpleBrowser::OnBeforeNavigate2(LPDISPATCH lpDisp, VARIANT FAR *URL,
					   VARIANT FAR *Flags, VARIANT FAR *TargetFrameName,
					   VARIANT FAR *PostData, VARIANT FAR *Headers,
					   VARIANT_BOOL *Cancel)
{
	if (m_pNotifier)
	{
		m_pNotifier->OnBeforeNavigate(CW2CT(URL->bstrVal));
	}

}
void SimpleBrowser::OnDocumentComplete(LPDISPATCH lpDisp,VARIANT FAR* URL)
{
	if (m_pNotifier)
		m_pNotifier->OnDocumentComplete(CW2CT(URL->bstrVal));
}
void SimpleBrowser::OnDownloadBegin()
{
	if (m_pNotifier)
		m_pNotifier->OnDownloadBegin();
}
void SimpleBrowser::OnProgressChange(long progress,long progress_max)
{
	if (m_pNotifier)
		m_pNotifier->OnProgressChange(progress, progress_max);
}
void SimpleBrowser::OnDownloadComplete()
{
	if (m_pNotifier)
		m_pNotifier->OnDownloadComplete();
}
void SimpleBrowser::OnNavigateComplete2(LPDISPATCH lpDisp,VARIANT FAR* URL)
{
	if (m_pNotifier)
		m_pNotifier->OnNavigateComplete(CW2CT(URL->bstrVal));

}
void SimpleBrowser::OnStatusTextChange(BSTR text)
{
	if (m_pNotifier)
		m_pNotifier->OnStatusTextChange(CW2CT(text));
}
void SimpleBrowser::OnTitleChange(BSTR text)
{
	if (m_pNotifier)
		m_pNotifier->OnStatusTextChange(CW2CT(text));
}
void SimpleBrowser::OnCommandStateChange(long command, VARIANT_BOOL Enable)
{
	if (m_pNotifier)
	{
		switch(command)
		{
		case CSC_NAVIGATEFORWARD:
			m_pNotifier->EnableNextButton(Enable ? TRUE : FALSE);
			break;
		case CSC_NAVIGATEBACK:
			m_pNotifier->EnableBackButton(Enable ? TRUE : FALSE);
			break;
		default:
			break;
		}
	}
}

BOOL SimpleBrowser::ExecuteJavascript(LPCTSTR script)
{
	IHTMLDocument2* pDocument = GetDocument();
	if (pDocument == NULL)
		return FALSE;
	BOOL bRet = FALSE;
	IHTMLWindow2* pHTMLWindow = NULL;
	if (pDocument->get_parentWindow(&pHTMLWindow) == S_OK)// execCommand(_T("testFunction()"), TRUE, value, &ret))
	{
		VARIANT vEmpty = {0};
		CComBSTR bstrFunction = script;
		CComBSTR bstrLanguage = L"javascript";
		HRESULT hr = pHTMLWindow->execScript(bstrFunction, bstrLanguage, &vEmpty);
		bRet = (hr == S_OK);
		pHTMLWindow->Release();
	}
	pDocument->Release();
	return bRet;
}
BOOL SimpleBrowser::GetElementText(LPCTSTR elementName, LPTSTR bf, UINT bfLen)
{
	IHTMLDocument3* pDocument = GetDocument3();
	if (pDocument == NULL)
		return FALSE;
	BOOL bRet = FALSE;
	IHTMLElement* pElement = NULL;
	CComBSTR elemName = elementName;
	if (pDocument->getElementById(elemName, &pElement) == S_OK && pElement != NULL)
	{
		CComBSTR innerText;
		if (pElement->get_innerText(&innerText) == S_OK)
		{
			if (innerText.Length() > 0)
			{
				_tcsncpy(bf, innerText, bfLen);
				bf[bfLen - 1] = 0;
			}
			else
				bf[0] = 0;
			bRet = TRUE;
		}
		pElement->Release();
	}
	pDocument->Release();
	return bRet;

}


