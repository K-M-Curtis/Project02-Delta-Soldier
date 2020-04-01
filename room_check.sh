#!/bin/bash
# docker查询容器中有没有 基于game:v1.0的容器 ，然后根据端口号来grep 
docker ps -f ancestor=mygame:V0.1 | grep "$1" 2>&1 >/dev/null
#这个if只是判断刚才执行的指令有没有成功，成功就返回yes
if [ 0 -eq $? ]
then
    echo -n "yes"       #默认 后面带\n
else
    echo -n "no"
fi
