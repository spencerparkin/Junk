// job.cpp

// Programmed by Spencer T. Parkin

#include "job.h"

const int JOBS = 64;
static job job_buf[JOBS];
static int job_idx_stack[JOBS];
static int job_top;
static job *job_list;

// Allocate a job object.
static job *PopJob(void)
{
	if(job_top < JOBS)
	{
		return &job_buf[job_idx_stack[job_top++]];
	}
	return 0;
}

// Deallocate a job object.
static bool PushJob(job *j)
{
	if(job_top > 0)
	{
		job_idx_stack[--job_top] = j - job_buf;
		return true;
	}
	return false;
}

// Link the given job into the job-list.
static void InsertJob(job *j)
{
	j->next = job_list;
	j->prev = 0;
	if(job_list)
		job_list->prev = j;
	job_list = j;
}

// Unlink the given job from the
// job-list.  Notice that the links
// of the given job remain intact.
static void RemoveJob(job *j)
{
	if(job_list == j)
		job_list = j->next;
	if(j->next)
		j->next->prev = j->prev;
	if(j->prev)
		j->prev->next = j->next;
}

void InitJobFacility(void)
{
	job_top = 0;
	job_list = 0;

	int k;
	for(k = 0; k < JOBS; k++)
		job_idx_stack[k] = k;
}

void DeinitJobFacility(void)
{
}

// Go run all the jobs.
void RunJobs(void)
{
	job *j;
	for(j = job_list; j; j = j->next)
	{
		if(j->die || j->func(j->data))
		{
			RemoveJob(j);
			PushJob(j);
		}
	}
}

// Add the given job to the list of
// jobs to be done each frame.
job *StartJob(job_func func, void *data)
{
	job *j = PopJob();
	if(j)
	{
		j->func = func;
		j->data = data;
		j->die = false;
		InsertJob(j);
	}
	return j;
}

// This function will not attempt to
// remove the given job from the job-
// list so that we prevent already
// removed jobs from being removed twice,
// and in case the job list in the middle
// of being iterated through.
// If the job is in the list, then when
// we go to run the job, we'll see this
// flag and delete it there instead.
void StopJob(job *j)
{
	j->die = true;
}

// endof job.cpp