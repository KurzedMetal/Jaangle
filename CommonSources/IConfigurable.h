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
#ifndef _IConfigurable_h_
#define _IConfigurable_h_


class IConfigurable
{
public:
	enum SettingValueTypeEnum
	{
		COVT_Unknown,
		COVT_Int,		//An INT or a selection from availableValues
		COVT_Bool,
		COVT_LPCTSTR,
		COVT_Last
	};
	struct SettingInfo
	{
		SettingInfo():type(COVT_Unknown),name(NULL),desc(NULL),availableValues(NULL){}
		SettingValueTypeEnum type;
		LPCTSTR name;
		LPCTSTR desc;
		LPCTSTR* availableValues;	//NULL Terminated LPCTSTR array
	};
public:
	IConfigurable()				{}
	virtual ~IConfigurable()	{}

	virtual BOOL GetSettingInfo(INT settingIndex, SettingInfo& setting) const	= 0;

	virtual INT GetIntSetting(INT settingIndex) const							= 0;
	virtual LPCTSTR GetLPCTSTRSetting(INT settingIndex) const					= 0;
	virtual void SetIntSetting(INT settingIndex, INT intVal)					= 0;
	virtual void SetLPCTSTRSetting(INT settingIndex, LPCTSTR strVal)			= 0;

	virtual void UpdateConfiguration()											= 0;

};

class IConfigurableHelper
{
public:
	static void TransferConfiguration(const IConfigurable& src, IConfigurable& dest)
	{
		INT idx = 0;
		IConfigurable::SettingInfo setting;
		while (src.GetSettingInfo(idx, setting))
		{
			switch(setting.type)
			{
			case IConfigurable::COVT_Int:
			case IConfigurable::COVT_Bool:
				dest.SetIntSetting(idx, src.GetIntSetting(idx));
				break;
			case IConfigurable::COVT_LPCTSTR:
				dest.SetLPCTSTRSetting(idx, src.GetLPCTSTRSetting(idx));
				break;
			case IConfigurable::COVT_Unknown:
			default:
				break;
			}
			idx++;
		}
	}
};

class IConfigurationManager
{
public:
	IConfigurationManager()				{}
	virtual ~IConfigurationManager()	{}
	virtual void RegisterConfigurable(IConfigurable& configurable)		= 0;
	virtual void UnRegisterConfigurable(IConfigurable& configurable)	= 0;
	virtual void UpdateConfigurables()									= 0;

};

#endif
