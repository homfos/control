#include "stdafx.h"
#include "DbOperation.h"

SAConnection DbOperation::con;
map<int, vector<vector<string>>> DbOperation::actionsInfo;
map<int, vector<string>> DbOperation::interLockDetail;
map<pair<int, int>, string> DbOperation::tOrf;

DbOperation::DbOperation() 
{
}

DbOperation::~DbOperation()
{
}

void DbOperation::LoadActoinInfo(map<int, vector <vector <string> >> &actionsContent, int cardId)
{
	std::string sqlStr;
	sqlStr = "select * from T_SCADA_ACTIONCOLL where cardid = " + std::to_string(static_cast<long long>(cardId));
//	sqlStr.append(" order by ACTIONSEQID");
	vector <vector <string>> actions;
	try
	{
		DbOperation::connect_db();
		SACommand cmd(&(DbOperation::get_connection()), sqlStr.c_str());
		cmd.Execute();
		while (cmd.FetchNext()) 
		{
			vector<string> action;
			action.push_back(std::to_string(static_cast<long long>(cmd.Field("ACTIONID").asLong())));
			action.push_back(std::string(cmd.Field("ACTIONNAME").asString()));
			action.push_back(std::string(cmd.Field("DEVACTTAG").asString()));
			action.push_back(std::string(cmd.Field("DEVADDTAG").asString()));
			action.push_back(std::string(cmd.Field("DEVLOCTAG").asString()));
			action.push_back(std::to_string(static_cast<long long>(cmd.Field("ACT_CLASS").asLong())));
			action.push_back(std::to_string(static_cast<long long>(cmd.Field("ACT_AVALUE").asLong())));
			action.push_back(std::to_string(static_cast<long long>(cmd.Field("ACTIONGROUP").asLong())));
			action.push_back(std::to_string(static_cast<long long>(cmd.Field("ACTIONSEQID").asLong())));
			action.push_back(std::to_string(static_cast<long long>(cmd.Field("INTERLOCK_ID").asLong())));
			action.push_back(std::string(cmd.Field("STA_ALIAS").asString()));
			action.push_back(std::string(cmd.Field("DEV_CLASS_ID").asString()));
			actions.push_back(action);
		}
		actionsContent[cardId] = actions; 
		BOOST_LOG_TRIVIAL(info) << "加载卡片: " + std::to_string(static_cast<long long>(cardId));
	}
	catch (SAException& e)
	{
		BOOST_LOG_TRIVIAL(fatal) << e.ErrText();
		BOOST_LOG_TRIVIAL(info) << "加载卡片动作序列失败！";
		return;
	}
}

void DbOperation::LoadActionInfo()
{
	//LoadActoinInfo(actionsInfo, 212);
	LoadActoinInfo(actionsInfo, ALL_LINE_ON);
	LoadActoinInfo(actionsInfo, ALL_LINE_Off);
	LoadActoinInfo(actionsInfo, UP_MOVE_ON);
	LoadActoinInfo(actionsInfo, UP_MOVE_OFF);
	LoadActoinInfo(actionsInfo, DOWN_MOVE_ON);
	LoadActoinInfo(actionsInfo, DOWN_MOVE_OFF);
	LoadInterLock();
	LoadInterLockResult();
}

void DbOperation::LoadInterLock()
{
	string	sql = "SELECT INTERLOCKID,stagnname, STAG_CLASS,SDEVTAG_A, SDEVTAG_B,  SDEVTAG_C, SDEVTAG_D, SDEVTAG_E FROM t_scada_interlock_detail";
	
	try
	{
		DbOperation::connect_db();
		SACommand cmd(&(DbOperation::get_connection()), sql.c_str());
		cmd.Execute();
		while (cmd.FetchNext()) 
		{
			vector<string> item;
			int interlocId = static_cast<long long>(cmd.Field("INTERLOCKID").asLong());

			item.push_back(std::string(cmd.Field("stagnname").asString()));
			item.push_back(std::to_string(static_cast<long long>(cmd.Field("STAG_CLASS").asLong())));
			if (std::string(cmd.Field("SDEVTAG_A").asString()).length() != 0)
				item.push_back(std::string(cmd.Field("SDEVTAG_A").asString()));
			if (std::string(cmd.Field("SDEVTAG_B").asString()).length() != 0)
				item.push_back(std::string(cmd.Field("SDEVTAG_B").asString()));
			if (std::string(cmd.Field("SDEVTAG_C").asString()).length() != 0)
				item.push_back(std::string(cmd.Field("SDEVTAG_C").asString()));
			if (std::string(cmd.Field("SDEVTAG_D").asString()).length() != 0)
				item.push_back(std::string(cmd.Field("SDEVTAG_D").asString()));
			if (std::string(cmd.Field("SDEVTAG_E").asString()).length() != 0)
				item.push_back(std::string(cmd.Field("SDEVTAG_E").asString()));
			interLockDetail[interlocId] = item;
		}
		BOOST_LOG_TRIVIAL(info) << "加载闭锁逻辑基础信息成功！";
	}
	catch (SAException& e)
	{
		BOOST_LOG_TRIVIAL(fatal) << e.ErrText();
		BOOST_LOG_TRIVIAL(fatal) << "加载闭锁逻辑基础信息失败！";
		return;
	}
}

void DbOperation::LoadInterLockResult()
{
	string	sql = "select stag_class, stagvalue, on_off from t_scada_torf";

	try
	{
		DbOperation::connect_db();
		SACommand cmd(&(DbOperation::get_connection()), sql.c_str());
		cmd.Execute();
		while (cmd.FetchNext()) 
		{
			pair<int, int> p(cmd.Field("stag_class").asLong(), cmd.Field("stagvalue").asLong());
			map<pair<int, int>,string> m;
			tOrf[p] =std::string(cmd.Field("on_off").asString()); 
		}
		BOOST_LOG_TRIVIAL(info) << "加载闭锁逻辑真值表成功！";
	}
	catch (SAException& e)
	{
		BOOST_LOG_TRIVIAL(fatal) << e.ErrText();
		BOOST_LOG_TRIVIAL(fatal) << "加载闭锁逻辑真值表失败！";
		return;
	}
}