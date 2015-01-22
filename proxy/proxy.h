#ifndef _PROXY_H_
#define _PROXY_H_


#include <libnanc++/singleton.h>
#include <tr1/unordered_map>
#include <stdint.h>


#ifdef  ONE_SERV
#define UserId uint32_t
#elif
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
		Proxy() {}
		~Proxy() {}
	public:

		/* @brief  获取fd
		 */
		int get(UserId id) {
			Iter it = fds.find(id);	
			if (it != fds.end()) {
				return it->second;
			}

			return 0;
		}

		/* @brief 缓存用户id fd
		 */
		void save(UserId id, int fd) {
			fds.insert(std::pair<Userid, int>(id, fd));
		}

		/* @brief 执行路由
		 */
		void do_router();

		/* @brief 处理发送proxy的请求包
		 */
		int handle_request(int fd, void *msg, int len);

		/* @brief 处理回调
		 */
		int handle_response(int fd, void *msg, int len);

	private:
		UserFdMap fds; //保存回调时间可以发给指定的用户
	
};

#define proxy Proxy::getInstance()
#endif 
