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
#ifndef _AppMonitor_h_
#define _AppMonitor_h_

//class CRegistry;
class AppMonitor
{
public:
	AppMonitor();
	~AppMonitor();

	//=== Start Monitoring 
	//		1. Resets the RunningTime Counter
	//		2. Calculate The IsAfterCrash Flag from the bRunning Flag
	//		3. Sets the bRunning to TRUE
	//	Usually use it after Load(...)
	void StartMonitoring();
	//=== Start Monitoring 
	//		1. Updates the "Running Minutes" state value
	//		2. Sets the bRunning to FALSE
	//	Usually use it before Save(...)
	void StopMonitoring();

	BOOL Load(LPCTSTR stateString);
	BOOL Save(LPTSTR bf, UINT bfLen);


	struct State
	{
		State()
		{
			memset(this, 0, sizeof(State));
		}
		UINT Runs;
		UINT TotalRuns;
		UINT Crashes;
		UINT TotalCrashes;
		UINT AppUniqueID;
		SYSTEMTIME InstallationDate;
		UINT RunningTimeInMinutes;
		BOOL bRunning;
	};

	State& GetState()					{return m_state;}

	void IncreaseAppCount();
	void IncreaseCrashCount();

	UINT GetRunningTimeInMinutes() const;

	BOOL IsAfterCrash() const			{return m_bIsAfterCrash;}
	DOUBLE GetInstalledTime() const;


private:
	static DOUBLE GetTimeFromNow(const SYSTEMTIME* time);
	BOOL m_bIsAfterCrash;
	SYSTEMTIME m_startTime;
	State m_state;

public:
#ifdef _UNITTESTING
	static BOOL UnitTest();
#endif
};

#endif
