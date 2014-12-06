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
//	std::shared_ptr<std::string> CheckCloseLogic(std::string, std::string, std::string nodeStr, int offSet);
	bool CheckCloseLogic(string actionId,std::shared_ptr<IFixPDBOperation> pdb);
	bool ProConditionCheck(const vector<vector<string>> &actionsContent);
	void RunAction(vector<vector<string>> &actionsContent);
	//void GetActionsContent(vector<vector<string>> &actionContent, string cardId, int groupId = -1);

	std::shared_ptr<TwoPointControl> ControlTypeFactoty();
	void GetGroupContent(map<int,vector<vector<string>>> &actionGroups, const vector<vector<string>> &actionList);
	std::shared_ptr<IFixPDBOperation> PdbOperationFactory(std::string scadaType, std::string);
//	std::string GetLogicCheckSqlString(std::string cardId, int actionSeqId, int groupId = -1);
	static void SetRunFlag(bool isRun);
	static bool GetRunFlag();
	bool isSatisfy;
	void Run(std::string);

private:
	static bool flag;
};



#endif