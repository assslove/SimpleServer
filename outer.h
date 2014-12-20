#ifndef OUT_H_
#define OUT_H_

/* @brief 提供对业务逻辑的接口
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

typedef struct ServInterface {
	void*   data_handle; 
	void*   handle;		

	void	(*handle_timers)();
	/*!
	  * Called to process packages from clients. Called once for each package. \n
	  * Return non-zero if you want to close the client connection from which the `pkg` is sent,
	  * otherwise returns 0. If non-zero is returned, `on_client_conn_closed` will be called too.
	  */
	int		(*proc_msg_from_cli)(void* msg, int len, fdsess_t* fdsess);
	/*! Called to process packages from servers that the child connects to. Called once for each package. */
	void	(*proc_msg_from_serv)(int fd, void* msg, int len);
	/*! Called each time when a client close a connection, or when `proc_pkg_from_client` returns -1. */
	void	(*on_cli_closed)(int fd);
	/*! Called each time on close of the FDs opened by the child. */
	void	(*on_serv_closed)(int fd);

	/* The following 3 interfaces are called both by the parent and child process */

	/*!
	  * Called only once at server startup by both the parent and child process. Optional interface.\n
	  * `isparent == 1` indicates this interface is called by the parent;
	  * `isparent == 0` indicates this interface is called by the child. \n
	  * You should initialize your service program (allocate memory, create objects, etc) here. \n
	  * You must return 0 on success, -1 otherwise.
	  */
	int 	(*serv_init)(int ismaster);
	/*!
	  * Called only once at server stop by both the parent and child process. Optional interface.\n
	  * `isparent == 1` indicates this interface is called by the parent;
	  * `isparent == 0` indicates this interface is called by the child. \n
	  * You should finalize your service program (release memory, destroy objects, etc) here. \n
	  * You must return 0 if you have finished finalizing the service, -1 otherwise.
	  */
	int 	(*serv_fini)(int ismaster);
	/*!
	  * This interface will be called both by the parent and child process.\n
	  * `isparent == 1` indicates this interface is called by the parent;
	  * `isparent == 0` indicates this interface is called by the child. \n
	  * You must return 0 if you cannot yet determine the length of the incoming package,
	  * return -1 if you find that the incoming package is invalid and AsyncServ will close the connection,
	  * otherwise, return the length of the incoming package. Note, the package should be no larger than 8192 bytes.
	  */
	int		(*get_msg_len)(int fd, const void *data, int len, int ismaster);
} serv_if_t;

extern serv_if_t so;

int  reg_data_so(const char* name);
int  reg_so(const char* name, int flag);
void unreg_data_so();
void unreg_so();

#ifdef __cplusplus
} // end of extern "C"
#endif

#endif // OUTER_H_
