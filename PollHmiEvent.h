#ifndef POLL_HMI_EVENT_H_
#define POLL_HMI_EVENT_H_

#include "IFixPDBOperation.h"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

class PollHmiEvent{
public:
	//PollHmiEvent():tag("OCCPOW_CKKP001_001"),node("OCC"),pdb(node){}
	PollHmiEvent():tag("OCCPOW_CKKP001_001"),node("RMB"),pdb(node){}
	void PollEvent();
	void ConcelAction();
	void Start();
	static void Stop() { isRun = false; }

private:
	void SleepSomeSecond(int second) const
	{
		boost::this_thread::sleep(boost::posix_time::seconds(second));
	}

	const std::string tag;
	const std::string node; 
	IFixPDBOperation pdb;
	static volatile bool isRun;
};

#endif