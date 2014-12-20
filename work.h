#ifndef WORK_H_
#define WORK_H_


/* @brief
 */
int work_init(int i);

/* @brief 
 */
int work_dispatch(int i);

/* @brief
 */
int work_fini(int i);

/* @brief 读取共享队列读入
 */
int handle_mq_recv();

#endif
