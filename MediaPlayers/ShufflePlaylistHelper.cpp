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
#include "ShufflePlayListHelper.h"

INT ShufflePlayListHelper::GetNext(INT relative)
{
	ASSERT(relative < (INT) m_randomNums.size());
	INT newTrack = GetNextShuffle(relative);
	if (newTrack == -1)
		newTrack = GetFirstShuffle(); //Well, I should have called it "Shuffle Loop"
	return newTrack;
}
INT ShufflePlayListHelper::GetPrev(INT relative)
{
	ASSERT(relative < (INT) m_randomNums.size());
	INT newTrack = GetPreviousShuffle(relative);
	if (newTrack == -1)
		newTrack = GetLastShuffle(); //Well, I should have called it "Shuffle Loop"
	return newTrack;
}

void ShufflePlayListHelper::InsertItem(INT Pos)
{
	ASSERT(Pos <= (INT) m_randomNums.size());
	UINT num = GetShuffleNumber();
	if (Pos > (INT) m_randomNums.size() || Pos < 0)
		m_randomNums.push_back(num);
	else
		m_randomNums.insert(m_randomNums.begin()+Pos , num);
}
void ShufflePlayListHelper::Clear()
{
	m_randomNums.clear();
}
void ShufflePlayListHelper::RemoveItem(INT Pos)
{
	ASSERT(Pos < (INT) m_randomNums.size());
	m_randomNums.erase(m_randomNums.begin() + Pos);
}
void ShufflePlayListHelper::SwapItems(INT index, INT newPos)
{
	ASSERT(max(index,newPos) < (INT)m_randomNums.size());
	UINT num = m_randomNums[newPos];
	m_randomNums[newPos] = m_randomNums[index];
	m_randomNums[index] = num;
}

UINT ShufflePlayListHelper::GetShuffleNumber()
{
	return rand() * RAND_MAX + rand();
}

INT ShufflePlayListHelper::GetFirstShuffle()
{
	UINT minShuffleNumber = RAND_MAX * RAND_MAX + RAND_MAX;
	INT firstItem = -1;
	UINT numItems = m_randomNums.size();
	for (UINT i = 0; i < numItems; i++)
	{
		if (m_randomNums[i] < minShuffleNumber)
		{
			minShuffleNumber = m_randomNums[i];
			firstItem = i;
		}
	}
	return firstItem;
}

INT ShufflePlayListHelper::GetLastShuffle()
{
	UINT maxShuffleNumber = 0;
	INT lastItem = -1;
	UINT numItems = m_randomNums.size();
	for (UINT i = 0; i < numItems; i++)
	{
		if (m_randomNums[i] > maxShuffleNumber)
		{
			maxShuffleNumber = m_randomNums[i];
			lastItem = i;
		}
	}
	return lastItem;
}

INT ShufflePlayListHelper::GetNextShuffle(INT relative)
{
	//INT curShuffleNumber = GetMediaInfo(m_PlayListIdx)->lParam;
	INT curMinDistance = RAND_MAX * RAND_MAX + RAND_MAX;
	INT minShuffleNumber = curMinDistance;
	INT Distance = 0;
	INT nextItem = -1;
	UINT numItems = m_randomNums.size();
	if (relative < 0)
		return 0;
	if (relative >= (INT)numItems)
		return 0;

	//We will calculate 2 things..
	//	1. The First Shuffle (min ShuffleNumber) in the case that we are at the last shuffle track
	for (UINT i = 0; i < numItems; i++)
	{
		INT shufNum = m_randomNums[i];
		Distance = shufNum - m_randomNums[relative];
		if (Distance > 0 && Distance < INT(curMinDistance))
		{
			curMinDistance = UINT(Distance);
			nextItem = i;
		}
	}
	return nextItem;
}

INT ShufflePlayListHelper::GetPreviousShuffle(INT relative)
{
	//UINT curShuffleNumber = GetMediaInfo(m_PlayListIdx)->lParam;
	UINT curMinDistance = RAND_MAX * RAND_MAX + RAND_MAX;
	INT maxShuffleNumber = 0;
	INT Distance = 0;
	INT previousItem = -1;
	UINT numItems = m_randomNums.size();
	
	if (relative < 0)
		return 0;
	if (relative >= (INT)numItems)
		return 0;

	//We will calculate 2 things..
	//	1. The LAST Shuffle (MAX ShuffleNumber) in the case that we are at the FIRST shuffle track
	//	2. The item with the LESS SMALLER Value than the current (Smaller negative distance)
	for (UINT i = 0; i < numItems; i++)
	{
		INT shufNum = m_randomNums[i];
		Distance = m_randomNums[relative] - shufNum;
		if (Distance > 0 && Distance < INT(curMinDistance))
		{
			curMinDistance = UINT(Distance);
			previousItem = i;
		}
	}
	return previousItem;
}
