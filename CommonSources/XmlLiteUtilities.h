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
#ifndef _XmlLiteUtilities_h_
#define _XmlLiteUtilities_h_

#include <xmllite.h>



//================================================================================
//================================================================================
//============================== FileStream ======================================
//================================================================================
//================================================================================
class FileStream : public IStream
{
	FileStream(HANDLE hFile) 
	{
		_refcount = 1;
		_hFile = hFile;
	}

	 ~FileStream()
	{
		if (_hFile != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(_hFile);
		}
	}

public:
	HRESULT static OpenFile(LPCWSTR pName, IStream ** ppStream, bool fWrite)
	{
		HANDLE hFile = ::CreateFileW(pName, fWrite ? GENERIC_WRITE : GENERIC_READ, FILE_SHARE_READ,
			NULL, fWrite ? CREATE_ALWAYS : OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile == INVALID_HANDLE_VALUE)
			return HRESULT_FROM_WIN32(GetLastError());

		*ppStream = new FileStream(hFile);

		if(*ppStream == NULL)
			CloseHandle(hFile);

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject)
	{ 
		if (iid == __uuidof(IUnknown)
			|| iid == __uuidof(IStream)
			|| iid == __uuidof(ISequentialStream))
		{
			*ppvObject = static_cast<IStream*>(this);
			AddRef();
			return S_OK;
		} else
			return E_NOINTERFACE; 
	}

	virtual ULONG STDMETHODCALLTYPE AddRef(void) 
	{ 
		return (ULONG)InterlockedIncrement(&_refcount); 
	}

	virtual ULONG STDMETHODCALLTYPE Release(void) 
	{
		ULONG res = (ULONG) InterlockedDecrement(&_refcount);
		if (res == 0) 
			delete this;
		return res;
	}

	// ISequentialStream Interface
public:
	virtual HRESULT STDMETHODCALLTYPE Read(void* pv, ULONG cb, ULONG* pcbRead)
	{
		BOOL rc = ReadFile(_hFile, pv, cb, pcbRead, NULL);
		return (rc) ? S_OK : HRESULT_FROM_WIN32(GetLastError());
	}

	virtual HRESULT STDMETHODCALLTYPE Write(void const* pv, ULONG cb, ULONG* pcbWritten)
	{
		BOOL rc = WriteFile(_hFile, pv, cb, pcbWritten, NULL);
		return rc ? S_OK : HRESULT_FROM_WIN32(GetLastError());
	}

