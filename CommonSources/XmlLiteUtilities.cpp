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
#include "XmlLiteUtilities.h"


#ifdef _UNITTESTING

HRESULT WriteAttributes(IXmlReader* pReader)
{
	const WCHAR* pwszPrefix;
	const WCHAR* pwszLocalName;
	const WCHAR* pwszValue;
	HRESULT hr = pReader->MoveToFirstAttribute();

	if (S_FALSE == hr)
		return hr;
	if (S_OK != hr)
	{
		TRACE(L"Error moving to first attribute, error is %08.8lx", hr);
		return -1;
	}
	else
	{
		while (TRUE)
		{
			if (!pReader->IsDefault())
			{
				UINT cwchPrefix;
				if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
				{
					TRACE(L"Error getting prefix, error is %08.8lx", hr);
					return -1;
				}
				if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
				{
					TRACE(L"Error getting local name, error is %08.8lx", hr);
					return -1;
				}
				if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
				{
					TRACE(L"Error getting value, error is %08.8lx", hr);
					return -1;
				}
				if (cwchPrefix > 0)
					TRACE(L"Prefix: %s\r\n", pwszPrefix);
				TRACE(L"Attr: %s=\"%s\" \n", pwszLocalName, pwszValue);
			}

			if (S_OK != pReader->MoveToNextAttribute())
				break;
		}
	}
	return hr;
}


BOOL ParseReader(IXmlReader* pReader)
{
	HRESULT hr;
	XmlNodeType nodeType;
	const WCHAR* pwszPrefix;
	const WCHAR* pwszLocalName;
	const WCHAR* pwszValue;
	UINT cwchPrefix;



	//read until there are no more nodes
	while (S_OK == (hr = pReader->Read(&nodeType)))
	{
		switch (nodeType)
		{
		case XmlNodeType_XmlDeclaration:
			TRACE(L"XmlDeclaration\n");
			if (FAILED(hr = WriteAttributes(pReader)))
			{
				TRACE(L"Error writing attributes, error is %08.8lx", hr);
				return FALSE;
			}
			break;
		case XmlNodeType_Element:
			if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
			{
				TRACE(L"Error getting prefix, error is %08.8lx", hr);
				return FALSE;
			}
			if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
			{
				TRACE(L"Error getting local name, error is %08.8lx", hr);
				return FALSE;
			}
			if (cwchPrefix > 0)
				TRACE(L"Element: %s:%s\n", pwszPrefix, pwszLocalName);
			else
				TRACE(L"Element: %s\n", pwszLocalName);

			if (FAILED(hr = WriteAttributes(pReader)))
			{
				TRACE(L"Error writing attributes, error is %08.8lx", hr);
				return FALSE;
			}

			if (pReader->IsEmptyElement() )
				TRACE(L" (empty)");
			break;
		case XmlNodeType_EndElement:
			if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
			{
				TRACE(L"Error getting prefix, error is %08.8lx", hr);
				return FALSE;
			}
			if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
			{
				TRACE(L"Error getting local name, error is %08.8lx", hr);
				return FALSE;
			}
			if (cwchPrefix > 0)
				TRACE(L"End Element: %s:%s\n", pwszPrefix, pwszLocalName);
			else
				TRACE(L"End Element: %s\n", pwszLocalName);
			break;
		case XmlNodeType_Text:
		case XmlNodeType_Whitespace:
			if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
			{
				TRACE(L"Error getting value, error is %08.8lx", hr);
				return FALSE;
			}
			TRACE(L"Text: \"%s\"\n", pwszValue);
			break;
		case XmlNodeType_CDATA:
			if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
			{
				TRACE(L"Error getting value, error is %08.8lx", hr);
				return FALSE;
			}
			TRACE(L"CDATA: %s\n", pwszValue);
			break;
		case XmlNodeType_ProcessingInstruction:
			if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
			{
				TRACE(L"Error getting name, error is %08.8lx", hr);
				return FALSE;
			}
			if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
			{
				TRACE(L"Error getting value, error is %08.8lx", hr);
				return FALSE;
			}
			TRACE(L"Processing Instruction name:%S value:%S\n", pwszLocalName, pwszValue);
			break;
		case XmlNodeType_Comment:
			if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
			{
				TRACE(L"Error getting value, error is %08.8lx", hr);
				return FALSE;
			}
			TRACE(L"Comment: %s\n", pwszValue);
			break;
		case XmlNodeType_DocumentType:
			TRACE(L"DOCTYPE is not printed\n");
			break;
		}
	}
	return TRUE;

}


