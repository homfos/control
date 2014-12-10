#include "stdafx.h"

#include "IFixPDBOperation.h"
#include <boost/thread/mutex.hpp>


extern "C" 
{
#include <fixeda.h> // iFIX / DMACS header files
}

void IFixPDBOperation::SetErrorMesage(int index, std::string msg)
{
	std::string tagName = "OCCPOW_CKKP002_00" + std::to_string(static_cast<long long>(index));

	INT16 err;
	GNUM group = eda_define_group(1, 0);
	THANDLE	ntf = eda_define_ntf(group, const_cast<char *>(nodeName.c_str()),
		const_cast<char *>(tagName.c_str()),
		const_cast<char *>("a_cv"),
		NULL);

	eda_lookup(group);
	eda_wait(group);
	// float value_float;
	err = eda_set_ascii(group, ntf, const_cast<char *>(msg.c_str()));

	eda_write(group);
	eda_wait(group);

	eda_delete_ntf(group, ntf);
	eda_delete_group(group);
	BOOST_LOG_TRIVIAL(debug) << std::string("Write PDB") + "["+ tagName +"]:"+ msg;
}

void IFixPDBOperation::SetCheckResult(int result)
{
	WritePDBValue("OCCPOW_CKKP001_002", result);
}

void IFixPDBOperation::SetActoinResult(std::string node, int commandValue)
{
	std::string tagName = "OCCPOW_CKKP" + node + "_"+"666";
	WritePDBValue(tagName.c_str(), commandValue);
}

void IFixPDBOperation::SetActoinResult(std::string node, std::string actionClass, int typeId)
{
	std::string tagName = "OCCPOW_CKKP" + node + "_"+actionClass.substr(actionClass.length() - 3);
	std::string objectClass = actionClass.substr(0, 3);
	int commandValue = -100;
	if (objectClass == "DLQ")
	{
		if (typeId == EXECUTE_OFF)
		{
			commandValue = 1;
		}
		else if (typeId = EXECUTE_ON)
		{
			commandValue = 2;
		}
	}
	else if (objectClass == "DDK")
	{
		if (typeId == EXECUTE_OFF)
		{
			commandValue = 3;
		}
		else if (typeId = EXECUTE_ON)
		{
			commandValue = 4;
		}
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "设备类型错误";
		return;
	}
	WritePDBValue(tagName.c_str(), commandValue);
}

bool IFixPDBOperation::WritePDBValue(const char* tagName, float value) const 
{
	INT16 err;
	GNUM group = eda_define_group(1, 0);
	THANDLE	ntf = eda_define_ntf(group, const_cast<char *>(nodeName.c_str()),
		const_cast<char *>(tagName),
		const_cast<char *>("F_CV"),
		NULL);

//	eda_lookup(group);
	eda_lookup(group);
	eda_wait(group);
	// float value_float;
	err = eda_set_float(group, ntf, &value);

	eda_write(group);
	eda_wait(group);

	eda_delete_ntf(group, ntf);
	eda_delete_group(group);

	BOOST_LOG_TRIVIAL(debug) << std::string("Write PDB") + "["+ tagName +"]:"+ std::to_string(static_cast<long double>(value)) + " Error Number: " + std::to_string((long long)err);

	if (err == 0)
		return true;
	else
		return false;
}

bool IFixPDBOperation::ReadPDBValue( const char* tagName, int &res) const
{
	GNUM eh;
	THANDLE hT;
	INT16 err;
	float value;

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

		BOOST_LOG_TRIVIAL(debug) << std::string("Read PDB") + "["+ tagName +"]:"+ std::to_string(static_cast<long double>(value)) + " Error Number: " + std::to_string((long long)err);
		res = value;
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