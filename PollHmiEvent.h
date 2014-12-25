#ifndef POLL_HMI_EVENT_H_
#define POLL_HMI_EVENT_H_

#include "IFixPDBOperation.h"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

class PollHmiEvent{
public:
	PollHmiEvent():tag("OCCPOW_CKKP001_001"),checkFeedTag("OCCPOW_CKKP001_002"), 
		actionFeedTag("OCCPOW_CKKP001_003"), r1Pdb("OCCR1"), r2Pdb("OCCR2"), pdb(LoadConfig::hostName){}
	void PollEvent();
	void ConcelAction();
	void Start();
	static void Stop() { isRun = false; }

private:
	void SleepSomeSecond(int second) const
	{
		boost::this_thread::sleep(boost::posix_time::seconds(second));
	}

	void WriteOCCHmiTag(std::string tagName, int value)
	{
		r1Pdb.WritePDBValue(tagName.c_str(), value);
		r2Pdb.WritePDBValue(tagName.c_str(), value);
	}

	const std::string tag, checkFeedTag, actionFeedTag;
	IFixPDBOperation r1Pdb, r2Pdb, pdb;
	static volatile bool isRun;
};

#endif