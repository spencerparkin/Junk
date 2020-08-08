// job.h

// Programmed by Spencer T. Parkin

#ifndef __job_h__
#define __job_h__

typedef bool (* job_func)(void *);

struct job
{
	friend void InitJobFacility(void);
	friend void DeinitJobFacility(void);
	friend void RunJobs(void);
	friend job *StartJob(job_func func, void *data);	
	friend void StopJob(job *j);
	friend static void InsertJob(job *j);
	friend static void RemoveJob(job *j);
	friend static bool PushJob(job *j);
	friend static job *PopJob(void);
	private:
	job_func func;
	void *data;
	job *next, *prev;
	bool die;
};

#endif __job_h__

// endof job.h