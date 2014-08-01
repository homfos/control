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
	long long index_;
	IFixPDBOperation hmiPdb_;
};



#endif