	// IStream Interface
public:
	virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER)				{return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*,
		ULARGE_INTEGER*)													{return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE Commit(DWORD)							{return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE Revert(void)							{return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)		{return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)	{return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE Clone(IStream **)						{return E_NOTIMPL;}

	virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER liDistanceToMove, DWORD dwOrigin,
		ULARGE_INTEGER* lpNewFilePointer)
	{ 
		DWORD dwMoveMethod;

		switch(dwOrigin)
		{
		case STREAM_SEEK_SET:
			dwMoveMethod = FILE_BEGIN;
			break;
		case STREAM_SEEK_CUR:
			dwMoveMethod = FILE_CURRENT;
			break;
		case STREAM_SEEK_END:
			dwMoveMethod = FILE_END;
			break;
		default:   
			return STG_E_INVALIDFUNCTION;
			break;
		}

		if (SetFilePointerEx(_hFile, liDistanceToMove, (PLARGE_INTEGER) lpNewFilePointer,
			dwMoveMethod) == 0)
			return HRESULT_FROM_WIN32(GetLastError());
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG* pStatstg, DWORD grfStatFlag) 
	{
		if (GetFileSizeEx(_hFile, (PLARGE_INTEGER) &pStatstg->cbSize) == 0)
			return HRESULT_FROM_WIN32(GetLastError());
		return S_OK;
	}

private:
	HANDLE _hFile;
	LONG _refcount;
};


//================================================================================
//================================================================================
//============================== ReadOnlyMemStream ===============================
//================================================================================
//================================================================================
class ReadOnlyMemStream : public IStream
{
	ReadOnlyMemStream(const BYTE* pMem, ULONG length, BOOL bAutoDelete) 
	{
		_refcount = 1;
		_pMem = pMem;
		_length = length;
		_bAutoDelete = bAutoDelete;
		_curPos = 0;
	}

	~ReadOnlyMemStream()
	{
		if (_bAutoDelete)
			delete _pMem;
	}

public:

	HRESULT static Create(const BYTE* pMem, ULONG length, BOOL bAutoDelete, IStream** ppStream)
	{
		*ppStream = new ReadOnlyMemStream(pMem, length, bAutoDelete);
		return S_OK;
	}


	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void ** ppvObject)
	{ 
		if (iid == __uuidof(IUnknown)
			|| iid == __uuidof(IStream)
			|| iid == __uuidof(ISequentialStream))
		{
			*ppvObject = static_cast<IStream*>(this);
			AddRef();
			return S_OK;
		} else
			return E_NOINTERFACE; 
	}

	virtual ULONG STDMETHODCALLTYPE AddRef(void) 
	{ 
		return (ULONG)InterlockedIncrement(&_refcount); 
	}

	virtual ULONG STDMETHODCALLTYPE Release(void) 
	{
		ULONG res = (ULONG) InterlockedDecrement(&_refcount);
		if (res == 0) 
			delete this;
		return res;
	}

	// ISequentialStream Interface
public:
	virtual HRESULT STDMETHODCALLTYPE Read(void* pv, ULONG cb, ULONG* pcbRead)
	{
		ASSERT(pv != NULL && pcbRead != NULL);
		ASSERT(_pMem != NULL);
		*pcbRead = ((_curPos + cb) > _length) ? (_length - _curPos) : cb;
		memcpy(pv, &_pMem[_curPos], *pcbRead);
		_curPos += *pcbRead;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Write(void const* pv, ULONG cb, ULONG* pcbWritten)
																			{return E_NOTIMPL;}

	// IStream Interface
public:
	virtual HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER)				{return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*,
		ULARGE_INTEGER*)													{return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE Commit(DWORD)							{return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE Revert(void)							{return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)		{return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD)	{return E_NOTIMPL;}
	virtual HRESULT STDMETHODCALLTYPE Clone(IStream **)						{return E_NOTIMPL;}

	virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER liDistanceToMove, DWORD dwOrigin,
		ULARGE_INTEGER* lpNewFilePointer)
	{ 
		switch(dwOrigin)
		{
		case STREAM_SEEK_SET:
			_curPos = liDistanceToMove.LowPart;
			break;
		case STREAM_SEEK_CUR:
			_curPos += liDistanceToMove.LowPart;
			break;
		case STREAM_SEEK_END:
			return STG_E_INVALIDFUNCTION;
			break;
		default:   
			return STG_E_INVALIDFUNCTION;
			break;
		}
		if (_curPos > _length)
			return STG_E_INVALIDFUNCTION;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Stat(STATSTG* pStatstg, DWORD grfStatFlag) 
	{
		pStatstg->cbSize.LowPart = _length;
		return S_OK;
	}

private:
	const BYTE* _pMem;
	LONG _refcount;
	ULONG _length;
	ULONG _curPos;
	BOOL _bAutoDelete;
};

class XmlLiteReaderHelper
{
public:
#ifdef _UNITTESTING
	static BOOL UnitTest();
#endif
	XmlLiteReaderHelper(IXmlReader& reader):m_reader(reader){}

	BOOL FindElement(LPCTSTR elementName, INT restrictDepth = -1);
	LPCTSTR GetElementText();
	LPCTSTR GetElementAttribute(LPCTSTR attributeName);
	INT GetCurrentDepth();
private:
	IXmlReader& m_reader;

};







#endif
