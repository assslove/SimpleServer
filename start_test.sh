#!/bin/bash

#启动多个程序测试

for i in `seq 0 4`
do
	./test_log $i &
done
