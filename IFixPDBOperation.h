#ifndef IFIX_PDB_OPERATION_H_
#define IFIX_PDB_OPERATION_H_

class  IFixPDBOperation
{

private:
	std::string nodeName;

public:
	 ~IFixPDBOperation();
	IFixPDBOperation(std::string );

	 bool WritePDBValue(const char* tagName, float value) const;
	 bool ReadPDBValue(const char* tagName, int &) const;
	 bool ReadPDBValueNoLog(const char* tagName, int &) const;
	 void SetCheckResult(int result);
	 void SetActoinResult(std::string node, std::string actionClass, int type);
	 void SetActoinResult(std::string node, int type);
	 void SetErrorMesage(int index, std::string msg);
};


#endif