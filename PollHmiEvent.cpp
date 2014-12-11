#include "stdafx.h"
#include "PollHmiEvent.h"
#include <boost/function.hpp>
#include "ControlManager.h"
#include <time.h>

volatile bool PollHmiEvent::isRun(true);

void PollHmiEvent::PollEvent()
{
	time_t start, ends;  
	//ControlManager cm;
	//cm.ProConditionCheck(200);
	//cm.Run(200);
	while (isRun)
	{
		start = time(NULL);
		int result = PDB_INIT_VALUE;

		SleepSomeSecond(1);
		if (pdb.ReadPDBValue(tag.c_str(), result))
		{
			if (result < 2000 || result > 2100)
				continue;
			BOOST_LOG_TRIVIAL(info) << "读到HMI指令：" + std::to_string(static_cast<long long>(result));
			int cardId = result / 10;
			int commandType = result % 10;
			ControlManager cm;
			pdb.WritePDBValue(tag.c_str(), HAVE_RUN);
			switch(commandType)
			{
			case START_CARD_EXECUTE:
				cm.Run(cardId);
				break;
			case CHECK_CARD_REQUIRE:
				BOOST_LOG_TRIVIAL(info) << "开始检查执行条件：" + std::to_string(static_cast<long long>(cardId)); 
				cm.ProConditionCheck(cardId);
				BOOST_LOG_TRIVIAL(info) << "检查执行结束：" + std::to_string(static_cast<long long>(cardId)); 
				break;
			default:
				break;
			}
			ends = time(NULL);
			pdb.WritePDBValue(tag.c_str(), HAVE_DONE);
			pdb.SetErrorMesage(cm.GetIndex(), std::to_string(static_cast<long long>(difftime(ends,start))).c_str());
		}
	}
}

void PollHmiEvent::Start()
{
	boost::function<void()> f = boost::bind(&PollHmiEvent::PollEvent,this);
	boost::thread thrd( f );
	thrd.join();
}

void PollHmiEvent::ConcelAction()
{
	while (isRun)
	{
		int result = PDB_INIT_VALUE;
		SleepSomeSecond(1);
		if (pdb.ReadPDBValue(tag.c_str(), result))
		{
			int commandType = result % 10;
			if (commandType == STOP_CARD_EXECUTE)
			{
				ControlManager::SetRunFlag(false);
				BOOST_LOG_TRIVIAL(info) << "读到HMI终止指令：" + std::to_string(static_cast<long long>(result));
				pdb.WritePDBValue(tag.c_str(), HAVE_CONCEL);
			}
		}
	}
}