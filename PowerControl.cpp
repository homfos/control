// PowerControl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <boost/function.hpp>
#include "ControlManager.h"
#include "LimitSingleInstance.h"
#include "PollHmiEvent.h"

CLimitSingleInstance singleInstanceObj(TEXT("Global\\{53521A4C-616D-466C-9E38-968A759E3F71}"));
void ActionDataInit();
void PrintBanner();

int _tmain(int argc, _TCHAR* argv[])
{
	bool isStop = false;

	LogInit();
	if (singleInstanceObj.IsAnotherInstanceRunning())
	{
		BOOST_LOG_TRIVIAL(error) << "程控卡片已在运行！";
		return 0;
	}
	if(LoadConfig::load("config.xml") < 0)
	{
		std::cout << "加载配置文件失败！";
		BOOST_LOG_TRIVIAL(error) << "加载配置文件失败！";
		exit(0);
	}
	ActionDataInit();
	PollHmiEvent pollEvent;
	boost::function<void()> workF = boost::bind(&PollHmiEvent::PollEvent,pollEvent);
	boost::function<void()> concelF = boost::bind(&PollHmiEvent::ConcelAction,pollEvent);
	boost::thread workThrd(workF);
	boost::thread concelThrd(concelF);
	do 
	{
		PrintBanner();	
		int a = getchar();
		if (a == 'q')
		{
			isStop = true;
			PollHmiEvent::Stop();
		}
	} while (!isStop);
	workThrd.join();
	concelThrd.join();
	return 0;
}

void ActionDataInit()
{
	DbOperation::connect_db();
	DbOperation::LoadActionInfo();
}

void PrintBanner()
{
	printf("---------------------------------------------\n节点名称：%s\n"
		"欢迎使用上海宝信软件程控卡片程序,版本：%s\n按q键可退出本程序!\n"
		"---------------------------------------------\n", (LoadConfig::hostName).c_str(), BASE_VERSION);
}

