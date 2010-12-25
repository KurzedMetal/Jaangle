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
#ifndef _Notification_H_
#define _Notification_H_

//typedef void* NotParam;
typedef UINT NotParam;

class Subject;

class Notification
{
public://accessors
	virtual Subject* GetSender()	= 0;
	virtual UINT GetID()			= 0;
	virtual NotParam GetParam1()	= 0;
	virtual NotParam GetParam2()	= 0;
};

class BaseNotification: public Notification
{
public:
	BaseNotification(Subject* subj, UINT ID, NotParam p1 = NULL, NotParam p2 = NULL):
				m_ID(ID),
				m_Subj(subj),
				m_p1(p1),
				m_p2(p2)		
				{
					ASSERT(m_Subj != NULL);
					ASSERT(m_ID != NULL);
				}
	BaseNotification(UINT ID, NotParam p1 = NULL, NotParam p2 = NULL):
				m_ID(ID),
				m_Subj(NULL),
				m_p1(p1),
				m_p2(p2)		
				{
					ASSERT(m_ID != NULL);
				}	

public://accessors
	virtual inline UINT GetID()			{return(m_ID);}
	virtual inline Subject* GetSender()	{return(m_Subj);}
	virtual inline NotParam GetParam1()	{return(m_p1);}
	virtual inline NotParam GetParam2()	{return(m_p2);}


private:
	UINT m_ID;
	Subject* m_Subj;
	NotParam m_p1;
	NotParam m_p2;

};

#endif
