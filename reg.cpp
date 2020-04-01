/*
 * echo.c --
 *
 *	Produce a page containing all FastCGI inputs
 *
 *
 * Copyright (c) 1996 Open Market, Inc.
 *
 * See the file "LICENSE.TERMS" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#include "fcgi_config.h"
#include <cstdio>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef _WIN32
#include <process.h>
#else
extern char **environ;
#endif
#include "fcgi_stdio.h"
#include "CJsonObject-1.0/CJsonObject.hpp"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <sstream>

#define DB_URI "tcp://127.0.0.1"
#define DB_USER "root"
#define DB_PASS "123456"
#define DB_DB "game"

using namespace neb;

using namespace std;

//反序列化的结果，是一个request 对象
struct Request
{
    string userName;
    string passwd;
};

//函数作用是读取输入的字符串，然后做反序列化
Request parseJson(string input)
{
    CJsonObject obj(input);
    string userName;
    string passwd;
    obj.Get("userName",userName);
    obj.Get("passwd",passwd);
    return {
        userName, passwd
    };
};

string DoRegister(Request req)
{
    string ret= "succ";
    sql::Connection *con =nullptr;
    sql::Statement *stmt = nullptr;
    sql::ResultSet *res = nullptr;
    //实现入库的操作
    try
    {
        //1获取mysql驱动
        sql::Driver * driver = ::get_driver_instance();
        //2建立连接
        con=driver->connect(DB_URI, DB_USER, DB_PASS);
        //选择数据库使用game
        con->setSchema(DB_DB);
        //开启事务，将自动提交关闭掉
        con->setAutoCommit(false);

        //3 创建一个语句
        stmt=con->createStatement();
        
        //构造sql语句,stringstream
        //C 语言构造一个字符串使用sprintf ,C++里边使用stringstream
        stringstream ss;
        //插入数据之前，先确定有没有同名用户存在，如果有就直接返回用户已经存在
        ss<<"select * from user where name = '"<<req.userName<<"'";
        res = stmt->executeQuery(ss.str());
        if(res->next())
        {
            //如果res返回的结果集不为空，就表示有同名的用户
            ret = "exists";
            goto END;
        }
        ss = stringstream();
        ss<<"insert into user(name,passwd) values('"<<req.userName<<"','"
            <<req.passwd<<"')";
        string sql = ss.str();

        int affactRow = stmt->executeUpdate(sql);
        //返回是插入成功的行数
        if(affactRow!=1)
        {
            //报错
            ret = "fail";
            con->rollback();
        }
        else
        {
            con->commit();
        }
    }
    catch(const exception &e)
    {
        printf("err:%s\n",e.what());
        ret = "fail";
        con->rollback();
    }

END:
    //垃圾回收
    if(res)
    {
        delete res;
    }
    if(stmt)
    {
        delete stmt;
    }
    if(con)
    {
        delete con;
    }
    return ret;
}
int main ()
{
    //将原来的进程的环境变量指针备份
    char **initialEnv = environ;
    int count = 0;

    //等待fastCGI请求到达，阻塞点
    while (FCGI_Accept() >= 0) {
        //获取环境变量CONTENT_LENGTH 的值
        char *contentLength = getenv("CONTENT_LENGTH");
        int len;

        //响应头的输出
        printf("Content-type: application/json\r\n\r\n");

        if (contentLength != NULL) {
            len = strtol(contentLength, NULL, 10);
        }
        else {
            len = 0;
        }

        if (len <= 0) {
            CJsonObject obj;
            obj.Add("ret","fail");
            printf("%s",obj.ToString().c_str());
        }
        else {
            //读取标准输入
            int i, ch;
            char *buf = new char[len];
            //printf("Standard input:<br>\n<pre>\n");
            for (i = 0; i < len; i++) {
                buf[i] = getchar();
            }
            //1 json解析
            //小坑，buf是客户端传输过来的数据，不包括最后一个\0
            //先做一下转换
            string input(buf,len);
            auto req = parseJson(input);
            //printf("userName:%s,passwd:%s\n",req.userName.c_str(),req.passwd.c_str());
            //2 完成注册业务
            string ret = DoRegister(req);

            //3 完成返回
            CJsonObject obj;
            obj.Add("ret",ret);
            printf("%s",obj.ToString().c_str());
            delete [] buf;
        }

    } /* while */

    return 0;
}
