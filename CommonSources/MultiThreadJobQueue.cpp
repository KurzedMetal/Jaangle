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
#include "MultiThreadJobQueue.h"



#ifdef _UNITTESTING

class TestJob:public Job
{
public:
	virtual ~TestJob()												{}
	virtual State GetState()	{return m_state;}
	virtual void Execute()
	{
		ASSERT(m_state != ST_Running);
		if (m_state != ST_Running)
		{
			m_state = ST_Running;
			srand(m_data * GetTickCount());
			DWORD delay = 4000 + (rand() - RAND_MAX / 2) / 10;
			TRACE(_T("TestJob::Execute. -->ENTER (Thread delay: %d) (Data: %d)\r\n"), delay, m_data);
#ifdef _ABORTABLE_LOOP
			DWORD tick = GetTickCount();
			while (GetTickCount() - tick < delay)
			{
				Sleep(0);
				if (m_bAbort)
				{
					TRACE(_T("TestJob::Execute. Aborting JobID: %d\r\n"), m_data);
					break;
				}
			}
#else
			Sleep(delay);
#endif
			if (m_bAbort)
			{
				m_bAbort = FALSE;
				m_state = ST_Aborted;
			}
			else
			{
				m_state = ST_Finished;
				m_result = m_data + 1;//A calculation 
			}
			TRACE(_T("TestJob::Execute. <--EXIT (Data: %d)\r\n"), m_data);
		}
	}
	virtual void Abort()		
	{
		TRACE(_T("TestJob::Abort. Request Aborting JobID: %d\r\n"), m_data);
		m_bAbort = TRUE;
	}
	INT GetData()				{return m_data;}
	UINT GetResult()			
	{
		if (m_state == ST_Running)
			return 0;
		m_state = ST_Idle;//
		return m_result;
	}
	static TestJob* CreateTestJob(UINT data)
	{
		return new TestJob(data);
	}
private:
	TestJob(UINT data):m_result(0),m_bAbort(FALSE),m_state(ST_Idle),m_data(data)		{}
	void SetData(INT data)		
	{
		ASSERT(m_state != ST_Running);
		if (m_state != ST_Running)
			m_data = data;
	}
	UINT m_data;
	UINT m_result;
	BOOL m_bAbort;
	State m_state;
};

void TraceStats(MultiThreadJobQueue::JobStatisics& stats)
{
	TRACE(_T("TestMultiThreadJobQueue. Stats waiting: %d working:%d\r\n"), 
		stats.waiting,
		stats.working);
	TRACE(_T("TestMultiThreadJobQueue. Stats finished: %d total:%d\r\n"), 
		stats.finished,
		stats.total);
}

BOOL TestMultiThreadJobQueue()
{
	UNITTESTSTART;

	if (TRUE)	//Hard Destruction Test - Check for leaks
	{
		MultiThreadJobQueue q;
		q.SetMaxThreads(50);
		for (int i = 0; i<350; i++)
			q.AddJob(JobSP(TestJob::CreateTestJob(i)));
		Sleep(100);
	}
	if (TRUE)	//1 Thread test
	{
		MultiThreadJobQueue q;
		q.SetMaxThreads(1);
		for (int i = 0; i<4; i++)
			q.AddJob(JobSP(TestJob::CreateTestJob(i)));
		while ((q.GetStatistics().waiting + q.GetStatistics().working) > 0)
			Sleep(100);
	}
	if (TRUE)//General Test
	{
		MultiThreadJobQueue q;
		q.SetMaxThreads(200);
		TraceStats(q.GetStatistics());
		for (int i = 0; i<1000; i++)
			q.AddJob(JobSP(TestJob::CreateTestJob(i)));
		int newThreads = 0;
		INT tick = GetTickCount();
		BOOL m_bAbortJobs = FALSE;

		while (TRUE)
		{
			JobSP job;
			while (q.RetrieveFinishedJob(job))
			{
				Job::State jst = job->GetState();
				UNITTEST(jst == Job::ST_Finished || jst == Job::ST_Aborted);
				if (jst == Job::ST_Finished || jst == Job::ST_Aborted)
				{
					TestJob* pTestJob = dynamic_cast<TestJob*>(job.get());
					UNITTEST(pTestJob != NULL);
					if (pTestJob != NULL)
					{
						INT jobID = pTestJob->GetData();
						UNITTEST(jobID + 1 == pTestJob->GetResult());
						if (jobID %7 == 0)
							q.SetMaxThreads(5);
						if (jobID %9 == 0)
							q.SetMaxThreads(20);
					}
				}
			}
			MultiThreadJobQueue::JobStatisics stats = q.GetStatistics();
			TraceStats(stats);
			if ((stats.waiting + stats.working + stats.finished) == 0)
				break;	//=========================Exit Here
			INT timePassed = GetTickCount() - tick;
			TRACE(_T("[%d]\r\n"), timePassed);
			if (timePassed < 10000)
			{
				TRACE(_T("TestMultiThreadJobQueue. _+_ Adding Job\r\n"));
				q.AddJob(JobSP(TestJob::CreateTestJob(++newThreads + 1000)));
			}
			if (timePassed > 5000 && timePassed < 10000 && q.IsJobAssignerActive())
			{
				TRACE(_T("TestMultiThreadJobQueue. -=- Pausing\r\n"));
				q.Pause();
			}
			if (timePassed > 10000 && !q.IsJobAssignerActive())
			{
				TRACE(_T("TestMultiThreadJobQueue. ... Resuming\r\n"));
				q.Resume();
			}
			if (timePassed > 50000 && m_bAbortJobs == FALSE)
			{
				TRACE(_T("TestMultiThreadJobQueue. ... AbortJobs\r\n"));
				q.AbortJobs();
				m_bAbortJobs = TRUE;
			}

			Sleep(100);
		}
	}


	return TRUE;


}
#endif



