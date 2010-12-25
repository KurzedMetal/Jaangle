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
#ifndef _TSCONFIGURABLE_H_
#define _TSCONFIGURABLE_H_

#include "IConfigurable.h"

class TSConfigurable: public IConfigurable
{
public:
	TSConfigurable();
	virtual ~TSConfigurable();

	virtual BOOL GetSettingInfo(INT settingIndex, IConfigurable::SettingInfo& setting) const;
	virtual INT GetIntSetting(INT settingIndex) const;
	virtual LPCTSTR GetLPCTSTRSetting(INT settingIndex) const;
	virtual void SetIntSetting(INT settingIndex, INT intVal);
	virtual void SetLPCTSTRSetting(INT settingIndex, LPCTSTR strVal);

	virtual void UpdateConfiguration()											= 0;

};


#endif
