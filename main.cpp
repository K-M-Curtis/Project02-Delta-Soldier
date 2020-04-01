#include <iostream>
#include <unistd.h>
#include "zinx.h"
#include "ZinxTimer.h"
#include "ZinxTCP.h"
#include "GameChannel.h"
#include <fcntl.h>

using namespace std;

class ShutdownServer :public TimerOutProc
{
public:
	virtual void Proc()		//��ʱ��ʱ���ȥ���õĽӿ�
	{
		//ÿ30����һ�µ�ǰ���������û�����,���Զ��ر�
		auto allPlayers = ZinxKernel::Zinx_GetAllRole();
		if(allPlayers.size() == 0)
		{
			ZinxKernel::Zinx_Exit();
		}
	}
	virtual int GetTimerSec()	//��ȡ��ǰ����ĳ�ʱ�����һ�봥��һ�ξͷ���1
	{
		return 30;
	}
};

//�ػ�����+���̼��
void Daemon()
{
	//����־�ļ�
	int logFd = open("./game.log", O_WRONLY | O_CREAT, 0664);
	if (logFd < 0)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}
	//����׼����ض�����־�ļ�
	if (dup2(logFd, STDOUT_FILENO) < 0)
	{
		perror("dup2");
		exit(EXIT_FAILURE);
	}
	if (dup2(logFd, STDERR_FILENO) < 0)
	{
		perror("dup2");
		exit(EXIT_FAILURE);
	}
	else
	{
		close(logFd);
	}

	//2 fork  ʵ���ػ�����
	int pid = fork();
	if (pid > 0)
	{
		exit(EXIT_SUCCESS);
	}
	else if (pid == 0)
	{
		if (setsid() < 0)
		{
			perror("setsid");
			exit(EXIT_FAILURE);
		}
		while (true)
		{
			//3 fork ʵ�ֽ��̼��
			pid = fork();
			if (pid > 0)
			{
				int status;
				wait(&status);
				if (status == 0)
				{
					exit(EXIT_SUCCESS);
				}
				else
				{
					//����õ����ֵ��Ϊ0
					//��������fork
					continue;
				}
			}
			else if (pid == 0)
			{
				//�ӽ���:
				//ִ����Ϸҵ���߼�
				break;
			}
			else
			{
				perror("fork");
				exit(EXIT_FAILURE);
			}
		}
	}
	else
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
}


int main(int argc, char **argv)
{
	//1 �ж�main ���� �� daemon
	if (argc == 2)
	{
		if (string("daemon") == argv[1])
		{
			Daemon();
		}
	}

	//ע�ᶨʱ����
	ShutdownServer *poTimeout = new ShutdownServer;
	ZinxTimerDeliver::GetInstance().RegisterProcObject(*poTimeout);


	//����TCP listen ͨ��,���������˿�
	ZinxTCPListen *chListener = new ZinxTCPListen(8899, new GameChannelFactory);

	ZinxKernel::ZinxKernelInit();

	ZinxTimer *poTimer = new ZinxTimer;
	ZinxKernel::Zinx_Add_Channel(*poTimer);
	ZinxKernel::Zinx_Add_Channel(*chListener);

	ZinxKernel::Zinx_Run();

	ZinxKernel::ZinxKernelFini();

	return 0;
}
