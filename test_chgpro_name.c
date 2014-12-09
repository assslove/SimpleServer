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

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>


int g_argc = 0;
char **g_argv;
char *argv_start;
char *argv_end;
char *env_end;
extern char **environ;

int chg_proc_title(const char *fmt, ...) 
{
	char title[48] = {'\0'};

	va_list ap;
	va_start(ap, fmt);
	vsprintf(title, fmt, ap);
	va_end(ap);

	int len = strlen(title) + 1;
	memcpy(argv_start, title, len);
	argv_start[len-1] = '\0';
	//clear old environ
	memset(argv_start + len, '\0', (argv_end - argv_start) - len);

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

	argv_start = (char *)argv[0];
	argv_end = (char*)(argv[argc-1]) + strlen(argv[argc-1]) + 1;

	int env_size = 0;
	for (i = 0; environ[i]; i++) {
		env_size += strlen(environ[i]) + 1;
	}

	char *env_new = (char *)malloc(sizeof(char) * env_size);;
	if (env_new == NULL) {
		printf("error malloc env");
		return 0;
	}

	for (i = 0; environ[i]; i++) {
		int len = strlen(environ[i]) + 1;
		argv_end += len;

		memcpy(env_new, environ[i], len);
		environ[i] = env_new;
		env_new += len;
	}

	printf("env size=%u", env_size);
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


int print_env() 
{
	int i = 0;
	for (; environ[i]; i++) {
		printf("%s\n", environ[i]);
	}

	printf("-----------------------");
}


int main(int argc, char* argv[]) 
{
	print_env();
	save_args(argc, argv);
	print_args();
	print_env();

	const char *proc_name = "SimpleServer";

	int pid = fork();

	if (pid == 0) {
		char chl_name[30];
		chg_proc_title("%s-%d", proc_name, getpid());

		while (1) {sleep(5);};
		free_args();
		return 0;
	} else if (pid < 0) {
		printf("fork error");
	}

	chg_proc_title("%s", proc_name);
	while (1) {sleep(5);};
	free_args();
	return 0;
}
