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
#include "BaseSubject.h"
#include "Observer.h"

BaseSubject::~BaseSubject()
{
	CSingleLock lock(&m_CS, TRUE);
	TRACE(_T("BaseSubject::~BaseSubject %d\n"), this);
	while (!m_Observers.empty())
	{
		TRACE(_T("BaseSubject::~BaseSubject. Asks An Observer to be Removed. Subject: %d Observer %d\n"), this, m_Observers[0]);
		m_Observers[0]->RemoveSubject(*this);
	}
}


BOOL BaseSubject::SubScribe(Observer& observer)
{
	CSingleLock lock(&m_CS, TRUE);
	for (std::vector<Observer*>::iterator i = m_Observers.begin(); i < m_Observers.end(); i++)
		if (&observer == (*i))
		{
			ASSERT(FALSE);//Already In
			return FALSE;
		}
	m_Observers.push_back(&observer);
	return TRUE;
}

void BaseSubject::UnSubScribe(Observer& observer)
{
	TRACE(_T("BaseSubject::UnSubScribe Subj(%d) Obs(%d)\n"), this, &observer);
	CSingleLock lock(&m_CS, TRUE);
#ifdef _DEBUG
	BOOL bSuccess = FALSE;
#endif
	for (std::vector<Observer*>::iterator i = m_Observers.begin(); i < m_Observers.end(); i++)
	{
		if (&observer == *i)
		{
#ifdef _DEBUG
			bSuccess = TRUE;
			TRACE(_T("@4\tBaseSubject::UnSubScribe. Success!\n"));
#endif			
			*i = 0;
			m_Observers.erase(i);
			break;
		}
	}
#ifdef _DEBUG
	if (!bSuccess)
	{
		ASSERT(0);
		TRACE(_T("@0\tBaseSubject::UnSubScribe. Failure!\n"));
	}
#endif
}

void BaseSubject::NotifyObservers(Notification& not)
{
	CSingleLock lock(&m_CS, TRUE);
	TRACEST(_T("BaseSubject::NotifyObservers"));
	for (std::vector<Observer*>::iterator i = m_Observers.begin(); i < m_Observers.end(); i++)
	{
		ASSERT(*i > 0);
		if (*i == 0)
			TRACE(_T("@0\tBaseSubject::UnSubScribe. Trying to call empty object!\n"));
		else
			(*i)->OnNotify(not);
	}
}

BOOL BaseSubject::HasObservers()
{
	return !m_Observers.empty();
}
