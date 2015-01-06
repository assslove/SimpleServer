/*
 * =====================================================================================
 *
 *       Filename:  test_proto.c
 *
 *    Description:  对google protobuf的测试
 *
 *        Version:  1.0
 *        Created:  2015年01月06日 14时33分13秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	houbin , houbin-12@163.com
 *   Organization:  Houbin, Inc. ShangHai CN. All rights reserved.
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <assert.h>

#include <sys/time.h>

#include "proto/lib/simple.pb.h"

typedef struct proto_pkg {
	int len;
	int id;
	int seq;
	int cmd;
	int ret;
	uint8_t data[];
} __attribute__((packed)) proto_pkg_t;

/* @brief 测试加密与解密的时间
 */
int main(int argc, char *argv[])
{
	srand(time(NULL));
	int i = 0;
	std::string rbuf;
	std::string sbuf;
	for (; i < 10000; ++i) {
		int len = rand() % 1000 + 1;
		int nozip_len = (len + 1) * 4;
		timeval start;
		gettimeofday(&start, NULL);
		//用于消息的加密与解密
		test::Simple sim;
		for (int j = 0; j < len; ++j) {
			sim.add_id(rand() % 10000);
		}

		assert(sim.id_size() == len);

		sim.SerializeToString(&sbuf);
		rbuf = sbuf;

		assert(rbuf.size() == sbuf.size());

		test::Simple tmp;	
		tmp.ParseFromString(rbuf);

		timeval end;
		gettimeofday(&end, NULL);

		int zip_len = sbuf.size();

		std::cout << "nozip_len = " << nozip_len 
			<<"\tzip_len=" << zip_len 
			<<"\tusedsec="  << (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec) << "\tlen=" << tmp.id_size() << std::endl;
		sleep(1);
	}

	//delete
	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}
