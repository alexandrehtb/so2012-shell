#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>
#include <unistd.h>

#define BY_PROCESS_ID 1
#define BY_JOB_ID 2
#define BY_JOB_STATUS 3
#define BY_FOREGROUND_JOB 4

enum job_state
{
    running, paused, finished
};

typedef struct
{
    pid_t pid;
    char name[512];
    enum job_state status;
    char foreground;
} job_type;

typedef struct job_node
{
    job_type *job;
    struct job_node *next;
    struct job_node *prev;
} job_node;

job_type *make_job(pid_t pid, char *name, enum job_state status, char fg);
job_type *create_job(pid_t pid, char *name, enum job_state status, char fg);
job_node *make_job_node(job_type *job);
job_node *add_job(job_node *head, job_type *new);
void delete_job(pid_t pid);
void update_status(job_type *job, int status);
job_type *get_fg_job();
job_type *get_job(int searchValue, int searchParameter);
void invoke_fg(pid_t pid);
void invoke_bg(pid_t pid);
void print_jobs(job_node *head);

#endif
