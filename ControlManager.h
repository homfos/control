#ifndef CONTROL_MANAGER_H_
#define CONTROL_MANAGER_H_

#include "TwoPointControl.h"
#include "IFixPDBOperation.h"
#include "DbOperation.h"
#include <array>


using std::string;
using std::vector;
using std::array;

bool CompareAction(vector<string> &a, vector<string> &b);

class ControlManager
{
public:
	ControlManager();
	~ControlManager();

	bool CheckOperationArea(const vector<vector<string>> &actionContent);
	bool CheckHangOn(const vector<vector<string>> &actionsContent);
	bool CheckCloseLogic(string actionId,std::shared_ptr<IFixPDBOperation> pdb);
	bool ProConditionCheck(int cardId);
	void RunAction(vector<vector<string>> &actionsContent);

	std::shared_ptr<TwoPointControl> ControlTypeFactoty();
	void GetGroupContent(map<int,vector<vector<string>>> &actionGroups, const vector<vector<string>> &actionList);
	std::shared_ptr<IFixPDBOperation> PdbOperationFactory(std::string scadaType, std::string);
	static void SetRunFlag(bool isRun);
	static bool GetRunFlag();
	bool isSatisfy;
	void Run(int cardId);
	int GetIndex();

private:
	static bool flag;
	boost::mutex io_mutex;
	int index_;
	IFixPDBOperation hmiR1Pdb, hmiR2Pdb;

	void HmiSetErrorMesage(string msg)
	{
		int index = GetIndex();
		if (index < 9)
		{
			hmiR1Pdb.SetErrorMesage(index, msg);
			hmiR2Pdb.SetErrorMesage(index, msg);
		}
		else
		{
			hmiR1Pdb.SetErrorMesage(9, "错误过多，详见一次图");
			hmiR2Pdb.SetErrorMesage(9, "错误过多，详见一次图");
		}
	}

	void HmiSetActionResult(string nodeName, int value)
	{
		hmiR1Pdb.SetActoinResult(nodeName, value);
		hmiR2Pdb.SetActoinResult(nodeName, value);
	}

	void HmiSetActionResult(string nodeName, string actionClass, int value)
	{
		hmiR1Pdb.SetActoinResult(nodeName, actionClass,value);
		hmiR2Pdb.SetActoinResult(nodeName, actionClass,value);
	}
};



#endif