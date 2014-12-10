#include "stdafx.h"
#include <boost/thread/thread.hpp>
#include <boost/function.hpp>
#include <algorithm>
#include <time.h>
#include "ControlManager.h"
#include "PscadaItem.h"
#include "LoadConfig.h"
#include "DbOperation.h"

ControlManager::ControlManager():index_(0),hmiPdb_("OCC")
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
	bool isOk = true;

	for (auto it = actionsContent.cbegin(); it != actionsContent.cend(); ++it)
	{
		std::shared_ptr<IFixPDBOperation> pdb = PdbOperationFactory(LoadConfig::scadaType, (*it)[STA_ALIAS_ ].substr(0, 3));
		actionName = (*it)[ACTIONNAME_];
		devacttag = (*it)[DEVACTTAG_] + OPERATION_AREA;				
		int value = -100;

		if (pdb->ReadPDBValue(devacttag.c_str(), value))
		{
			if (value != OPERATION_AREA_OK)
			{
				BOOST_LOG_TRIVIAL(info) << actionName+": 操作场所不具备条件";
				hmiPdb_.SetErrorMesage(GetIndex(), actionName+"操作场所不具备条件");
				isOk = false;
			}
		}
		else
		{
			hmiPdb_.SetErrorMesage(GetIndex(),"读取iFix错误");
			BOOST_LOG_TRIVIAL(error) <<"检查操作场所时iFix读取"+devacttag+"错误！";
			isOk = false;
		}
	}
	if (isOk)
		return true;
	else
		return false;
}

bool ControlManager::CheckHangOn(const vector<vector<string>> &actionsContent)
{
	std::string tagName1, tagName2, tagName3, classId;
	bool isOk = true;

	for (auto it = actionsContent.cbegin(); it != actionsContent.cend(); ++it)
	{
		std::shared_ptr<IFixPDBOperation> pdb = PdbOperationFactory(LoadConfig::scadaType, (*it)[STA_ALIAS_].substr(0, 3));
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
			int value = PDB_INIT_VALUE;
			if (pdb->ReadPDBValue(tag->c_str(), value))
			{
				if (HANG_ON_OK != value)
				{
					BOOST_LOG_TRIVIAL(info) << (*it)[1] + " : 挂牌不具备操作条件";
					hmiPdb_.SetErrorMesage(GetIndex(), (*it)[1]+"挂牌不具备条件");
					isOk = false;
					break;
				}
			}
			else
			{
				BOOST_LOG_TRIVIAL(info) << std::string(tag->c_str()) + (": 挂牌判断时读取iFix错误");
				isOk = false;
				break;
			}
		}
	}
	if (isOk)
		return true;
	else
		return false;
}

bool ControlManager::CheckCloseLogic(string actionId, std::shared_ptr<IFixPDBOperation> pdb)
{
	if (actionId == "-1")
		return true;

	vector<string> tags =DbOperation::interLockDetail[stoi(actionId)]; 
	int res = 0, weight = 1;

	/*当记录没有需检测的tag点时直接返回（属于异常情况）*/
	if (tags.size() < 3)
	{
		return false;
	}
	for (auto it = tags.cbegin() + 2; it != tags.cend(); ++it)
	{
		int value = PDB_INIT_VALUE;
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

bool ControlManager::ProConditionCheck(int cardId)
{
	std::shared_ptr<IFixPDBOperation> pdb = PdbOperationFactory(LoadConfig::scadaType, "OCC");
	bool isOk = true;

	if (!CheckOperationArea(DbOperation::actionsInfo[cardId]))
	{
		isOk = false;
		pdb->SetCheckResult(OPERATION_AREA_ERROR);
	}
	if (!CheckHangOn(DbOperation::actionsInfo[cardId]))
	{
		isOk = false;	
		pdb->SetCheckResult(HAND_ON_ERROR);
	}

	if (isOk)
	{
		pdb->SetCheckResult(CHECK_OK);
		return true;
	}
	else
	{
		return false;
	}
}

int ControlManager::GetIndex()
{
	boost::mutex::scoped_lock lock(io_mutex);
	long long i = index_++;
	return i % 10;
}

void ControlManager::RunAction(vector<vector<string>> &actionsContent)
{
	std::string tagName, statusTagName, nodeName;
	bool isOk = true;
	SetRunFlag(true);
	std::sort(actionsContent.begin(), actionsContent.end(), CompareAction);

	for (auto it = actionsContent.begin(); it != actionsContent.end(); ++it)
	{	
		if(!GetRunFlag())
			return;

		std::shared_ptr<IFixPDBOperation> pdbPtr = PdbOperationFactory(LoadConfig::scadaType, (*it)[STA_ALIAS_].substr(0, 3));
		int targetValue = std::stoi((*it)[ACT_AVALUE_]);
		int actionId = std::stoi((*it)[ACTIONID_ ]);
		string classType = ((*it)[DEV_CLASS_ID_]);
		tagName = ((*it)[DEVACTTAG_ ]);
		statusTagName = ((*it)[DEVADDTAG_]);
		nodeName = (*it)[STA_ALIAS_];
		std::shared_ptr<TwoPointControl> actionPtr = ControlTypeFactoty();
		int res = PDB_INIT_VALUE;
		if (actionPtr->CheckEquipCurrentState(*pdbPtr, tagName + REMOTE_FEEDBACK, res))
		{
			if (res == targetValue + LoadConfig::twoPointOffset)
				continue;
		}
		else
		{
			return;
		}
		if (!CheckCloseLogic((*it)[INTERLOCK_ID_], pdbPtr))
			continue;
		if (actionPtr->ToDoCommand(*pdbPtr, tagName, statusTagName, targetValue) != ACTION_OK)
		{
				isOk = false;
				hmiPdb_.SetActoinResult((*it)[STA_ALIAS_], classType, targetValue);
				//hmiPdb_.SetErrorMesage(GetIndex(), (*it)[ACTIONNAME_]+"执行失败");
		}
	}	
	if (isOk)
		hmiPdb_.SetActoinResult(nodeName, 1);
	else
		hmiPdb_.SetActoinResult(nodeName, 2);
}

void ControlManager::Run(int cardId)
{	
	time_t start, ends;
	boost::function<void(vector<vector<string>> &)> f = boost::bind(&ControlManager::RunAction, this, _1);
	boost::thread_group tg;
	map<int,vector<vector<string>>> actionGroups;

	start = time(NULL);
	GetGroupContent(actionGroups, DbOperation::actionsInfo[cardId]);
	for (map<int,vector<vector<string>>>::const_iterator it = actionGroups.cbegin(); it != actionGroups.cend(); ++it)
	{
		tg.add_thread(new boost::thread(f, (*it).second)); 
	}
	tg.join_all();
	ends = time(NULL);
	BOOST_LOG_TRIVIAL(info) << "执行耗时： " + std::to_string(static_cast<long long>(difftime(ends,start))) + "秒";
	hmiPdb_.SetErrorMesage(GetIndex(), string("执行耗时： " + std::to_string(static_cast<long long>(difftime(ends,start)))+ "秒").c_str());
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


