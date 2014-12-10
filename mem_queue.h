#ifndef MEM_QUEUE_H_
#define MEM_QUEUE_H_

#include <stdint.h>

/* @brief 内存块类型
 */
enum MEM_BLOCK_TYPE {
	BLK_DATA	= 0,
	BLK_ALIGN	= 1, 
	BLK_CLOSE	= 2,
	BLK_OPEN	= 3,
};

/* @brief 共享内存首部,用于记录队列信息
 */
typedef struct mem_head {
	volatile int head;		//头部	
	volatile int tail;		//尾部
	volatile int blk_cnt;	//总共块数 
} __attribute__((packed)) mem_head_t;

/* @brief 内存队列
 */
typedef struct mem_queue {
	mem_head_t *ptr;
	int len;				//总长
	int pipefd[2];			//通知
	int blk_head_len;		//
	int mem_head_len;		
} __attribute__((packed)) mem_queue_t;

/* @brief 内存块信息
 */
typedef struct mem_block {
	int id;					//server id
	int fd;					//fd
	uint8_t type;			//类型
	int len;				//长度
	char *data;			//数据
} __attribute__((packed)) mem_block_t;


/* @brief b放在q的尾部
 * @param q 共享内存队列
 * @param b 队列块
 */
int mq_push(mem_queue_t *q, mem_block_t *b);

/* @brief q从尾部出来
 */
void mq_pop(mem_queue_t *q);

/* @brief get mem_block_t
 */
mem_block_t *mq_get(mem_queue_t *q);

/* @biref 创建共享队列
 */
int mq_init(mem_queue_t *q, int size);

/* @brief 释放共享队列
 */
int mq_fini(mem_queue_t *q, int size);

/* @brief 尾块
 */
inline mem_block_t* blk_tail(mem_queue_t *q);

/* @brief 头部块
 */
inline mem_block_t* blk_head(mem_queue_t *q);

/* @brief 打印调试信息
 */
void mq_display(mem_queue_t *q);

#endif