static int gJobThreadInstances = 0;



JobThreadRunner::JobThreadRunner():
m_pThread(0),m_state(ST_Idle),m_bAbortThread(FALSE),m_abortTick(0)
{
	TRACE(_T("JobThreadRunner Instances (+) %d\r\n"), ++gJobThreadInstances);
}

JobThreadRunner::~JobThreadRunner()
{
	m_bAbortThread = TRUE;
	if (m_abortTick == 0)
		RequestJobAbort();
	INT timeOut =  3000 - (GetTickCount() - m_abortTick);
	if (timeOut < 10)
		timeOut = 10;
	TRACE(_T("JobThreadRunner::~JobThreadRunner. Timeout: %d\r\n"), timeOut);
	if (!MultiThreadJobQueue::LeaveThread(m_pThread->m_hThread, timeOut))
		TRACE(_T("@1 *** JobThreadRunner::~JobThreadRunner. Thread Failed to be properly closed."));
	delete m_pThread;
	TRACE(_T("JobThreadRunner Instances (-) %d\r\n"), --gJobThreadInstances);
}

BOOL JobThreadRunner::StartJob(JobSP job)
{
	ASSERT(job.get() != NULL && m_state == ST_Idle);
	if (job.get() == NULL || m_state != ST_Idle)
		return FALSE;
	m_abortTick = 0;
	m_bAbortThread = FALSE;
	m_job = job;
	m_state = ST_Running;
	if (m_pThread != 0)
	{
		DWORD exitCode = 0;
		GetExitCodeThread(m_pThread->m_hThread, &exitCode);
		if (exitCode != STILL_ACTIVE)
		{
			delete m_pThread;
			m_pThread = NULL;
		}
	}
	if (m_pThread == 0)
	{
		m_pThread = AfxBeginThread(JobThreadRunner::RunJobST, this);
		m_pThread->m_bAutoDelete = FALSE;
	}
	return TRUE;
}
void JobThreadRunner::RequestJobAbort()
{
	TRACE(_T("JobThreadRunner::RequestAbort...\r\n"));
	if (m_job.get() != NULL && m_state == ST_Running)
	{
		m_job->Abort();
	}
}

void JobThreadRunner::RequestThreadAbort()
{
	TRACE(_T("JobThreadRunner::RequestThreadAbort...\r\n"));
	m_bAbortThread = TRUE;
	m_abortTick = GetTickCount();
}


JobSP JobThreadRunner::GetJob()
{
	return m_job;
}

UINT JobThreadRunner::RunJobST(LPVOID pParam)
{
	ASSERT(pParam != NULL);
	return ((JobThreadRunner*)pParam)->RunJob();
}

UINT JobThreadRunner::RunJob()
{
	while (!m_bAbortThread)
	{
		if (m_state == ST_Running)
		{
			ASSERT(m_job.get() != NULL);
			if (m_job.get())
				m_job->Execute();
			m_state = ST_Idle;
		}
		Sleep(1);
	}
	m_state = ST_Idle;
	return 0;
}

