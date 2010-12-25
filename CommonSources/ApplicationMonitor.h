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
#ifndef _ApplicationMonitor_h_
#define _ApplicationMonitor_h_

//class CRegistry;
class ApplicationMonitor
{
public:
	ApplicationMonitor(BOOL bReadOnlyMode);
	~ApplicationMonitor();

	BOOL InitByName(LPCTSTR applicationName);
	BOOL Init(LPCTSTR regKey, HKEY hKey);


	enum CountType
	{
		CT_First,
		CT_Runs,
		CT_TotalRuns,
		CT_Crashes,
		CT_TotalCrashes,
		CT_Last
	};

	UINT GetCount(CountType ct) const;
	void SetCount(CountType ct, UINT value);

	////===Application Counter
	//UINT GetAppCount() const			{return m_appCount;}
	//UINT GetTotalAppCount() const		{return m_totalAppCount;}
	//void ResetAppCounter();
	void IncreaseAppCount();
	//void SetTotalAppCount(UINT totalAppCount)	{m_totalAppCount = totalAppCount;}

	////===Crash Counter
	//UINT GetCrashCount() const			{return m_crashCount;}
	//UINT GetTotalCrashCount() const		{return m_totalCrashCount;}
	//void ResetCrashCounter();
	void IncreaseCrashCount();
	//void SetTotalCrashCount(UINT totalCrashCount)	{m_totalCrashCount = totalCrashCount;}

	void EnableCrashDetector(BOOL bEnable);

	//===Crash Detector
	BOOL IsAfterCrash() const			{return m_bIsAfterCrash;}

	UINT GetAppUniqueID() const			{return m_appUniqueID;}
	void GetInstallationDate(SYSTEMTIME* st) const;

	DOUBLE GetInstalledTime() const;

	UINT GetRunningTimeInMinutes() const	{return m_runningTime;}

	void SaveState();


private:
	BOOL m_bReadOnlyMode;
	static DOUBLE GetTimeFromNow(const SYSTEMTIME* time);
	BOOL WriteUINT(LPCTSTR keyName, UINT val);
	BOOL ReadUINT(LPCTSTR keyName, UINT& val, UINT defVal);

	BOOL WriteSYSTEMTIME(LPCTSTR keyName, SYSTEMTIME val);
	BOOL ReadSYSTEMTIME(LPCTSTR keyName, SYSTEMTIME& val);

	UINT m_count[CT_Last];

	//UINT m_appCount;
	//UINT m_totalAppCount;
	//UINT m_crashCount;
	//UINT m_totalCrashCount;
	BOOL m_bIsAfterCrash;
	UINT m_appUniqueID;
	SYSTEMTIME m_installationDate;
	//CRegistry* m_pReg;
	UINT m_runningTime;
	SYSTEMTIME m_startTime;
	HKEY m_hKey;
	BOOL m_bStateChanged;


public:
#ifdef _UNITTESTING
	static BOOL UnitTest();
#endif
};

#endif
