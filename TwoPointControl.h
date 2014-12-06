#ifndef TWO_POINT_CONTROL_H_
#define TWO_POINT_CONTROL_H_

#include "IFixPDBOperation.h"

class TwoPointControl {
public:
	TwoPointControl ();

	~TwoPointControl();

	int ToDoCommand(const IFixPDBOperation & pdb, std::string,std::string, int);
	int SendChooseCommand(const IFixPDBOperation & pdb, std::string tagName, int value);
	int CheckChooseCommandResult(const IFixPDBOperation & pdb, std::string tagName, int value);
	int SendControlCommand(const IFixPDBOperation & pdb, std::string tagName, int value);
	int CheckExecuteCommandResult(const IFixPDBOperation & pdb, std::string tagName, int value);
	int CheckEquipCurrentState(const IFixPDBOperation & pdb, std::string tagName, int value);

	void SleepSomeSecond(int second) const;

private:

};



#endif