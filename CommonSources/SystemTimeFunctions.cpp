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
#include "SystemTimeFunctions.h"

BOOL SystemTimeFunctions_UnitTest()
{
	SYSTEMTIME lt;
	GetSystemTime(&lt);
	UINT unixTimeStamp = SystemTime2UnixTimeStamp(lt);
	SYSTEMTIME st = UnixTimeStampToSystemTime(unixTimeStamp);
	ASSERT(st.wSecond == lt.wSecond);
	return TRUE;
}


DOUBLE ConvertDaysToTimeInterval(DOUBLE days, DateIntervalEnum di)
{
	ASSERT(di < DI_Last);
	switch (di)
	{
	case DI_Year:
		return days / 365;
	case DI_Day:
		return days;
	case DI_Hour:
		return days * 24.0;
	case DI_Minute:
		return days * 24.0 * 60.0;
	case DI_Second:
		return days * 24.0 * 60.0 * 60.0;
	}
	return 0.0;

}
DOUBLE DateDiff(const SYSTEMTIME& st1, const SYSTEMTIME& st2, DateIntervalEnum di)
{
	DOUBLE t1 = 0;
	SystemTimeToVariantTime((SYSTEMTIME*)&st1, &t1);
	DOUBLE t2 = 0;
	SystemTimeToVariantTime((SYSTEMTIME*)&st2, &t2);
	return ConvertDaysToTimeInterval(t1 - t2, di);
}

DOUBLE DateAdd(const SYSTEMTIME& st1, const SYSTEMTIME& st2, DateIntervalEnum di)
{
	DOUBLE t1 = 0;
	SystemTimeToVariantTime((SYSTEMTIME*)&st1, &t1);
	DOUBLE t2 = 0;
	SystemTimeToVariantTime((SYSTEMTIME*)&st2, &t2);
	return ConvertDaysToTimeInterval(t1 + t2, di);
}


UINT SystemTime2UnixTimeStamp(const SYSTEMTIME& st)
{
	SYSTEMTIME unixTime;
	memset(&unixTime, 0, sizeof(SYSTEMTIME));
	unixTime.wYear = 1970;
	unixTime.wMonth = 1;
	unixTime.wDay = 1;
	return UINT(DateDiff(st, unixTime, DI_Second));
}

SYSTEMTIME UnixTimeStampToSystemTime(UINT timeStamp)
{
	SYSTEMTIME unixTime;
	memset(&unixTime, 0, sizeof(SYSTEMTIME));
	unixTime.wYear = 1970;
	unixTime.wMonth = 1;
	unixTime.wDay = 1;
	DOUBLE ut = 0;
	SystemTimeToVariantTime(&unixTime, &ut);
	ut += timeStamp / (24.0 * 3600.0);
	SYSTEMTIME st;
	VariantTimeToSystemTime(ut, &st);
	return st;
}


UINT GetCurrentUnixTimeStamp()
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	return SystemTime2UnixTimeStamp(st);
}


