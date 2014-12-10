#ifndef TWO_POINT_CONTROL_H_
#define TWO_POINT_CONTROL_H_

#include "IFixPDBOperation.h"

class TwoPointControl {
public:
	TwoPointControl ();

	~TwoPointControl();

	int ToDoCommand(const IFixPDBOperation & pdb, std::string,std::string, int);
	bool SendChooseCommand(const IFixPDBOperation & pdb, std::string tagName, int value);
	bool CheckCommandResult(const IFixPDBOperation & pdb, std::string tagName, int &value);
	bool SendControlCommand(const IFixPDBOperation & pdb, std::string tagName, int value);
	bool CheckExecuteCommandResult(const IFixPDBOperation & pdb, std::string tagName, int &value);
	bool CheckEquipCurrentState(const IFixPDBOperation & pdb, std::string tagName, int &value);
	void SleepSomeSecond(int second) const;

private:

};



#endif