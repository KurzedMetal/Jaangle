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
#ifndef _BassInstance_h_
#define _BassInstance_h_

class BassInstance
{
public:
	BassInstance();
	~BassInstance();
	static BOOL IsFileSupported(LPCTSTR str);
	static BOOL IsValid()	{return m_bassInstances > 0;}
	static BOOL bUseFloatDSP;
	static BOOL bUseSampleFloat;
private:
	static BOOL InitBass();
	static BOOL AddPlugin(LPCTSTR dllName, LPCTSTR supportedExtensions);

	static INT m_bassInstances;
	static std::tstring m_sFormats;
	static std::vector<DWORD> m_Plugins;

#ifdef DYN_LINK_BASS
	HINSTANCE hBass;
#endif


};

#endif
