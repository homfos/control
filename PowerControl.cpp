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
		BOOST_LOG_TRIVIAL(error) << "�̿ؿ�Ƭ�������У�";
		return 0;
	}
	if(LoadConfig::load("config.xml") < 0)
	{
		std::cout << "���������ļ�ʧ�ܣ�";
		BOOST_LOG_TRIVIAL(error) << "���������ļ�ʧ�ܣ�";
		exit(0);
	}
	ActionDataInit();
	PollHmiEvent pollEvent;
	boost::function<void()> f = boost::bind(&PollHmiEvent::PollEvent,pollEvent);
	boost::thread thrd( f );
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
	thrd.join();
	return 0;
}

void ActionDataInit()
{
	DbOperation::connect_db();
	DbOperation::LoadActionInfo();
}

void PrintBanner()
{
	printf("---------------------------------------------\n"
		"��ӭʹ�ñ�������̿ؿ�Ƭ����q�����˳�������\n"
		"---------------------------------------------\n");
}