//====================================================================
//=================================			MultiThreadJobQueue
//====================================================================

MultiThreadJobQueue::MultiThreadJobQueue():
m_pThread(NULL),
m_bExitThread(FALSE),
m_bAbortAllJobs(FALSE),
m_maxThreads(3),
m_bJobAssingnerPaused(FALSE),
m_assignJobDelay(0)
{

}

MultiThreadJobQueue::~MultiThreadJobQueue()
{
	TRACE(_T("@2 MultiThreadJobQueue::~MultiThreadJobQueue\r\n"));
	m_bExitThread = TRUE;
	AbortJobs();
	if (m_pThread)
	{
		if (LeaveThread(m_pThread->m_hThread, 5000))
			delete m_pThread;
		else
			TRACE(_T("@1 *** MultiThreadJobQueue::~MultiThreadJobQueue. Thread Failed to be properly closed."));
	}
}

BOOL MultiThreadJobQueue::LeaveThread(HANDLE hThread, UINT timeOut)
{
	DWORD exitCode = 0;
	GetExitCodeThread(hThread, &exitCode);
	DWORD tickCount = GetTickCount();
	while (exitCode == STILL_ACTIVE)
	{
		Sleep(0);
		if (GetTickCount() - tickCount > timeOut)
		{
			TRACE(_T("@0 LeaveThread: Terminating Thread: %d\r\n"), (INT)hThread);
			TerminateThread(hThread, 166);
			return FALSE;
		}
		GetExitCodeThread(hThread, &exitCode);
	}
	return TRUE;
}




