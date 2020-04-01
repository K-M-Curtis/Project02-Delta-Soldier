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
#include <cstring>

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
    string opt;
    string roomnum;
};

//函数作用是读取输入的字符串，然后做反序列化
Request parseJson(string input)
{
    CJsonObject obj(input);
    string userName;
    string passwd;
    string opt;
    string roomnum;
    obj.Get("userName",userName);
    obj.Get("passwd",passwd);
    obj.Get("opt",opt);

    //如果是跟房，那么就还会存在 房间号
    if(opt=="follow")
    {
        obj.Get("roomnum",roomnum);
    }
    return {
        userName,
        passwd,
        opt,
        roomnum
    };
};

string DoLogin(Request req)
{
    string ret= "succ";
    sql::Connection *con =nullptr;
    sql::PreparedStatement *stmt = nullptr;
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

        //3 创建一个语句
        //stmt=con->createStatement();
        //
        ////构造sql语句,stringstream
        ////C 语言构造一个字符串使用sprintf ,C++里边使用stringstream
        //stringstream ss;
        ////插入数据之前，先确定有没有同名用户存在，如果有就直接返回用户已经存在
        //ss<<"select * from user where name = '"<<req.userName<<"' and passwd = '"<<req.passwd<<"'";
        //res = stmt->executeQuery(ss.str());
        //if(!res->next())
        //{
        //    //如果res返回的结果集为空，就表示不存在这个用户和密码，可以认为登录失败
        //    ret="fail";
        //    goto END;
        //}

        //创建预处理语句,要填的参数以？代替
        string sql = "select * from user where name = ? and passwd = ?";
        stmt=con->prepareStatement(sql);
        stmt->setString(1,req.userName);
        stmt->setString(2,req.passwd);
        
        res = stmt->executeQuery();
        if(!res->next())
        {
            //如果res返回的结果集为空，就表示不存在这个用户和密码，可以认为登录失败
            ret="fail";
            goto END;
        }

    }
    catch(const exception &e)
    {
        printf("err:%s\n",e.what());
        ret = "fail";
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

int create_room()
{
    int ret = -1;
    FILE *cmd = popen("./room_create.sh","r");
    if(cmd!=nullptr)
    {
        char buf[128];
        memset(buf,0,sizeof(buf));
        int readCount = fread(buf,1,sizeof(buf),cmd);
        ret = atoi(buf);
    }
    return ret;
}
int room_check(int roomnum)
{
    int ret = -1;
    string strCmd = string("./room_check.sh") + " " + to_string(roomnum);
    FILE *cmd = popen(strCmd.c_str(),"r");
    if(cmd!=nullptr)
    {
        char buf[128];
        memset(buf,0,sizeof(buf));
        int readCount = fread(buf,1,sizeof(buf),cmd);
        string result(buf);
        if(result=="yes")
        {
            ret = 0;
        }
        else
        {
            ret = -1;
        }
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
            delete [] buf;

            auto req = parseJson(input);
            //printf("userName:%s,passwd:%s\n",req.userName.c_str(),req.passwd.c_str());
            //2 完成登录业务
            string ret = DoLogin(req);
            string opt_ret="fail";
            int roomnum = atoi(req.roomnum.c_str());
            if(ret=="succ")
            {
                //根据opt来实现建房和跟房
                if(req.opt=="create")
                {
                    //实现建房
                    roomnum = create_room();
                    if(roomnum>=0)
                    {
                        //房间创建成功
                        opt_ret = "succ";
                    }
                }
                else if(req.opt=="follow")
                {
                    //实现跟房
                    if(room_check(roomnum)==0)
                    {
                        //跟房成功
                        opt_ret = "succ";
                    }
                }
            }

            //3 完成返回
            CJsonObject obj;
            obj.Add("ret",ret);
            obj.Add("opt_ret",opt_ret);
            obj.Add("roomnum",roomnum);
            printf("%s",obj.ToString().c_str());
        }

    } /* while */

    return 0;
}
