/*
 * =====================================================================================
 *
 *       Filename:  test_chgpro_name.c
 *
 *    Description:	测试修复进程名字 
 *
 *        Version:  1.0
 *        Created:  12/09/2014 11:10:15 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	genry , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>


int g_argc = 0;
char **g_argv;

int chg_proc_title(int argc, char* argv[], const char *title) 
{
	memcpy(argv[0], title, sizeof(title));
	return 0;
}


int save_args(int argc, char* argv[])
{
	g_argc = argc;
	g_argv = (char **)malloc(sizeof(char*) * argc);

	int i = 0;
	for (; i < argc; i++) {
		g_argv[i] = strdup(argv[i]);
	}
	return 0;
}

int free_args() 
{
	int i = 0;
	for (; i < g_argc; i++) {
		free(g_argv[i]);
	}
	free(g_argv);
}

void print_args()
{
	int i = 0;
	for (; i < g_argc; i++) {
		printf("%s\n", (g_argv[i]));
	}
}

extern char **environ;

int print_env() 
{
	int i = 0;
	for (; environ[i]; i++) {
		printf("%s\n", environ[i]);
	}
}


int main(int argc, char* argv[]) 
{
	save_args(argc, argv);
	print_args();
	print_env();

	const char *proc_name = "Simp";

	int pid = fork();

	if (pid == 0) {
		char chl_name[30];
		sprintf(chl_name, "%s-%d", proc_name, getpid());
		chg_proc_title(argc, argv, chl_name);

		while (1) {sleep(5);};
		free_args();
		return 0;
	} else if (pid < 0) {
		printf("fork error");
	}

	chg_proc_title(argc, argv, proc_name);
	while (1) {sleep(5);};
	free_args();
	return 0;
}