BOOL MultiThreadJobQueue::AddJob(JobSP job)
{
	ASSERT(job.get() != NULL);
	if (!m_bExitThread && job.get() != NULL)
	{
		CSingleLock lock(&m_cs, TRUE);
		m_statistics.waiting++;
		m_statistics.total++;
		m_waitingJobs.push(job);
		if (m_pThread == NULL)
		{
			m_pThread = AfxBeginThread(MultiThreadJobQueue::JobAssignerST, this);
			m_pThread->m_bAutoDelete = FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

MultiThreadJobQueue::JobStatisics MultiThreadJobQueue::GetStatistics()		
{
	CSingleLock lock(&m_cs, TRUE);
	m_statistics.finished = (INT) m_finishedJobs.size();
	m_statistics.waiting = (INT) m_waitingJobs.size();
	return m_statistics;
}


void MultiThreadJobQueue::AbortJobs()
{
	m_bAbortAllJobs = TRUE;
}

BOOL MultiThreadJobQueue::RetrieveFinishedJob(JobSP& job)
{
	CSingleLock lock(&m_cs, TRUE);
	if (!m_finishedJobs.empty())
	{
		job = m_finishedJobs.front();
		m_finishedJobs.pop();
		ASSERT(job.get() != NULL);
		ASSERT(job->GetState() == Job::ST_Finished || job->GetState() == Job::ST_Aborted);
		return TRUE;
	}
	return FALSE;
}

void MultiThreadJobQueue::Pause()
{
	m_bJobAssingnerPaused = TRUE;
}

void MultiThreadJobQueue::Resume()
{
	m_bJobAssingnerPaused = FALSE;
}

UINT MultiThreadJobQueue::JobAssignerST(LPVOID pParam)
{
	ASSERT(pParam != NULL);
	return ((MultiThreadJobQueue*)pParam)->JobAssigner();
}


UINT MultiThreadJobQueue::JobAssigner()
{
	std::vector<JobThreadRunner*> availableThreads;
	std::vector<JobThreadRunner*> workingThreads;
	UINT lastAssignedJobTick = 0;
	while (TRUE)//Thread Loop
	{
		//================================================
		//===Step 1.
		//===Checking for any finished jobs + Release free working threads
		//================================================
		UINT i = 0;
		while (i < workingThreads.size())
		{
			JobThreadRunner* thread = workingThreads[i];
			if (thread->GetState() == JobThreadRunner::ST_Idle)
			{
				//TRACE(_T("@3 MultiThreadJobQueue::JobAssigner. A Job Has Finished\r\n"));
				m_cs.Lock();
				m_finishedJobs.push(thread->GetJob());
				m_cs.Unlock();
				availableThreads.push_back(thread);
				workingThreads.erase(workingThreads.begin() + i);
				m_statistics.working = (INT)workingThreads.size();
			}
			else
				i++;
		}

		//================================================
		//===Step 2.
		//===Evaluating MaxThreads Option
		//================================================
		INT totalThreads = (INT)availableThreads.size() + (INT)workingThreads.size();
		while (totalThreads > m_maxThreads && !availableThreads.empty())
		{
			TRACE(_T("MultiThreadJobQueue::JobAssigner. Destroy Thread (maxThreads reduced)\r\n"));
			JobThreadRunner* thread = availableThreads.back();
			thread->RequestThreadAbort();
			availableThreads.pop_back();
			totalThreads--;
			Sleep(0);
			delete thread;
		}

		BOOL bExitThread = m_bExitThread;//Store it at this point. Otherwise
										//It may be FALSE here and TRUE later
										//This may cause Step 4. to fail
		if (m_bAbortAllJobs || bExitThread)
		{
			TRACE(_T("@2 MultiThreadJobQueue::JobAssigner m_bAbortAllJobs || m_bExitThread\r\n"));

			//================================================
			//===Step 3.1.
			//===Nicely Aborting all jobs
			//================================================
			//===Step 3.1.1.
			//--- Request Abort from all threads
			std::vector<JobThreadRunner*>::iterator it = workingThreads.begin();
			for (;it != workingThreads.end(); it++)
				(*it)->RequestJobAbort();
			//===Step 3.1.2.
			//--- Transfer all waiting jobs directly to Finished
			m_cs.Lock();
			while (!m_waitingJobs.empty())
			{
				m_finishedJobs.push(m_waitingJobs.front());
				m_waitingJobs.pop();
			}
			m_cs.Unlock();
			m_bAbortAllJobs = FALSE;
		}
		else
		{
			//================================================
			//===Step 3.2.
			//===Assigning Jobs To Threads
			//================================================
			m_cs.Lock();
			BOOL bJobsWaiting = !m_waitingJobs.empty();
			m_cs.Unlock();
			while (m_bJobAssingnerPaused == FALSE && (INT)workingThreads.size() < m_maxThreads && bJobsWaiting)
			{
				//===Evaluate Assign New Job Delay
				if (GetTickCount() - lastAssignedJobTick <= m_assignJobDelay)
					break;
				//===Assign the job to a thread
				//TRACE(_T("@3 MultiThreadJobQueue::JobAssigner. Assign a job\r\n"));
				JobThreadRunner* thread = NULL;
				if (availableThreads.size() == 0)
				{
					TRACE(_T("@3 MultiThreadJobQueue::JobAssigner. Creating New Thread\r\n"));
					thread = new JobThreadRunner;
				}
				else
				{
					TRACE(_T("@3 MultiThreadJobQueue::JobAssigner. Reusing Old Thread\r\n"));
					thread = availableThreads.back();
					availableThreads.pop_back();
				}
				lastAssignedJobTick = GetTickCount();
				workingThreads.push_back(thread);
				m_statistics.working = (INT)workingThreads.size();
				m_cs.Lock();
				JobSP job = m_waitingJobs.front();
				m_waitingJobs.pop();
				bJobsWaiting = !m_waitingJobs.empty();
				m_cs.Unlock();
				thread->StartJob(job);
			}
		}


		//================================================
		//===Step 4.
		//===Check if we need to exit
		//================================================
		if (bExitThread)
		{
			//At this stage the state should be.
			//No waiting jobs
			TRACE(_T("@2 Deleting available threads\r\n"));
			std::vector<JobThreadRunner*>::iterator it;
			for (it = availableThreads.begin(); it != availableThreads.end(); it++)
				(*it)->RequestThreadAbort();
			for (it = availableThreads.begin(); it != availableThreads.end(); it++)
				delete *it;
			TRACE(_T("@2 Deleting workingThreads threads\r\n"));
			m_cs.Lock();
			for (it = workingThreads.begin(); it != workingThreads.end(); it++)
			{
				m_finishedJobs.push((*it)->GetJob());
				(*it)->RequestThreadAbort();
			}
			m_cs.Unlock();
			for (it = workingThreads.begin(); it != workingThreads.end(); it++)
				delete *it;
			m_statistics.working = 0;
			TRACE(_T("@2 Deleting workingThreads threads FINISHED\r\n"));
			return 0;	//==============Thread Exits here
		}
		Sleep(100);
	}
	return 1;
}


