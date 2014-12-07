#include "stdafx.h"
#include "LoadConfig.h"
#include "boost/filesystem/path.hpp" 
#include "boost/filesystem/operations.hpp"  
#include <fstream>
#include <iostream>
#include <sstream>

std::string LoadConfig::hostName ="localhost";
std::string LoadConfig:: dbHost ="";
std::string LoadConfig::userName="tom";
std::string LoadConfig:: passWord="tom";
std::string LoadConfig:: dbName="Local_SID";
std::string LoadConfig:: scadaName="";
std::string LoadConfig:: scadaType="fix";
std::string LoadConfig::hangOnTag1 ="_901";
std::string LoadConfig::hangOnTag2 ="_902";
std::string LoadConfig::hangOnTag3 ="_903";
bool LoadConfig::parallelFlag=false;
int LoadConfig::feedBackTimeout = 7;
int LoadConfig::commandTimeout = 12;
std::string LoadConfig::dbType = "oracle";
int LoadConfig::excuteOn = 2;
int LoadConfig::excuteOff = 1;
int LoadConfig::chooseCommandOffset = 129;
int LoadConfig::chooseFeedBackOffset = 7;
int LoadConfig::executeFeedBackOffset = 7;
int LoadConfig::twoPointOffset = 1;
std::string LoadConfig::operationArea = "_107";
std::string LoadConfig::remoteControl = "_205";
std::string LoadConfig::remoteFeedBack = "_105";
std::vector<std::vector<std::string>> LoadConfig::allLineOn; 
std::vector<std::vector<std::string>> LoadConfig::interLockDetail;
vector<map<pair<string, string>, string>> LoadConfig::tOrf;

using namespace std;

void LoadConfig::load(const string fileName, vector<vector<string>> &actionInfo)
{
	ifstream infile(fileName);

	while (infile)
	{
		string line;
		if (!getline( infile, line )) break;

		istringstream ss( line );
		vector <string> record;

		while (ss)
		{
			string s;
			if (!getline( ss, s, ',' )) break;
			record.push_back( s );
		}

		actionInfo.push_back( record );
	}
	if (!infile.eof())
	{
		BOOST_LOG_TRIVIAL(error) << "加载动作信息失败！";
	}
}

void LoadConfig::loadtorf(const std::string fileName, vector<map<pair<string, string>, string>> &torf)
{
	ifstream infile(fileName);

	while (infile)
	{
		string line;
		if (!getline( infile, line )) break;

		istringstream ss( line );
		vector <string> record;

		while (ss)
		{
			string s;
			if (!getline( ss, s, ',' )) break;
			record.push_back( s );
		}
		pair<string, string> p(record[0], record[1]);
		map<pair<string, string>,string> m;
		m[p] = record[2];
		tOrf.push_back(m);
	}
	if (!infile.eof())
	{
		BOOST_LOG_TRIVIAL(error) << "加载闭锁逻辑结果信息失败！";
	}
	cout << "加载闭锁逻辑结果信息成功！";
}

int LoadConfig::loadCardInfo()
{
	/*load("test.txt", allLineOn);
	load("interlocak_detail.txt", interLockDetail);
	loadtorf("torf.txt", tOrf);*/
	return 1;
}

int LoadConfig::load(const std::string &fileName)
{
	std::string fullpath = boost::filesystem::initial_path<boost::filesystem::path>().string();

	using boost::property_tree::ptree;
	ptree pt;

	try
	{
		read_xml(fullpath+"\\"+fileName, pt);
	}
	catch (boost::exception const&)
	{
		return -1;
	}

	hostName = pt.get<std::string>("configuration.context-param.hostname");
	scadaType = pt.get<std::string>("configuration.context-param.scada-type");
	scadaName =pt.get<std::string>("configuration.context-param.scada-name");
	parallelFlag= pt.get<bool>("configuration.context-param.parallel-enable");
	feedBackTimeout = pt.get<int>("configuration.context-param.feedback-timeout");
	commandTimeout = pt.get<int>("configuration.context-param.command-timeout");
	hangOnTag1 = pt.get<std::string>("configuration.context-param.HANG-ON-TAG1");
	hangOnTag2 = pt.get<std::string>("configuration.context-param.HANG-ON-TAG2");
	hangOnTag3 = pt.get<std::string>("configuration.context-param.HANG-ON-TAG3");
	excuteOn =  pt.get<int>("configuration.context-param.EXECUTE_ON");
	excuteOff =  pt.get<int>("configuration.context-param.EXECUTE_OFF");
	twoPointOffset = pt.get<int>("configuration.context-param.TWO_POINT_OFFSET");
	chooseCommandOffset=  pt.get<int>("configuration.context-param.CHOOSE_COMMAND_OFFSET");
	chooseFeedBackOffset=  pt.get<int>("configuration.context-param.CHOOSE_FEEDBACK_OFFSET");
	executeFeedBackOffset=  pt.get<int>("configuration.context-param.EXECUTE_FEEDBACK_OFFSET");
	operationArea = pt.get<std::string>("configuration.context-param.OPERATION_AREA");
	remoteControl = pt.get<std::string>("configuration.context-param.REMOTE_CONTROL");
	remoteFeedBack = pt.get<std::string>("configuration.context-param.REMOTE_FEEDBACK");
	dbHost = pt.get<std::string>("configuration.db-config.ipaddress");
	userName = pt.get<std::string>("configuration.db-config.username");
	passWord = pt.get<std::string>("configuration.db-config.password");
	dbName = pt.get<std::string>("configuration.db-config.dbname");
	dbType = pt.get<std::string>("configuration.db-config.dbtype");

	BOOST_LOG_TRIVIAL(info) << "加载配置文件成功！";
	return 1;
}