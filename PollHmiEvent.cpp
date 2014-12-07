#include "stdafx.h"
#include "PollHmiEvent.h"
#include <boost/function.hpp>
#include "ControlManager.h"

volatile bool PollHmiEvent::isRun(true);

void PollHmiEvent::PollEvent()
{
	while (isRun)
	{
		float result;
		if (pdb.ReadPDBValue(tag.c_str(), result))
		{
			int value = result;
			int cardId = value / 10;
			int commandType = value % 10;
			ControlManager cm;
			cm.ProConditionCheck(DbOperation::allLineOff);
			cm.Run("1");
		}
		SleepSomeSecond(1);
	}
}

void PollHmiEvent::Start()
{
	boost::function<void()> f = boost::bind(&PollHmiEvent::PollEvent,this);
	boost::thread thrd( f );
	thrd.join();
}