#include "jobs.h"
#include "memory.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

extern job_node *jobs;

job_type *make_job(pid_t pid, char *name, enum job_state status, char fg) {
	job_type *job = malloc(sizeof(*job));

	job->pid = pid;
	job->status = status;
	job->foreground = fg;
    
    /* fix! */
	strcpy(job->name, name);

	return job;
}

job_type *create_job(pid_t pid, char *name, enum job_state status, char fg) {
	job_type *job = malloc(sizeof(*job));

	job->pid = pid;
	job->status = status;
	job->foreground = fg;
    
    /* fix! */
	strcpy(job->name, name);

	return job;
}

job_node *make_job_node(job_type *job) {
	job_node *jn = malloc(sizeof(*jn));

	jn->job  = job;
	jn->next = NULL;
	jn->prev = NULL;

	return jn;
}

job_node *add_job(job_node *head, job_type *new) {
	job_node *newnode = make_job_node(new);

	if (!head) {
		return newnode;
	}

	job_node *iter = head;

	while (iter->next)
		iter = iter->next;

	newnode->prev = iter;
	iter->next = newnode;

	return head;
}

void delete_job(pid_t pid) {
	if (jobs && jobs->job->pid == pid) {
		job_node *del = jobs;
		jobs = jobs->next;

		if (jobs)
			jobs->prev = NULL;

		free(del->job);
		free(del);

		return;
	}

	job_node *p = jobs;

	while (p && p->job->pid != pid)
		p = p->next;

	if (p && p->job->pid == pid) {
		if (p->next)
			p->next->prev = p->prev;

		if (p->prev)
			p->prev->next = p->next;

		free(p->job);
		free(p);
	}
}
/*
void delete_job(pid_t pid) {
	job_node *head = jobs;
	if (head && head->job->pid == pid) {
		job_node *del = head;
		jobs = head->next;

		if (jobs)
			jobs->prev = NULL;

		free(del->job);
		free(del);

		return;
	}

	job_node *p = head;

	while (p && p->job->pid != pid)
		p = p->next;

	if (p && p->job->pid == pid) {
		if (p->next)
			p->next->prev = p->prev;

		if (p->prev)
			p->prev->next = p->next;

		free(p->job);
		free(p);
	}
}
*/
void update_status(job_type *job, int status) {
	if (!job) {
		return;
	}

	if (WIFSTOPPED(status)) {
		job->status = paused;
		job->foreground = 0;
	}
	else if (WIFCONTINUED(status)){
		job->status = running;
	}
	else  {
			job->status = finished;
			job->foreground = 0;
			delete_job(job->pid);
	}
}

job_type *get_fg_job() {
	job_node *list = jobs;
	while (list && !list->job->foreground)
		list = list->next;

	return (list) ? list->job : NULL;
}

job_type *get_job(int searchValue, int searchParameter) {
	job_node *list = jobs;
	switch(searchParameter) {
		case BY_PROCESS_ID:
			while (list != NULL) {
				if (list->job->pid == searchValue)
					return list->job;
				else
					list = list->next;
			}
			break;
		case BY_JOB_ID:
			while (list != NULL) {
				if (list->job->pid == searchValue)
					return list->job;
				else
					list = list->next;
			}
			break;
		case BY_JOB_STATUS:
			while (list != NULL) {
				if (list->job->status == searchValue)
					return list->job;
				else
					list = list->next;
			}
			break;
		case BY_FOREGROUND_JOB:
			while (list != NULL) {
				if (list->job->foreground == searchValue)
					return list->job;
				else
					list = list->next;
			}
			break;
		default:
			return NULL;
			break;
	}

	return NULL;
}

void invoke_fg(pid_t pid) {
	job_node *list = jobs;
	job_type *job = get_job(pid, BY_JOB_ID);

	if (!job) {
		fprintf(stderr, "\n\t** Nao ha jobs com o pid determinado (%d). **\n\n", pid);
		return;
	}
	if (job->status == finished) {
		fprintf(stderr, "\n\t** O job com pid %d ja terminou. **\n\n", pid);
		return;
	}
	if (job->foreground) {
		fprintf(stderr, "\n\t** O job com pid %d ja esta rodando em foreground. **\n\n", pid);
		return;
	}

	job->foreground = 1;
	job->status = running;

	kill(job->pid, SIGCONT);
	sleep(10);
	waitpid(job->pid, NULL, 0);
}

void invoke_bg(pid_t pid) {
	job_node *list = jobs;
	job_type *job = get_job(pid, BY_JOB_ID);

	if (!job) {
		fprintf(stderr, "\n\t** Nao ha jobs com o pid determinado (%d). **\n\n", pid);
		return;
	}

	if (job->status == finished) {
		fprintf(stderr, "\n\t** O job com pid %d ja terminou. **\n\n", pid);
		return;
	}

	job->foreground = 0;
	job->status = running;

	kill(job->pid, SIGCONT);
}

void print_jobs(job_node *head) {
	if (!head) {
		puts("\n\tNenhum job foi registrado ainda.");
		return;
	}

	puts(
		" [Nome do Job]\t\t[pid]\t\t[status]\t\t[foreground]\n"
		" -------------\t\t-----\t\t--------\t\t------------\n"
	);

    char *job_status_names[] = {
		"Executando", "Pausado", "Terminado"
	};

	job_node *iter = head;
	while (iter) {
		printf(" %10.10s\t\t%u\t\t%-10.10s\t\t%-10.10s\n", 
			iter->job->name,
			iter->job->pid,
			job_status_names[iter->job->status],
			iter->job->foreground ? "Sim" : "Nao");

		iter = iter->next;
	}
	puts("");
}


