#ifndef _ROUTER_H
#define _ROUTER_H

#include <tr1/unordered_map>
#include <stdint.h>

#include <libnanc++/singleton.h>
#include <libnanc++/range.h>

#pragma pack(1)

/* @brief 路由类型
 */
enum ROOTER_TYPE {
	ROOTER_TYPE_1X1 = 0,		//单库单表
	ROOTER_TYPE_1X10 = 1,		//单库十表
	ROOTER_TYPE_1X100 = 2,		//单库百表
	ROOTER_TYPE_100X10 = 3,		//百库十表
	ROOTER_TYPE_100X100 = 4		//百库百表
};

/* @brief 最低级的路由 更把某张表路由到指定服务器
 */
typedef struct table {
	int fd;					//服务器连接fd
	uint32_t remote_ip;		//远端ip
	uint16_t remote_port;	//远端port
	uint8_t start;			//起始点
	uint8_t end;			//结束点

	table& operator< (const table& node) const {
		return start < node.start;
	}

} table_t;

/* @brief 根据库来路由
 */
typedef struct database {
	uint8_t start;
	uint8_t end;
	RangeMgr<table_t> tables;

	database& operator< (const database& node) const {
		return start < node.start;
	}

} database_t;

typedef struct {
	int8_t type;					//router类型
	RangeMgr<database_t> databases;	//二层嵌套 路由信息
} router_t;

typedef std::tr1::unordered_map<uint32_t, router_t> RouterMap; //路由id-路由信息
typedef RouterMap::iterator RouterIter;

#pragma pack()

/* @breif 路由管理类
 */
class RouterManager : public Singleton<RouterManager> {

	friend class Singleton<RouterManager>;
	private: 
		RouterManager() {}
		~RouterManager() {}

	public:

		/* @brief 打印路由信息
		 */
		void printRouter();

		/* @brief 预处理，主要对RangeMgr 进行排序 查找时可以用log(n)查找到
		 */
		void preProcess();
		
		/* @brief 获取指定cmd id 的路由信息
		 */
		const table_t* getRouter(uint16_t cmd, uint32_t id);

		/* @brief 能过router发送消息到指定服务器
		 */
		void sendAcrossRouter(proto_pkg_t *pkg);

		/* @brief 遍历routers, 找到fd
		 */
		const table_t* getRouterByFd(int fd);

	private:
		RouterMap routers; //所有路由信息
};

#define router_mgr RouterManager::getInstance();

#endif
