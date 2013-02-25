#ifndef _MultiThreadJobQueue_h_
#define _MultiThreadJobQueue_h_

#include <vector>
#include <queue>
#include <afxmt.h>
#include <boost/shared_ptr.hpp>

#ifdef _UNITTESTING
BOOL TestMultiThreadJobQueue();
#endif

class Job
{
public:
	virtual ~Job()				{}
	enum State
	{
		ST_Idle,			//When Starting
		ST_Running,			//While Running
		ST_Aborted,			//When it has finished "Execute" normally. (Used like ST_Idle)
		ST_Finished,		//When it has aborted "Execute". (Used like ST_Idle)
		ST_Last
	};
	virtual State GetState()	= 0;
	virtual void Execute()		= 0;
	virtual void Abort()		= 0;
};

typedef boost::shared_ptr<Job> JobSP;

class JobThreadRunner
{
public:
	JobThreadRunner();
	~JobThreadRunner();
	enum State
	{
		ST_Idle,
		ST_Paused,
		ST_Running,
		ST_Last
	};		
	State GetState()						{return m_state;}
	BOOL StartJob(JobSP job);
	//RequestJobAbort: 
	//Calls the Job->Abort only. Does NOT stop/Terminate the thread
	void RequestJobAbort();
	void RequestThreadAbort();
	JobSP GetJob();

private:
	static UINT RunJobST(LPVOID pParam);
	UINT RunJob();
	CWinThread* m_pThread;
	JobSP  m_job;
	volatile State m_state;
	volatile BOOL m_bAbortThread;
	volatile DWORD m_abortTick;
};


class MultiThreadJobQueue
{
public:
	MultiThreadJobQueue();
	virtual ~MultiThreadJobQueue();

	virtual BOOL AddJob(JobSP job);
	virtual void AbortJobs();
	virtual void Pause();
	virtual void Resume();
	//Retrieves Finished Job and Removes job (This can be called only once).
	BOOL RetrieveFinishedJob(JobSP& job);

	virtual BOOL IsJobAssignerActive()			{return !m_bJobAssingnerPaused;}

	//===This sets the maximum number of available threads.
	//		If this value is reached the new Jobs are queued
	virtual INT GetMaxThreads()					{return m_maxThreads;}
	virtual void SetMaxThreads(INT threads)		{m_maxThreads = threads;}
	//===This sets a delay before assigning a new job
	//		Default value is 0 (No delay)
	//		This feature can help if you want for example to retrieve web pages and
	//		you don't want to overload the server with simultaneous requests.
	//		Example: MaxThreads is 3. You assign 10 new jobs. Set Delay to 200ms
	//		At 0ms the first job is assigned. At 200ms the second at 400 the third.
	//		At 1000ms the first job is finished and a new thread is initiated.
	virtual UINT GetAssignJobDelay()			{return m_assignJobDelay;}
	virtual void SetAssignJobDelay(UINT ms)		{m_assignJobDelay = ms;}

	struct JobStatisics
	{
		JobStatisics():waiting(0),working(0),finished(0),total(0)	{}
		INT waiting;
		INT working;
		INT finished;
		INT total;
	};
	virtual JobStatisics GetStatistics();
	//virtual INT GetWorkingThreads()				{return m_workingThreads;}
	static BOOL LeaveThread(HANDLE hThread, UINT timeOut);

private:
	JobStatisics m_statistics;

	INT volatile m_maxThreads;
	//INT m_workingThreads;
	std::queue<JobSP> m_waitingJobs;
	std::queue<JobSP> m_finishedJobs;

	static UINT JobAssignerST(LPVOID pParam);
	UINT JobAssigner();
	typedef std::vector<JobThreadRunner*> ThreadRunners;
	UINT m_assignJobDelay;
	
	BOOL volatile m_bJobAssingnerPaused;
	BOOL volatile m_bExitThread;
	BOOL volatile m_bAbortAllJobs;
	CCriticalSection m_cs;
	CWinThread* m_pThread;
};

#endif
