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
#include "MultiObserver.h"


MultiObserver::MultiObserver ()
{

}

MultiObserver::~MultiObserver()				
{
	//What can happen.....
	//If you re in a multithreaded a environment then you should use the
	//RemoveAllSubjects in the destructor of the original object
	//Otherwise strange things may happen (crashes)
	//While The the inherited object dies and while this destructor 
	//is called (before the call to removeallsubjects) the other thread
	//may call Notifyobservers and then call the DESTRUCTED OnNotify
	//of the inherited object
	//--> pure virtual call error
	TRACE(_T("@4MultiObserver::~MultiObserver (%d)\n"), this);
	RemoveAllSubjects();
}

void MultiObserver::AddSubject(Subject& subj)	
{
	TRACE(_T("@4MultiObserver::AddSubject Obs(%d) Subj(%d)\r\n"), this, &subj);
	if (subj.SubScribe(*this))
		m_Subjects.push_back(&subj);
}

void MultiObserver::RemoveSubject(Subject& subj)	
{
	TRACE(_T("@4MultiObserver::RemoveSubject. Obs(%d) Subj(%d)\n"), this, &subj);
	for (std::vector<Subject*>::iterator i = m_Subjects.begin(); i < m_Subjects.end(); i++)
	{
		if ((*i) == &subj)
		{
			TRACE(_T("\tMultiObserver::RemoveSubject. Success!\n"));
			subj.UnSubScribe(*this);
			m_Subjects.erase(i);
			break;
		}
	}
}

void MultiObserver::RemoveAllSubjects()
{
	while (!m_Subjects.empty())
	{
		TRACE(_T("@4MultiObserver::RemoveAllSubjects. Tries to unsubscribe from his Subject, Obs(%d) Subj(%d)\n"), this, m_Subjects[0]);
		//RemoveSubject(*m_Subjects[0]);
		m_Subjects[0]->UnSubScribe(*this);
		m_Subjects.erase(m_Subjects.begin());
	}	
}
