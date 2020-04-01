#!/bin/bash
#创建容器
docker run -d -P --rm mygame:V0.1 2>&1 > /dev/null
#提取映射的端口号
docker ps -n 1 | grep 8899 | awk -F "->" '{print $1}' | awk -F : '{print $NF}'
