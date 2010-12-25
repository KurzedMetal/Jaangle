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
#ifndef _CompatibleWebInfoProvider_h_
#define _CompatibleWebInfoProvider_h_

#include "..\InfoProvider.h"

class CompatibleWebInfoProvider:public IInfoProvider
{
public:
	CompatibleWebInfoProvider();
	virtual ~CompatibleWebInfoProvider();
	virtual LPCTSTR Request(InfoItemType type, LPCTSTR params);
	virtual LPCTSTR GetLastInfoSource();
	virtual LPCTSTR RequestNext();
	virtual LPCTSTR GetIdentity();
	virtual void Configure()		{}
	virtual void About()			{}

private:
	LPTSTR m_pResult;
	InfoItemType m_type;
	LPTSTR m_params;
	UINT m_CurAttempt;
};



#endif
