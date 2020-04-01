#指定一下当前cmake文件的最低版本号
cmake_minimum_required(VERSION 3.2)
add_executable(reg ./reg.cpp ./CJsonObject-1.0/cJSON.c ./CJsonObject-1.0/CJsonObject.cpp)
add_executable(login ./login.cpp ./CJsonObject-1.0/cJSON.c ./CJsonObject-1.0/CJsonObject.cpp)
target_link_libraries(reg mysqlcppconn fcgi)
target_link_libraries(login mysqlcppconn fcgi)
