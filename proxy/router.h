#ifndef _ROUTER_H
#define _ROUTER_H

#include <tr1/unordered_map>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

extern "C" {
#include <libnanc/log.h>
#include <libnanc/proto_head.h>
}

#include <libnanc++/singleton.h>
#include <libnanc++/range.h>

#pragma pack(1)

/* @brief 路由类型
 */
enum RooterType {
	ROOTER_TYPE_1X1 = 0,		//单库单表
	ROOTER_TYPE_1X10 = 1,		//单库十表
	ROOTER_TYPE_1X100 = 2,		//单库百表
	ROOTER_TYPE_100X10 = 3,		//百库十表
	ROOTER_TYPE_100X100 = 4		//百库百表
};

/* @brief 最低级的路由 更把某张表路由到指定服务器
 */
typedef struct table {
	mutable int fd;			//服务器连接fd
	uint32_t remote_ip;		//远端ip
	uint16_t remote_port;	//远端port
	uint8_t start;			//起始点
	uint8_t end;			//结束点

	bool operator< (const table& node) const {
		return start < node.start;
	}

	/* @brief 打印结构体信息
	 */
	void print() 
	{
		INFO(0, "table: [start=%u][end=%u][fd=%u][remote_ip=%s][remote_port=%u]", \
				start, end, fd, inet_ntoa(*((struct in_addr*)&remote_ip)), remote_port);
	}

} table_t;

/* @brief 根据库来路由
*/
typedef struct database {
	uint8_t start;
	uint8_t end;
	RangeManager<table_t> tables;

	bool operator< (const database& node) const {
		return start < node.start;
	}

	/* @brief 打印结构体信息
	 */
	void print() {
		INFO(0, "database: [start=%u][end=%u]", start, end);
		tables.print();
	}

	void sort()	{
		tables.sort_less();	
	}

} database_t;

/* @brief 路由功能
 */
typedef struct {
	uint8_t type;					//router类型
	RangeManager<database_t> databases;	//二层嵌套 路由信息
} router_t;

#pragma pack()

typedef std::tr1::unordered_map<uint32_t, router_t> RouterMap; //路由id-路由信息
typedef RouterMap::iterator RouterIter;


/* @breif 路由管理类
*/
class RouterManager : public Singleton<RouterManager> {

	friend class Singleton<RouterManager>;

	private: 
		RouterManager() {}
		~RouterManager() {}

	public:

		/* @brief 加载router.xml
		 */
		int loadRouterXml();

		/* @brief 打印路由信息
		*/
		void printRouter();

		/* @brief 预处理，主要对RangeManager 进行排序 查找时可以用log(n)查找到
		*/
		void preProcess();

		/* @brief 获取指定cmd id 的路由信息
		*/
		const table_t* getRouter(uint16_t cmd, uint32_t id);


		/* @brief 获取table 的下标
		 */
		uint32_t get_table_idx(uint32_t type, uint32_t id) {
			switch (type) {
				case ROOTER_TYPE_1X10:
				case ROOTER_TYPE_100X10:
					return id / 100 % 10;
				case ROOTER_TYPE_1X1:
				case ROOTER_TYPE_1X100:
				case ROOTER_TYPE_100X100:
				default:
					return id / 100 % 100;

			}
		}

		/* @brief 通过router发送消息到指定服务器
		 * @note  0 成功 其它发送失败
		 */
		int sendAcrossRouter(proto_pkg_t *pkg);

		/* @brief 遍历routers, 找到fd
		*/
		const table_t* getRouterByFd(int fd);

	private:
		RouterMap routers;	//所有路由信息
		RouterIter it;		//迭代器
		const router_t* m_router = NULL;	//临时用
		const database_t* m_database = NULL;	//临时用
	};

#define  g_router RouterManager::getInstance()

#endif
