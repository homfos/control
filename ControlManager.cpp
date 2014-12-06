#include "stdafx.h"
#include <boost/thread/thread.hpp>
#include <boost/function.hpp>
#include <algorithm>
#include "ControlManager.h"
#include "PscadaItem.h"
#include "LoadConfig.h"
#include "DbOperation.h"

ControlManager::ControlManager()
{
}

ControlManager::~ControlManager()
{
}

bool ControlManager::flag = false;

void ControlManager::SetRunFlag(bool isRun)
{
	flag = isRun;
}

bool ControlManager::GetRunFlag()
{
	return flag;
}

bool ControlManager::CheckOperationArea(const vector<vector<string>> &actionsContent)
{
	std::string devacttag;
	std::string actionName;

	for (auto it = actionsContent.cbegin(); it != actionsContent.cend(); ++it)
	{
		std::shared_ptr<IFixPDBOperation> pdb = PdbOperationFactory(LoadConfig::scadaType, (*it)[STA_ALIAS_ ]);
		actionName = (*it)[ACTIONNAME_];
		devacttag = (*it)[DEVACTTAG_] + OPERATION_AREA;				
		float value = -100;

		if (pdb->ReadPDBValue(devacttag.c_str(), value))
		{
			if (value != OPERATION_AREA_OK)
			{
				BOOST_LOG_TRIVIAL(info) << actionName+": 操作场所不具备条件";
				return false;
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(info) <<"检查操作场所时iFix读取"+devacttag+"错误！";
			return false;
		}
	}
	return true;
}

bool ControlManager::CheckHangOn(const vector<vector<string>> &actionsContent)
{
	std::string tagName1, tagName2, tagName3, classId;

	for (auto it = actionsContent.cbegin(); it != actionsContent.cend(); ++it)
	{
		std::shared_ptr<IFixPDBOperation> pdb = PdbOperationFactory(LoadConfig::scadaType, (*it)[STA_ALIAS_]);
		std::vector<std::string> tags;
		/*	classId = cmd.Field("devaddtag").asString();
		std::size_t found = classId.find("DLQ");
		if (found == std::string::npos)
		continue; */
		classId = (*it)[DEVADDTAG_ ];
		tags.push_back(classId + LoadConfig::hangOnTag1);
		tags.push_back(classId + LoadConfig::hangOnTag2);
		tags.push_back(classId + LoadConfig::hangOnTag3);
		for ( vector<string>::iterator tag = tags.begin(); tag != tags.cend(); ++tag)
		{
			float value = -100;
			if (pdb->ReadPDBValue(tag->c_str(), value))
			{
				if (HANG_ON_OK != value)
				{
					BOOST_LOG_TRIVIAL(info) << (*it)[1] + " : 挂牌不具备操作条件";
					return false;
				}
			}
			else
			{
				BOOST_LOG_TRIVIAL(info) << std::string(tag->c_str()) + (": 挂牌判断时读取iFix错误");
				return false;
			}
		}
	}
	return false;
}

bool ControlManager::CheckCloseLogic(string actionId, std::shared_ptr<IFixPDBOperation> pdb)
{
	vector<string> tags =DbOperation::interLockDetail[stoi(actionId)]; 
	int res = 0, weight = 1;

	/*当记录没有需检测的tag点时直接返回（属于异常情况）*/
	if (tags.size() < 3)
	{
		return false;
	}
	for (auto it = tags.cbegin() + 2; it != tags.cend(); ++it)
	{
		float value = 0;
		if (!pdb->ReadPDBValue(it->c_str(), value))
			return false;
		res += (value - 1)*weight;
		weight *= 10;
	}

	int stagClass = stoi(tags[1]);
	pair<int,int> p(stagClass, res);

	if (DbOperation::tOrf[p] == "T")
		return true;

	return false;
}

//std::shared_ptr<std::string> ControlManager::CheckCloseLogic(std::string cardId, std::string sql, string nodeStr, int offSet)
//{
//	DbOperation::connect_db();
//	SACommand cmd(&DbOperation::get_connection(), sql.c_str());
//	std::shared_ptr<std::string> checkResult = std::make_shared<std::string>();
//	std::shared_ptr<IFixPDBOperation> pdb = PdbOperationFactory(LoadConfig::scadaType, nodeStr);

//	int stagClass;
//	std::string classID;
//	std::vector<std::string> tags;
//	int res = 0, weight = 1;

//	try
//	{
//		cmd.Execute();
//		while (cmd.FetchNext()) 
//		{
//			stagClass = cmd.Field("STAG_CLASS").asLong();
//			classID = cmd.Field("DEV_CLASS_ID").asString();
//			tags.push_back(std::string(cmd.Field("SDEVTAG_A").asString()));
//			tags.push_back(std::string(cmd.Field("SDEVTAG_B").asString()));
//			tags.push_back(std::string(cmd.Field("SDEVTAG_C").asString()));
//			tags.push_back(std::string(cmd.Field("SDEVTAG_D").asString()));
//			tags.push_back(std::string(cmd.Field("SDEVTAG_E").asString()));

//			for (auto it = tags.begin(); it != tags.end(); ++it)
//			{
//				if(it->size()> 5)
//				{
//					float value = 0;
//					if (!pdb->ReadPDBValue(it->c_str(), value))
//						return nullptr;
//					res += (value - offSet)*weight;
//					weight *= 10;
//				}
//			}
//			sql = "SELECT * FROM t_scada_torf where STAG_CLASS = " + std::to_string(static_cast<long long>(stagClass)) + " and STAGVALUE = "+ std::to_string(static_cast<long long>(res));
//			SACommand sub_cmd(&DbOperation::get_connection(), sql.c_str());
//			sub_cmd.Execute();
//			while (sub_cmd.FetchNext())
//			{
//				std::string onOff = sub_cmd.Field("on_off").asString();
//				if(onOff != "T")
//					checkResult->append(std::string(sub_cmd.Field("lockname").asString()) + ": 闭锁逻辑不具备条件\n");
//			}	
//		}
//	}
//	catch (SAException& e)
//	{
//		return nullptr;
//	}

//	return checkResult;
//}

bool ControlManager::ProConditionCheck(const vector<vector<string>> &actionsContent)
{
	std::shared_ptr<IFixPDBOperation> pdb = PdbOperationFactory(LoadConfig::scadaType, "OCC");

	if (!CheckOperationArea(actionsContent))
	{
		pdb->SetCheckResult(2);
		return false;
	}
	if (!CheckHangOn(actionsContent))
	{
		pdb->SetCheckResult(2);
		return false;	
	}

	pdb->SetCheckResult(1);
	return true;
}

void ControlManager::RunAction(vector<vector<string>> &actionsContent)
{
	std::string tagName, statusTagName;
	SetRunFlag(true);
	std::sort(actionsContent.begin(), actionsContent.end(), CompareAction);

	for (auto it = actionsContent.begin(); it != actionsContent.end(); ++it)
	{	
		if(!GetRunFlag())
			return;

		std::shared_ptr<IFixPDBOperation> pdbPtr = PdbOperationFactory(LoadConfig::scadaType, (*it)[STA_ALIAS_]);
		int targetValue = std::stoi((*it)[ACT_AVALUE_]);
		int actionId = std::stoi((*it)[ACTIONID_ ]);
		std::shared_ptr<TwoPointControl> actionPtr = ControlTypeFactoty();
		tagName = ((*it)[DEVACTTAG_ ]);
		statusTagName = ((*it)[DEVADDTAG_]);

		if (actionPtr->CheckEquipCurrentState(*pdbPtr, tagName, targetValue) == targetValue + LoadConfig::twoPointOffset)
			continue;
		if (!CheckCloseLogic((*it)[ACTIONID_], pdbPtr))
			continue;
		actionPtr->ToDoCommand(*pdbPtr, tagName, statusTagName, targetValue); 
	}	
}

//std::string ControlManager::GetLogicCheckSqlString(std::string cardId, int actionSeqId, int groupId)
//{
//	std::string sql;

//	sql = "SELECT actionname, STAG_CLASS,t_scada_actioncoll.DEV_CLASS_ID as DEV_CLASS_ID,SDEVTAG_A, SDEVTAG_B,  SDEVTAG_C, SDEVTAG_D, SDEVTAG_E FROM t_scada_actioncoll  join t_scada_interlock_detail on INTERLOCK_ID = INTERLOCKID ";
//	sql.append(" where cardid = ");
//	sql.append(cardId);
//	sql.append(" and ignore_flag = 'T' and actionseqid = ");
//	sql.append(std::to_string(static_cast<long long>(actionSeqId)));
//	sql.append(" and actiongroup = ");
//	sql.append(std::to_string(static_cast<long long>(groupId)));

//	return sql;
//}

//void ControlManager::GetActionsContent(vector<vector<string>> &actionsContent, string cardId, int groupId)
//{
//	std::string sqlStr;
//	sqlStr = "select * from T_SCADA_ACTIONCOLL where cardid = " + cardId;
//	sqlStr.append(" and actiongroup =" + std::to_string(static_cast<long long>(groupId)));
//	sqlStr.append(" order by ACTIONSEQID") ;

//	try
//	{
//		DbOperation::connect_db();
//		SACommand cmd(&(DbOperation::get_connection()), sqlStr.c_str());
//		cmd.Execute();
//		while (cmd.FetchNext()) 
//		{
//			vector<string> action;
//			action.push_back(std::to_string(static_cast<long long>(cmd.Field("ACTIONID").asLong())));
//			action.push_back(std::string(cmd.Field("ACTIONNAME").asString()));
//			action.push_back(std::string(cmd.Field("DEVACTTAG").asString()));
//			action.push_back(std::string(cmd.Field("DEVADDTAG").asString()));
//			action.push_back(std::string(cmd.Field("DEVLOCTAG").asString()));
//			action.push_back(std::to_string(static_cast<long long>(cmd.Field("ACT_CLASS").asLong())));
//			action.push_back(std::to_string(static_cast<long long>(cmd.Field("ACT_AVALUE").asLong())));
//			action.push_back(std::to_string(static_cast<long long>(cmd.Field("ACTIONGROUP").asLong())));
//			action.push_back(std::to_string(static_cast<long long>(cmd.Field("ACTIONSEQID").asLong())));
//			action.push_back(std::to_string(static_cast<long long>(cmd.Field("INTERLOCK_ID").asLong())));
//			action.push_back(std::string(cmd.Field("STA_ALIAS").asString()));
//			actionsContent.push_back(action);
//		}
//	}
//	catch (SAException& e)
//	{
//		actionsContent.clear();
//		return;
//	}
//}

void ControlManager::Run(std::string cardId)
{	
	boost::function<void(vector<vector<string>> &)> f;
	f = boost::bind(&ControlManager::RunAction, this, _1);
	boost::thread_group tg;
	map<int,vector<vector<string>>> actionGroups;
	GetGroupContent(actionGroups, DbOperation::allLineOn);
	//std::sort(actionGroups.begin(), actionGroups.end(), CompareAction);
	for (map<int,vector<vector<string>>>::const_iterator it = actionGroups.cbegin(); it != actionGroups.cend(); ++it)
	{
		tg.add_thread(new boost::thread(f, (*it).second)); 
	}
	tg.join_all();
}

std::shared_ptr<TwoPointControl> ControlManager::ControlTypeFactoty()
{
	return std::shared_ptr<TwoPointControl> (new TwoPointControl());
}

std::shared_ptr<IFixPDBOperation> ControlManager::PdbOperationFactory(std::string scadaType, std::string nodeName)
{
	return std::shared_ptr<IFixPDBOperation> (std::make_shared<IFixPDBOperation>(nodeName));
}


void ControlManager::GetGroupContent(map<int,vector<vector<string>>> &actionGroups, const vector<vector<string>> &actionList)
{
	for (vector<vector<string>>::const_iterator it = actionList.cbegin(); it != actionList.cend(); ++it)
	{
		int groupId = stoi((*it)[ACTIONGROUP_]);
		actionGroups[groupId].push_back(*it);
	}
}

bool CompareAction(vector<string> &a, vector<string> &b)
{
	return (stoi(a[ACTIONSEQID_]) < stoi(b[ACTIONSEQID_]));
}


