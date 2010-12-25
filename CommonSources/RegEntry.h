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

#if !defined(_REGENTRY_H_INCLUDED)
#define _REGENTRY_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRegistry;

class CRegEntry
{
public:

	CRegEntry(CRegistry* Owner = NULL);		
	virtual ~CRegEntry() { if (lpszName) delete [] lpszName; if (lpszStr) delete [] lpszStr; };

	/* -----------------------------------------*
	 *	Operators								*
	 * -----------------------------------------*/
	
	CRegEntry&	operator =( CRegEntry& cregValue );
	CRegEntry&	operator =( LPCTSTR lpszValue );
	CRegEntry&	operator =( LPDWORD lpdwValue );	
	CRegEntry&	operator =( DWORD dwValue ) { return (*this = &dwValue); }	
				operator LPTSTR();
				operator DWORD();

	
	// Data types without implemented conversions
	// NOTE: I realize these will only check asserts
	// when a value is set and retrieved during the
	// same session. But it is better than no check.

	REGENTRY_NONCONV_STORAGETYPE(POINT);
	REGENTRY_NONCONV_STORAGETYPE(RECT);
	REGENTRY_NONCONV_STORAGETYPE(SYSTEMTIME);


	// Numeric types with conversions
	// If you'd like to add more, follow this form:
	// data type, max string length + 1, format specification, from string, from DWORD

	REGENTRY_CONV_NUMERIC_STORAGETYPE(__int64, 28, %I64d, _ttoi64(lpszStr), (__int64)dwDWORD)
	REGENTRY_CONV_NUMERIC_STORAGETYPE(double, 18, %f, _tcstod(lpszStr, NULL), (double)dwDWORD)	
	REGENTRY_CONV_NUMERIC_STORAGETYPE(bool, 2, %d, (_ttoi(lpszStr) != 0), (dwDWORD != 0))
	REGENTRY_CONV_NUMERIC_STORAGETYPE(int, 12, %d, _ttoi(lpszStr), (int)dwDWORD)
	REGENTRY_CONV_NUMERIC_STORAGETYPE(UINT, 12, %d, (UINT)_tcstoul(lpszStr, NULL, NULL), (UINT)dwDWORD)

	// Types with conversions: type to/from string, type from unsigned long

	REGENTRY_CONV_STORAGETYPE(tstring, _R_BUF(_MAX_REG_VALUE); _tcscpy(buffer, Value.c_str());,
	lpszStr, _ultot(dwDWORD, lpszStr, NULL), _T(""))
		

	/* -----------------------------------------*
	 *	Member Variables and Functions			*
	 * -----------------------------------------*/
	
	LPTSTR		lpszName;	// The value name
	UINT		iType;		// Value data type
	
	void		InitData(CRegistry* Owner = NULL);	
	void		ForceStr();
	bool		Delete();	


	/* The following six functions handle REG_MULTI_SZ support: */

	void		SetMulti(LPCTSTR lpszValue, size_t nLen, bool bInternal = false);	
	void		MultiRemoveAt(size_t nIndex);
	void		MultiSetAt(size_t nIndex, LPCTSTR lpszVal);
	LPTSTR		GetMulti(LPTSTR lpszDest, size_t nMax = _MAX_REG_VALUE);
	LPCTSTR		MultiGetAt(size_t nIndex);	
	size_t		MultiLength(bool bInternal = false);
	size_t		MultiCount();
		

	void		SetBinary(LPBYTE lpbValue, size_t nLen);	
	void		GetBinary(LPBYTE lpbDest, size_t nMaxLen);
	size_t		GetBinaryLength();
	bool		Convertible() { return __bConvertable; }

	__inline	void SetOwner(CRegistry* Owner) { __cregOwner = Owner; }
	
	template <class T>void SetStruct(T &type) { SetBinary((LPBYTE) &type, sizeof(T)); }
	template <class T>void GetStruct(T &type) { GetBinary((LPBYTE) &type, sizeof(T)); }
	
	__inline	bool IsString()		{ return (iType == REG_SZ); }
	__inline	bool IsDWORD()		{ return (iType == REG_DWORD); }
	__inline	bool IsBinary()		{ return (iType == REG_BINARY); }	
	__inline	bool IsMultiString() { return (iType == REG_MULTI_SZ); }
	
	__inline	bool IsStored()		{ return __bStored; }
	__inline	bool Exists()		{ return __bStored; }

	__inline	void MultiClear()	{ SetMulti(_T("\0"), 2); }
	__inline	void MultiAdd(LPCTSTR lpszVal) { MultiSetAt(MultiCount(), lpszVal); }

protected:

	CRegistry*	__cregOwner;
	bool		__bConvertable;
	bool		__bStored;

private:

	/* Create a variable for each prominent data type */

	DWORD		dwDWORD;	
	LPTSTR		lpszStr;
		
	std::vector<BYTE> vBytes;
	std::vector<tstring> vMultiString;
};


#endif
