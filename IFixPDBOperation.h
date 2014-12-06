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
	 bool ReadPDBValue(const char* tagName, float &) const;
	 void SetCheckResult(int result);
};


#endif