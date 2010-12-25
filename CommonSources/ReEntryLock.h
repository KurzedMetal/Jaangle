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
#ifndef _ReEntryLock_h_
#define _ReEntryLock_h_

//USAGE:
//1. Add a BOOL member variable (eg m_bLockFunction)
//2. Initialize this variable to FALSE in the class's Costructor
//3. Use it in the function you want to protect like this
//	ReEntryLock lock(m_bLockFunction);
//	if (lock.WasAlreadyLocked())
//	{
//		TRACE(_T("..."));
//		return;
//	}


class ReEntryLock
{
public:
	ReEntryLock(BOOL& memberVar):
	  m_WasAlreadyLocked(FALSE),
	  m_memberVar(memberVar)
	{
		if (m_memberVar == TRUE)
			m_WasAlreadyLocked = TRUE;
		else
			m_memberVar = TRUE;
	}
	~ReEntryLock()
	{
		if (!WasAlreadyLocked())
			m_memberVar = FALSE;
	}

	BOOL WasAlreadyLocked(){return m_WasAlreadyLocked;}
private:
	BOOL& m_memberVar;
	BOOL m_WasAlreadyLocked;
};

#endif
