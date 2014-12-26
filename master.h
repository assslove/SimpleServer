#ifndef MASTER_H_
#define MASTER_H_

struct mem_block;

/* @brief master init
 * @note 
 */
int master_init();

/* @brief 
 */
int master_listen(int i);

int master_mq_create(int i); 

/* @brief 
 */
int master_dispatch();

/* @brief 
 */
int master_fini();

/* @brief 增加fd到epinfo
 */
int add_fdinfo_to_epinfo(int fd, int idx, int type, int ip, uint16_t port);

/*  @brief 处理cli
 */
int handle_cli(int fd);

/* @brief  handle read
 */
int handle_read(int fd);

/* @brief 处理管道的读取
 * @note 直接就可以
 */
int handle_pipe(int fd);

/* @brief 处理往客户端发送消息
 */
void handle_mq_send();

/* @brief 发送消息块
 */
int do_blk_send(struct mem_block *blk);

/* @brief 处理消息的发送
 */
int do_fd_send(int fd, void *data, int len);

/* @brief 处理消息的写入
 */
int do_fd_write(int fd);

/* @brief 处理可读队列
 */
int handle_readlist();

/* @brief 处理待关闭对列
 */
int handle_closelist();

/* @brif 将fd增加 可读队列里面
 */
void do_add_to_readlist(int fd);

/* @brief fd从可读队列删除
 */
void do_del_from_readlist(int fd);

/*  @brief 将fd增加到待关闭队列里面
*/
void do_add_to_closelist(int fd);

/* @brief 将fd从关闭队列里面删除
 */
void do_del_from_closelist(int fd);

/* @brief 执行fd关闭
 */
int do_fd_close(int fd);

/* @brief 执行fd打开
 */
int do_fd_open(int fd);

/* @brief 初始化配置信息
 */
int init_setting();

/* @brief master为work进程初始化 用于通信
 */
int master_init_for_work(int id);

#endif
