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

int TwoPointControl::SendChooseCommand(const IFixPDBOperation & pdb, std::string tagName, int value)
{
	return pdb.WritePDBValue(tagName.c_str(), value);
}

int TwoPointControl::CheckChooseCommandResult(const IFixPDBOperation & pdb, std::string tagName, int value)
{
	return pdb.WritePDBValue(tagName.c_str(), value);
}

int TwoPointControl::SendControlCommand(const IFixPDBOperation & pdb, std::string tagName, int value)
{
	return pdb.WritePDBValue(tagName.c_str(), value);
}

int TwoPointControl::CheckExecuteCommandResult(const IFixPDBOperation & pdb, std::string tagName, int value)
{
	return pdb.WritePDBValue(tagName.c_str(), value);
}

int TwoPointControl::CheckEquipCurrentState(const IFixPDBOperation & pdb, std::string tagName, int value)
{
	return pdb.WritePDBValue(tagName.c_str(), value);
}

int TwoPointControl::ToDoCommand(const IFixPDBOperation & pdb, std::string tagName,std::string statusTagName, int targetValue)
{
	bool chooseFlag = false;
	int ret;
	//ret = CheckEquipCurrentState(pdb, targetValue, statusTagName + LoadConfig::remoteFeedBack);

	//if (ret == targetValue + LoadConfig::twoPointOffset)
	//	return ACTION_OK;

	SendChooseCommand(pdb,  tagName + LoadConfig::remoteControl, targetValue + LoadConfig::chooseCommandOffset);

	for (int i = 0; i < LoadConfig::commandTimeout; i++)
	{
		SleepSomeSecond(1);
		ret = CheckChooseCommandResult(pdb,  tagName + LoadConfig::remoteControl, targetValue);
		if (ret == LoadConfig::chooseFeedBackOffset)
		{
			chooseFlag = true;
			SleepSomeSecond(1);
			break;
		}
		else if(ret == targetValue + LoadConfig::twoPointOffset)
		{
			return ACTION_RETURN_VALUE_ERROR;
		} else if(ret == ACTION_ERR)
			return ACTION_ERR;
	}

	if (chooseFlag)
	{
		if (SendControlCommand(pdb, tagName+LoadConfig::remoteControl, targetValue + LoadConfig::twoPointOffset) == ACTION_ERR)
			return ACTION_ERR;

		int  i;
		i = 0;
		while (i < LoadConfig::feedBackTimeout + 1)
		{
			if (i == LoadConfig::feedBackTimeout)
				return ACTION_DEVICE_TIMEOUT;
			SleepSomeSecond(1);
			if (CheckExecuteCommandResult(pdb, statusTagName + LoadConfig::remoteControl, targetValue) == 7)
				break;
			i++;
		}
		
		for (int i = 0; i < LoadConfig::feedBackTimeout; i++)
		{
			SleepSomeSecond(1);
			if (CheckEquipCurrentState(pdb, statusTagName + LoadConfig::remoteFeedBack, targetValue) == targetValue + LoadConfig::twoPointOffset)
				return ACTION_OK;
		}
	}
	else
	{
		return ACTION_CHOOSE_DEVICE_TIMEOUT;
	}
	return ACTION_ERR;
}



