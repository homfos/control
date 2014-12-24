#include "stdafx.h"
#include <boost/thread/thread.hpp>
#include <boost/function.hpp>
#include <algorithm>
#include <time.h>
#include "ControlManager.h"
#include "PscadaItem.h"
#include "LoadConfig.h"
#include "DbOperation.h"

ControlManager::ControlManager():index_(0),hmiR1Pdb("OCCR1"),hmiR2Pdb("OCCR2")
{
}

ControlManager::~ControlManager()
{
}

bool ControlManager::flag = true;

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
				HmiSetErrorMesage("操作场所不具备条件");
				isOk = false;
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(error) <<"检查操作场所时iFix读取"+devacttag+"错误！";
			HmiSetErrorMesage("操作场所不具备条件");
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
					HmiSetErrorMesage("挂牌不具备操作条件");
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

bool ControlManager::ProConditionCheck(int cardId)
{
	std::shared_ptr<IFixPDBOperation> r1Pdb = PdbOperationFactory(LoadConfig::scadaType, "OCCR1");
	std::shared_ptr<IFixPDBOperation> r2Pdb = PdbOperationFactory(LoadConfig::scadaType, "OCCR2");
	bool isOk = true;

	if (!CheckOperationArea(DbOperation::actionsInfo[cardId]))
	{
		isOk = false;
		r1Pdb->SetCheckResult(OPERATION_AREA_ERROR);
		r2Pdb->SetCheckResult(OPERATION_AREA_ERROR);
	}
	if (!CheckHangOn(DbOperation::actionsInfo[cardId]))
	{
		isOk = false;	
		r1Pdb->SetCheckResult(HAND_ON_ERROR);
		r2Pdb->SetCheckResult(HAND_ON_ERROR);
	}

	if (isOk)
	{
		r1Pdb->SetCheckResult(CHECK_OK);
		r2Pdb->SetCheckResult(CHECK_OK);
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
	int i = index_++;
	return i;
}

void ControlManager::RunAction(vector<vector<string>> &actionsContent)
{
	std::string tagName, statusTagName, nodeName;
	bool isOk = true;
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
		HmiSetActionResult((*it)[STA_ALIAS_], classType, targetValue);
		if (actionPtr->CheckEquipCurrentState(*pdbPtr, tagName + REMOTE_FEEDBACK, res))
		{
			if (res == targetValue + LoadConfig::twoPointOffset)
			{
				BOOST_LOG_TRIVIAL(info) << (*it)[ACTIONNAME_]+": 执行成功"; 
				HmiSetActionResult((*it)[STA_ALIAS_], classType, SINGLE_STEP_OK);
				continue;
			}
		}
		if (!CheckCloseLogic((*it)[INTERLOCK_ID_], pdbPtr))
		{
			isOk = false;
			BOOST_LOG_TRIVIAL(info) << (*it)[ACTIONNAME_]+":逻辑闭锁"; 
			HmiSetActionResult((*it)[STA_ALIAS_], classType, SINGLE_STEP_ERROR);
			HmiSetErrorMesage((*it)[1]+"逻辑闭锁");
			continue;
		}
		int commandRet = DEVICE_REJECT_ACTION;
		commandRet = actionPtr->ToDoCommand(*pdbPtr, tagName, statusTagName, targetValue);
		if (commandRet != ACTION_OK)
		{
			isOk = false;

			if (commandRet == ACTION_RETURN_VALUE_ERROR)
				HmiSetErrorMesage((*it)[ACTIONNAME_]+":控制命令发送成功，但设备未动作");
			else
				HmiSetErrorMesage((*it)[ACTIONNAME_]+":执行失败");

			HmiSetActionResult((*it)[STA_ALIAS_], classType, SINGLE_STEP_ERROR);
			BOOST_LOG_TRIVIAL(info) << (*it)[ACTIONNAME_]+": 执行失败"; 
		}
		else
		{
			HmiSetActionResult((*it)[STA_ALIAS_], classType, SINGLE_STEP_OK);
			BOOST_LOG_TRIVIAL(info) << (*it)[ACTIONNAME_]+": 执行成功"; 
		}
	}	
	if (isOk)
		HmiSetActionResult(nodeName, COMMAND_ACTION_OK);
	else
		HmiSetActionResult(nodeName, COMMAND_ACTOIN_ERROR);
}

void ControlManager::Run(int cardId)
{	
	BOOST_LOG_TRIVIAL(info) << "开始执行卡片：" + std::to_string(static_cast<long long>(cardId)); 
	SetRunFlag(true);
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
	HmiSetErrorMesage(string("执行完成，共耗时: " + std::to_string(static_cast<long long>(difftime(ends,start)))+ "秒").c_str());
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


