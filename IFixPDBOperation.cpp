#include "stdafx.h"

#include "IFixPDBOperation.h"
#include <boost/thread/mutex.hpp>


extern "C" 
{
#include <fixeda.h> // iFIX / DMACS header files
}

void IFixPDBOperation::SetCheckResult(int result)
{

}

bool IFixPDBOperation::WritePDBValue(const char* tagName, float value) const 
{
	INT16 err;
	GNUM group = eda_define_group(1, 0);
	THANDLE	ntf = eda_define_ntf(group, const_cast<char *>(nodeName.c_str()),
		const_cast<char *>(tagName),
		const_cast<char *>("F_CV"),
		NULL);

	eda_lookup(group);
	eda_lookup(group);
	eda_wait(group);
	// float value_float;
	err = eda_set_float(group, ntf, &value);

	eda_write(group);
	eda_wait(group);

	eda_delete_ntf(group, ntf);
	eda_delete_group(group);

	BOOST_LOG_TRIVIAL(info) << std::string("Write PDB") + "["+ tagName +"]:"+ std::to_string(static_cast<long double>(value)) + " Return Number: " + std::to_string((long long)err);

	if (err == 0)
		return true;
	else
		return false;
}

bool IFixPDBOperation::ReadPDBValue( const char* tagName , float &value) const
{

	float Tagdata = 0;
	
	GNUM eh;
	THANDLE hT;
	INT16 err;

	eh = eda_define_group(1,0);	

	hT = eda_define_ntf(eh, const_cast<char *>(nodeName.c_str()),
		const_cast<char *>(tagName),"f_cv",NULL);

	    eda_lookup(eh);

	    eda_wait(eh);

	    eda_read(eh);

	    // You must do this after every read.
	    eda_wait(eh);

	    err = eda_get_float(eh, hT, &value);

	     eda_wait(eh);
	    //clean up
	    eda_delete_ntf(eh, hT);
	    eda_delete_group(eh);

		BOOST_LOG_TRIVIAL(info) << std::string("Read PDB") + "["+ tagName +"]:"+ std::to_string(static_cast<long double>(value)) + " Return Number: " + std::to_string((long long)err);

	    if (err == 0)
	        return true;
	    else
	        return false;
			
}

IFixPDBOperation::IFixPDBOperation(std::string nodeStr)
{
	nodeName = nodeStr;
}

IFixPDBOperation::~IFixPDBOperation()
{

}