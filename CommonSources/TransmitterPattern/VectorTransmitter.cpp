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
#include "VectorTransmitter.h"

VectorTransmitter::VectorTransmitter()
{
	m_MainThread = GetCurrentThreadId();
}

VectorTransmitter::~VectorTransmitter()
{
	HeartBeat();						//Dispatch All Posted messages
	SendMessage(MSG_TRANSMITTER_QUIT);	//Send a "Transmitter QUIT message"
}

void VectorTransmitter::RegisterReceiver(Receiver& receiver)
{
	//Accept Actor registrations only from main thread
	ASSERT(m_MainThread == GetCurrentThreadId());
	if (m_MainThread == GetCurrentThreadId())
	{
		for (std::vector<Receiver*>::iterator i = m_pReceivers.begin(); i < m_pReceivers.end(); i++)
		{
			if (&receiver == *i)
			{
				ASSERT(0);
				return;
			}
		}
		m_pReceivers.push_back(&receiver);
	}
}

void VectorTransmitter::UnRegisterReceiver(Receiver& receiver)
{
	//Accept Actor un-registrations only from main thread
	ASSERT(m_MainThread == GetCurrentThreadId());
	if (m_MainThread == GetCurrentThreadId())
	{
		for (std::vector<Receiver*>::iterator i = m_pReceivers.begin(); i < m_pReceivers.end(); i++)
		{
			if (&receiver == *i)
			{
				m_pReceivers.erase(i);
				return;
			}
		}
	}
	//Check for double unregistrations. It should never reach here.
	ASSERT(0);
}

void VectorTransmitter::SendMessage(UINT msgID, LPVOID pSender, MessageDetails* pMsgDetail)
{
	if (m_MainThread != GetCurrentThreadId())
	{
		TRACE(_T("@2BaseDirector::SendMessage. Message from another thread...Posted"));
		//If the SendMessage have came from another thread then we will "Post" the
		//message for the following "Heartbeat" which will run from the Main Thread
		//PostMessage Also is "Thread Safe" for the messages vector
		PostMessage(msgID, pSender, pMsgDetail, FALSE);
	}
	else
	{
		for (std::vector<Receiver*>::iterator i = m_pReceivers.begin(); i != m_pReceivers.end(); i++)
			(*i)->OnMessage(msgID, pSender, pMsgDetail);
	}


}

void VectorTransmitter::PostMessage(UINT msgID, LPVOID pSender, MessageDetails* pMsgDetail, BOOL bGroupSimilar)
{
	CSingleLock lock(&m_cs, TRUE);
	if (bGroupSimilar)
	{
		for (std::list<MessageStruct>::const_iterator i = m_PostedMessages.begin(); i != m_PostedMessages.end(); i++)
		{
			if (msgID == (*i).msgID)
			{
				TRACE(_T("4FlexDirector::PostMessage. Message Found. Discarding\r\n"));
				return;
			}
		}
	}
	MessageStruct msg;
	msg.msgID = msgID;
	msg.pSender = pSender;
	msg.pMsgDetail = pMsgDetail;
	m_PostedMessages.push_back(msg);
}

void VectorTransmitter::HeartBeat()
{
	//I do this, this way because a Message May Generate other Messages
	//which i want to be sent in the next heartbeat
	CSingleLock lock(&m_cs, TRUE);
	size_t messagesToPost = m_PostedMessages.size();
	for (size_t i = 0; i < messagesToPost; i++)
	{
		MessageStruct& msgStr = m_PostedMessages.front();
		SendMessage(msgStr.msgID, msgStr.pSender, msgStr.pMsgDetail);
		m_PostedMessages.pop_front();
	}
}
