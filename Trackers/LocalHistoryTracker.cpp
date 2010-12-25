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
#include "LocalHistoryTracker.h"
#include "SQLManager.h"
#include "SystemTimeFunctions.h"

void LocalHistoryTracker::SubmitTrack(Action action, const TrackerTrack& track, UINT actionTime)
{
	ASSERT(m_pSM != NULL);
	if (m_pSM != NULL && action == ACT_Played)
	{
		SYSTEMTIME st,lt;
		st = UnixTimeStampToSystemTime(actionTime);
		TIME_ZONE_INFORMATION timeZoneInfo;
		GetTimeZoneInformation(&timeZoneInfo);
		SystemTimeToTzSpecificLocalTime(&timeZoneInfo, &st, &lt);

		if (m_pSM->LogTrackInHistory(track.artist, track.title, HLA_Played, lt) == FALSE)
			TRACE(_T("@1 LocalHistoryTracker::SubmitTrack. History Track Log Failed\n"));
	}
}