BOOL XmlLiteReaderHelper::UnitTest()
{
	//===Test a file stream
	CComPtr<IStream> pFileStream;
	CComPtr<IXmlReader> pReader;
	UNITTEST(FileStream::OpenFile(_T("c:\\test.xml"), &pFileStream, FALSE) == S_OK);
	UNITTEST(CreateXmlReader(__uuidof(IXmlReader), (void**) &pReader, NULL) == S_OK);
	UNITTEST(pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit) == S_OK);
	UNITTEST(pReader->SetInput(pFileStream) == S_OK);
	ParseReader(pReader);


	LARGE_INTEGER i;
	i.LowPart = 0;
	i. HighPart = 0;
	UNITTEST(pFileStream->Seek(i, STREAM_SEEK_SET, NULL) == S_OK);
	STATSTG stats;
	UNITTEST(pFileStream->Stat(&stats, 0) == S_OK);
	BYTE* f = new BYTE[stats.cbSize.LowPart];
	ULONG readBytes = 0;
	UNITTEST(pFileStream->Read(f, stats.cbSize.LowPart, &readBytes) == S_OK);
	CComPtr<IStream> pReadOnlyMemStream;
	UNITTEST(ReadOnlyMemStream::Create(f, 
		stats.cbSize.LowPart, TRUE, &pReadOnlyMemStream) == S_OK);

	UNITTEST(pReader->SetInput(pReadOnlyMemStream) == S_OK);
	//ParseReader(pReader);
	XmlLiteReaderHelper hlp(*pReader);
	INT curDepth = hlp.GetCurrentDepth();
	BOOL bContinueParsing = hlp.FindElement(_T("ItemSearchResponse"), curDepth);
	if (bContinueParsing)
		bContinueParsing = hlp.FindElement(_T("Items"), curDepth + 1);
	if (bContinueParsing)
		bContinueParsing = hlp.FindElement(_T("Request"), curDepth + 2);
	if (bContinueParsing)
		bContinueParsing = hlp.FindElement(_T("IsValid"), curDepth + 3);
	if (bContinueParsing)
	{
		bContinueParsing = FALSE;
		LPCTSTR ret = hlp.GetElementText();
		if (ret != NULL)
			bContinueParsing = (_tcsicmp(_T("True"), ret) == 0);
	}
	if (bContinueParsing)
	{
		while (hlp.FindElement(_T("Item"), curDepth + 2))
		{
			if (hlp.FindElement(_T("Content"), curDepth + 3))
			{
				LPCTSTR ret = hlp.GetElementText();
				if (ret != NULL)
					TRACE(_T("LargeImage URL is: '%s'\r\n"), ret);
			}
		}
	}

	return TRUE;
}

#endif

INT XmlLiteReaderHelper::GetCurrentDepth()
{
	UINT curDepth = 0;
	if (m_reader.GetDepth(&curDepth) == S_OK)
		return curDepth;
	return -1;
}


BOOL XmlLiteReaderHelper::FindElement(LPCTSTR elementName, INT restrictDepth)
{
	XmlNodeType nodeType;
	const WCHAR* pwszLocalName;
	while (m_reader.Read(&nodeType) == S_OK)
	{
		if (nodeType == XmlNodeType_Element)
		{
			if (m_reader.GetLocalName(&pwszLocalName, NULL) == S_OK)
			{
				if (_tcscmp(pwszLocalName, elementName) == 0)
					return TRUE;
			}
		}
		else if (restrictDepth != -1 && nodeType == XmlNodeType_EndElement)
		{
			UINT curDepth = 0;
			m_reader.GetDepth(&curDepth);
			if ((INT)curDepth <= restrictDepth)
				break;
		}
	}
	return FALSE;
}

LPCTSTR XmlLiteReaderHelper::GetElementText()
{
	XmlNodeType nodeType;
	while (m_reader.Read(&nodeType) == S_OK)
	{
		switch (nodeType)
		{
		case XmlNodeType_Text:
		case XmlNodeType_Whitespace:
			LPCTSTR ret;
			if (m_reader.GetValue(&ret, NULL) == S_OK)
				return ret;
			break;
		case XmlNodeType_EndElement:
			return NULL;
		//case XmlNodeType_Element:
		//	ASSERT(0);
		//	return NULL;
		}
	}
	return NULL;
}

LPCTSTR XmlLiteReaderHelper::GetElementAttribute(LPCTSTR attributeName)
{
	if (m_reader.MoveToAttributeByName(attributeName, NULL) == S_OK)
	{
		LPCTSTR ret = NULL;
		if (m_reader.GetValue(&ret, NULL) == S_OK)
			return ret;
	}
	return NULL;
}


