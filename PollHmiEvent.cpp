#include "stdafx.h"
#include "PollHmiEvent.h"
#include <boost/function.hpp>
#include "ControlManager.h"

volatile bool PollHmiEvent::isRun(true);

void PollHmiEvent::PollEvent()
{
	while (isRun)
	{
		int result = -100;

		SleepSomeSecond(1);
		if (pdb.ReadPDBValue(tag.c_str(), result))
		{
			if (result <2000 || result > 2100)
				continue;
			BOOST_LOG_TRIVIAL(info) << "¶Áµ½HMIÖ¸Áî£º" + std::to_string(static_cast<long long>(result));
			int cardId = result / 10;
			int commandType = result % 10;
			ControlManager cm;
			pdb.WritePDBValue(tag.c_str(), 100);
			switch(commandType)
			{
			case START_CARD_EXECUTE:
				cm.ProConditionCheck(cardId);
				break;
			case CHECK_CARD_REQUIRE:
				break;
			case STOP_CARD_EXECUTE:
				cm.Run(cardId);
				break;
			default:
				break;
			}
			pdb.WritePDBValue(tag.c_str(), 999);
		}
	}
}

void PollHmiEvent::Start()
{
	boost::function<void()> f = boost::bind(&PollHmiEvent::PollEvent,this);
	boost::thread thrd( f );
	thrd.join();
}