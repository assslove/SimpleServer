#ifndef _PROXY_H_
#define _PROXY_H_

#include <libnanc++/singleton.h>
#include <tr1/unordered_map>
#include <stdint.h>

#include "router.h"

#ifdef SERV_ONE 
#define UserId uint32_t
#else
#define UserId uint64_t
#endif

/* @brief 代理服务器实现逻辑
 * @note 本服务器只适用于单角色多服或者单服
 */
class Proxy : public Singleton<Proxy> {

	friend class Singleton<Proxy>;

	typedef std::tr1::unordered_map<UserId, int> UserFdMap; //用户->客户端fd映射
	typedef UserFdMap::iterator Iter;

	private:
		Proxy() : temp_user_id(0), m_tempfd(0), m_it(NULL) {fds.clear();}
		~Proxy() {}
	public:

		/* @brief  获取fd
		 */
		int get(UserId id) {
			m_it = fds.find(id);	
			if (m_it != fds.end()) {
				return m_it->second;
			}

			return 0;
		}

		/* @brief 缓存用户id fd
		 */
		void save(UserId id, int fd) {
			fds.insert(std::pair<UserId, int>(id, fd));
		}

		/* @brief 删除用户id
		 */
		void del(UserId id) {
			fds.erase(id);
		}

		/* @brief 执行路由
		 */
		int doRouter(proto_pkg_t *pkg) {
			return g_router.sendAcrossRouter(pkg);
		}

		/* @brief 处理发送proxy的请求包
		 */
		void handleRequest(int fd, proto_pkg_t *pkg);

		/* @brief 处理回调
		 */
		void  handleResponse(const proto_pkg_t *pkg);

		/* @brief 处理服务器fd断开
		 * @note  将相关fd置为-1
		 */
		void handleServClosed(int fd);

		/* @brief 处理客户端fd断开
		 *  将fd相关用户都删除
		 */
		void handleCliClosed(int fd);

	private:
		UserFdMap fds; //保存回调时间可以发给指定的用户
		UserId	  temp_user_id; //临时用用户id
		int m_tempfd; //临时fd
		Iter m_it;	  //
};

#define g_proxy Proxy::getInstance()

#endif 
