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
#include "BaseDirector.h"

BaseDirector::BaseDirector()
{
	m_MainThread = GetCurrentThreadId();
}

BaseDirector::~BaseDirector()
{
	HeartBeat();
	SendMessage(ActorMessage(MSG_DIRECTOR_CLOSING));
}

void BaseDirector::RegisterActor(Actor* pActor)
{
	ASSERT(pActor != NULL);
	//Accept Actor registrations only from main thread
	ASSERT(m_MainThread == GetCurrentThreadId());
#ifdef _DEBUG
	//Check for double Registrations
	for (std::vector<Actor*>::iterator i = m_pActors.begin(); i < m_pActors.end(); i++)
	{
		if (pActor == *i)
		{
			ASSERT(0);
			return;
		}
	}

#endif
	m_pActors.push_back(pActor);
}

void BaseDirector::UnRegisterActor(Actor* pActor)
{
	ASSERT(pActor != NULL);
	//Accept Actor un-registrations only from main thread
	ASSERT(m_MainThread == GetCurrentThreadId());
	for (std::vector<Actor*>::iterator i = m_pActors.begin(); i < m_pActors.end(); i++)
	{
		if (pActor == *i)
		{
			m_pActors.erase(i);
			return;
		}
	}
	//Check for double unregistrations. It should never reach here.
	ASSERT(0);
}

void BaseDirector::SendMessage(ActorMessage& message)
{
	if (m_MainThread != GetCurrentThreadId())
	{
		//If the sendmessage have came from another thread then we will "Post" the
		//message for the following "Heartbeat" which will run from the Main Thread
		//PostMessage Also is "Thread Safe" for the messages vector
		PostMessage(message);
		TRACE(_T("@2BaseDirector::SendMessage. Message from another thread...Posted"));
		return;
	}

	for (std::vector<Actor*>::iterator i = m_pActors.begin(); i != m_pActors.end(); i++)
		(*i)->OnNotify(message);

}

void BaseDirector::PostMessage(ActorMessage& message, BOOL bAllowGrouping/* = FALSE*/)
{
	CSingleLock lock(&m_cs, TRUE);
	if (bAllowGrouping)
	{
		for (std::list<ActorMessage>::const_iterator i = m_PostedMessages.begin(); i != m_PostedMessages.end(); i++)
		{
			if (message.ID == (*i).ID)
			{
				TRACE(_T("4FlexDirector::PostMessage. Message Found. Discarding\r\n"));
				return;
			}
		}
	}
	m_PostedMessages.push_back(message);
}

void BaseDirector::HeartBeat()
{
	//I do this, this way because a Message May Generate other Messages
	//which i want to be sent in the next heartbeat
	std::list<ActorMessage> pMessagesToBeSend;
	{//This code needs to be thread safe
		CSingleLock lock(&m_cs);
		pMessagesToBeSend = m_PostedMessages;
		m_PostedMessages.clear();
	}
	for (std::list<ActorMessage>::iterator i = pMessagesToBeSend.begin(); i != pMessagesToBeSend.end(); i++)
		SendMessage(*i);
}
