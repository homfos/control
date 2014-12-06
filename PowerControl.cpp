// PowerControl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "ControlManager.h"
#include "LimitSingleInstance.h"

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
	do 
	{
		PrintBanner();	
		std::string s;
		std::cin >>s;
		if (s == "q")
		{
			isStop = true;
		}
	} while (!isStop);
	ControlManager cm;
	cm.ProConditionCheck(DbOperation::allLineOff);
	cm.Run("1");
	return 0;
}

void ActionDataInit()
{
	DbOperation::connect_db();
	DbOperation::LoadActionInfo();
}

void PrintBanner()
{
	std::cout << "---------------------------------------------" << std::endl;
	std::cout << "��ӭʹ�ñ�������̿ؿ�Ƭ����q�����˳�������" << std::endl;
	std::cout << "---------------------------------------------"<<std::endl;
}

