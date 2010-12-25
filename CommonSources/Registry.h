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

#if !defined(_REGISTRY_H_INCLUDED)
#define _REGISTRY_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/* Silence STL warnings */

#pragma warning (disable : 4786)
#pragma warning (disable : 4514)
#pragma warning (push, 3)

#ifdef _UNICODE
#if !defined(UNICODE)
#define UNICODE 
#endif
#endif

#include <windows.h>
#include <math.h>
#include <TCHAR.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <assert.h>


/* ====================================
 * Begin Preprocessor Definitions
 *
 * - Ugly, but well worth it.
 * ==================================== */


#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef	std::string tstring;
#endif


/* CRegistry Open Flags */

#define CREG_CREATE		1
#define CREG_AUTOOPEN	2
#define CREG_NOCACHE	4

/* CRegistry Behaivor flags */

#define CREG_LOADING	8


#define _MAX_REG_VALUE	2048	// Maximum Value length, this may be increased

#define NOT_ES(func)			func != ERROR_SUCCESS
#define IS_ES(func)				func == ERROR_SUCCESS
#define _R_BUF(size)			_TCHAR buffer[size]

#define REGENTRY_AUTO			__cregOwner->GetFlags() & CREG_AUTOOPEN
#define REGENTRY_TRYCLOSE		if (REGENTRY_AUTO) __cregOwner->AutoClose()
#define REGENTRY_SZ_SAFE		iType == REG_SZ || iType == REG_BINARY
#define REGENTRY_ALLOWCONV(b)	__bConvertable = b;


#define REGENTRY_REFRESH_IF_NOCACHE \
	if (__cregOwner->GetFlags() & CREG_NOCACHE && \
		REGENTRY_NOTLOADING && REGENTRY_KEYVALID( KEY_QUERY_VALUE ))\
		__cregOwner->Refresh();

#define REGENTRY_UPDATE_MULTISTRING \
	LPTSTR lpszBuffer = new _TCHAR[_MAX_REG_VALUE];	\
	REGENTRY_SETLOADING(+); GetMulti(lpszBuffer); REGENTRY_SETLOADING(-); \
	SetMulti(lpszBuffer, MultiLength(true), true); \
	delete [] lpszBuffer;

	
#define REGENTRY_KEYVALID(auto_access) \
	lpszName && ((REGENTRY_AUTO && __cregOwner->AutoOpen(auto_access)) || (!(REGENTRY_AUTO) && __cregOwner->hKey != NULL))

#define REGENTRY_NOTLOADING \
	!(__cregOwner->GetFlags() & CREG_LOADING)

#define REGENTRY_SETLOADING(op) \
	__cregOwner->__dwFlags op= CREG_LOADING

#define REGENTRY_BINARYTOSTRING \
	if (iType == REG_BINARY) { ForceStr(); lpszStr = *this; } 

#define REGENTRY_NONCONV_STORAGETYPE(type) \
	CRegEntry& operator=( type &Value ){ REGENTRY_ALLOWCONV(false) SetStruct(Value); return *this; }  \
	operator type(){ type Return; GetStruct(Return); return Return; }

#define REGENTRY_CONV_STORAGETYPE(type, to_sz, from_sz, from_dw, no_result) \
	CRegEntry& operator=( type Value ) { to_sz return (*this = (LPCTSTR)(buffer)); } \
	operator type(){ REGENTRY_BINARYTOSTRING return (REGENTRY_SZ_SAFE ? from_sz :(iType == REG_DWORD ? from_dw : no_result)); }

#define REGENTRY_CONV_NUMERIC_STORAGETYPE(type, maxlen, form, from_sz, from_dw) \
	REGENTRY_CONV_STORAGETYPE(type, _R_BUF(maxlen); _stprintf(buffer, _T(#form), Value);, from_sz, from_dw, 0)


/* ====================================
 * Include CRegEntry Class Definition
 * ==================================== */

#include "RegEntry.h"

/* ====================================
 * Begin CRegistry Class Definition
 * ==================================== */

using namespace std;

class CRegistry {

public:
	
	CRegistry	(DWORD flags = CREG_CREATE);	
	virtual		~CRegistry() { Close(); for (int i=0; i < int(_reEntries.size()); ++i) delete _reEntries[i]; delete [] _lpszSubKey; }

	CRegEntry&	operator[](LPCTSTR lpszVName);
	CRegEntry*	GetAt(size_t n) { assert(n < Count());  return _reEntries.at(n); }
	
	bool		Open(LPCTSTR lpszRegPath, HKEY hRootKey = HKEY_LOCAL_MACHINE,
				DWORD dwAccess = KEY_QUERY_VALUE | KEY_SET_VALUE, bool bAuto = false);
	
	bool		AutoOpen(DWORD dwAccess);
	void		AutoClose();
	void		Close();
	bool		Refresh();	

	static bool	KeyExists(LPCTSTR lpszRegPath, HKEY hRootKey = HKEY_LOCAL_MACHINE);
	bool		SubKeyExists(LPCTSTR lpszSub);	
	
	void		DeleteKey();	

	__inline	DWORD GetFlags()	{	return __dwFlags; }
	__inline	size_t Count()		{	return _reEntries.size(); }
	
	HKEY		hKey;		/* Registry key handle */

protected:
	
	DWORD		__dwFlags;
	friend		void CRegEntry::MultiSetAt(size_t nIndex, LPCTSTR lpszVal);
	friend		void CRegEntry::MultiRemoveAt(size_t nIndex);

private:

	void		InitData();	
	void		DeleteKey(HKEY hPrimaryKey, LPCTSTR lpszSubKey);

	HKEY		_hRootKey;
	LPTSTR		_lpszSubKey;

	std::vector<CRegEntry *> _reEntries;
};

#pragma warning(pop)

#endif
