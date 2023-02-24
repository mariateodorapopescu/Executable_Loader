#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include "exec_parser.h"
static so_exec_t *exec;
static struct sigaction old_action;
static int fd;
static int **v;
static void segv_handler(int signum, siginfo_t *info, void *context)
{
	/* TODO - actual loader implementation */
	char *addr;
	int pgsize;
	int i;
	int ok;
	int segm_x;
	int bsss;
	int addr_new;
	int addr_new2;
	int pg_x;
	int rc;
	void *unde;
	int dif;
	int dim;
	int memm;
	int reed;
	reed = 0;
	memm = 0;
	ok = 0;
	pgsize = getpagesize();
	bsss = 0;
	if (signum != SIGSEGV)
	{
		old_action.sa_sigaction(signum, info, context);
		return;
	}
	if (info == NULL)
	{
		old_action.sa_sigaction(signum, info, context);
		return;
	}
	addr = (char *)info->si_addr;
	if (addr == NULL)
	{
		old_action.sa_sigaction(signum, info, context);
		return;
	}
	for (i = 0; i < exec->segments_no; i++)
	{
		addr_new = (exec->segments[i].vaddr);
		addr_new2 = (exec->segments[i].vaddr + exec->segments[i].mem_size);
		if (addr_new <= (int)addr && (int)addr < addr_new2)
		{
			ok = 1;
			segm_x = i;
		}
	}
	if (ok == 1)
	{
		pg_x = ceil(((int)addr - exec->segments[segm_x].vaddr) / pgsize);
		if (exec->segments[segm_x].mem_size - exec->segments[segm_x].file_size > 0)
		{
			bsss = 1;
		}
		if (v[segm_x][pg_x] == 0)
		{
			unde = mmap((void *)(exec->segments[segm_x].vaddr + pg_x * pgsize), pgsize, PERM_R | PERM_W | PERM_X, MAP_PRIVATE | MAP_FIXED, fd, pg_x * pgsize + exec->segments[segm_x].offset);
			if (bsss == 1)
			{
				if (pg_x * pgsize < exec->segments[segm_x].file_size)
				{
					dif = exec->segments[segm_x].file_size - pg_x * pgsize;
					if (dif >= pgsize)
					{
						dim = pgsize;
					}
					else
					{
						dim = dif;
					}
					rc = lseek(fd, exec->segments[segm_x].offset + pg_x * pgsize, SEEK_SET);
					if (rc < 0)
					{
						perror("lseek");
						exit(EXIT_FAILURE);
					}
					if (((pg_x + 1) * pgsize <= exec->segments[segm_x].file_size))
					{
						dim = pgsize;
					}
					else
					{
						dim = exec->segments[segm_x].file_size - pg_x * pgsize;
					}
					read(fd, unde, dim);
				}
				memset(unde + dim, 0, pgsize - dim);
				}
			if (unde == MAP_FAILED)
			{
				perror("mmap");
				exit(EXIT_FAILURE);
			}
			v[segm_x][pg_x] = 1;

			rc = mprotect(unde, pgsize, exec->segments[segm_x].perm);
			if (rc == -1)
			{
				perror("mmap");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			old_action.sa_sigaction(signum, info, context);
			return;
		}
	}
	else
	{
		old_action.sa_sigaction(signum, info, context);
		return;
	}
}
int so_init_loader(void)
{
	int rc;
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = segv_handler;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGSEGV);
	rc = sigaction(SIGSEGV, &sa, &old_action);
	if (rc < 0)
	{
		perror("sigaction");
		return -1;
	}
	return 0;
}
int so_execute(char *path, char *argv[])
{
	int rc;
	int number_of_pages;
	int i;
	int j;
	int pgsize;
	pgsize = getpagesize();
	fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}
	exec = so_parse_exec(path);
	v = (int **)malloc(exec->segments_no * sizeof(int *));
	for (i = 0; i < exec->segments_no; i++)
	{
		number_of_pages = ceil(exec->segments[i].mem_size / pgsize);
		v[i] = (int *)malloc((number_of_pages + 1) * sizeof(int));
		for (j = 0; j < number_of_pages; j++)
		{
			v[i][j] = 0;
		}
	}
	if (!exec)
		return -1;
	so_start_exec(exec, argv);
	rc = close(fd);
	if (rc < 0)
	{
		perror("close");
		return -1;
	}
	return -1;
}