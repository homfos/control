#include "stdafx.h"

#include <boost/thread.hpp>

#include "TwoPointControl.h"
#include "PscadaItem.h"
#include "LoadConfig.h"


TwoPointControl::TwoPointControl()
{

}

TwoPointControl:: ~TwoPointControl()
{

}

void TwoPointControl::SleepSomeSecond(int second) const
{
	boost::this_thread::sleep(boost::posix_time::seconds(second));
}

bool TwoPointControl::SendChooseCommand(const IFixPDBOperation & pdb, std::string tagName, int value)
{
	return pdb.WritePDBValue(tagName.c_str(), value);
}

bool TwoPointControl::CheckCommandResult(const IFixPDBOperation & pdb, std::string tagName, int &value)
{
	return pdb.ReadPDBValue(tagName.c_str(), value);
}

bool TwoPointControl::SendControlCommand(const IFixPDBOperation & pdb, std::string tagName, int value)
{
	return pdb.WritePDBValue(tagName.c_str(), value);
}

bool TwoPointControl::CheckExecuteCommandResult(const IFixPDBOperation & pdb, std::string tagName, int &value)
{
	return pdb.ReadPDBValue(tagName.c_str(), value);
}

bool TwoPointControl::CheckEquipCurrentState(const IFixPDBOperation & pdb, std::string tagName, int &value)
{
	return pdb.ReadPDBValue(tagName.c_str(), value);
}

int TwoPointControl::ToDoCommand(const IFixPDBOperation & pdb, std::string tagName,std::string statusTagName, int targetValue)
{
	bool chooseFlag = false;
	int ret;

	if(!SendChooseCommand(pdb,  tagName + LoadConfig::remoteControl, targetValue + LoadConfig::chooseCommandOffset))
		return ACTION_ERR;
	for (int i = 0; i < LoadConfig::commandTimeout; i++)
	{
		SleepSomeSecond(1);
		if (CheckCommandResult(pdb,  tagName + LoadConfig::remoteControl, ret))
		{
			if (ret == LoadConfig::chooseFeedBackOffset)
			{
				chooseFlag = true;
				SleepSomeSecond(1);
				break;
			}
		}
		else
		{
			return ACTION_ERR; 
		}
	}

	if (chooseFlag)
	{
		if (!SendControlCommand(pdb, tagName+LoadConfig::remoteControl, targetValue + LoadConfig::twoPointOffset))
			return ACTION_ERR;

		int  i = 0;
		while (i < LoadConfig::feedBackTimeout + 1)
		{
			if (i == LoadConfig::feedBackTimeout)
				return ACTION_ERR;
			SleepSomeSecond(1);
			int ret = -100;
			if (CheckExecuteCommandResult(pdb, statusTagName + LoadConfig::remoteControl, ret))
			{
				if (ret == EXECUTE_FEEDBACK_OFFSET)
					break;
			}
			i++;
		}
		
		for (int i = 0; i < LoadConfig::feedBackTimeout; i++)
		{
			SleepSomeSecond(1);
			int ret = -100;
			if (CheckEquipCurrentState(pdb, statusTagName + LoadConfig::remoteFeedBack, ret))
			{
				if( ret == targetValue + LoadConfig::twoPointOffset)
					return ACTION_OK;
			}
		}
	}
	else
	{
		return ACTION_CHOOSE_DEVICE_TIMEOUT;
	}
	return ACTION_ERR;
}



