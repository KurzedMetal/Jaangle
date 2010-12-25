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
#ifndef _UnitTest_h_
#define _UnitTest_h_

//Include this header to stdafx.h to enable UnitTesting
//Call RunTests somewhere in the program (Usually InitInstance())

#define _UNITTESTING

#define UNITTESTSTART 	if (MessageBox(0, _T("Start Unit Test?"), (LPCTSTR)CA2CT(__FUNCTION__), MB_YESNO) == IDNO) return TRUE

#define UNITTEST(x) if (!(x))							\
					{									\
					UnitTestFailed(#x, __FILE__);		\
						return FALSE;					\
					}									\
					else								\
						UnitTestSuccess(#x, __FILE__);	\


					
BOOL RunTests();
void UnitTestFailed(LPCSTR message, LPCSTR file);
void UnitTestSuccess(LPCSTR message, LPCSTR file);

#endif

