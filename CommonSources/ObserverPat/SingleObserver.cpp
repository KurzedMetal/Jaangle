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
#include "SingleObserver.h"


SingleObserver::SingleObserver ():
m_pSubject(NULL)
{

}

SingleObserver::~SingleObserver ()				
{
	TRACE(_T("SingleObserver::~SingleObserver %d\r\n"), this);
	if (m_pSubject != NULL)
		RemoveSubject(*m_pSubject);
}

void SingleObserver::AddSubject(Subject& subj)	
{
	TRACE(_T("SingleObserver::AddSubject Obs(%d) Subj(%d)\n"), this, &subj);
	ASSERT(m_pSubject == NULL);//Already have a subject
	subj.SubScribe(*this);
	m_pSubject = &subj;
}

void SingleObserver::RemoveSubject(Subject& subj)	
{
	TRACE(_T("SingleObserver::RemoveSubject Obs(%d) Subj(%d)\n"), this, &subj);
	ASSERT(m_pSubject == &subj);//It should have a subject
	m_pSubject->UnSubScribe(*this);
	m_pSubject = NULL;
}
