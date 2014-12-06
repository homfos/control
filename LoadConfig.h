
#ifndef LOAD_CONFIG_H_
#define LOAD_CONFIG_H_


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <string>
#include <map>
#include <utility>

using std::pair;
using std::vector;
using std::map;
using std::string;

struct LoadConfig
{
	static std::string hostName;
	static std::string dbHost;
	static std::string userName;
	static std::string passWord;
	static std::string dbName;
	static std::string scadaName;
	static std::string scadaType;
	static std::string hangOnTag1;
	static std::string hangOnTag2;
	static std::string hangOnTag3;
	static bool parallelFlag;
	static int feedBackTimeout;
	static int commandTimeout;
	static std::string dbType;
	static int excuteOn;
	static int excuteOff;
	static int twoPointOffset;
	static int chooseCommandOffset;
	static int chooseFeedBackOffset;
	static int executeFeedBackOffset;
	static std::string operationArea;
	static std::string remoteControl;
	static std::string remoteFeedBack;
	static std::vector <std::vector <std::string> > allLineOn;
	static std::vector <std::vector <std::string> > interLockDetail;
	static vector <map<pair<string, string>, string>> tOrf;
	static int load(const std::string &filename);
	static int loadCardInfo();
private:
	static void load(const std::string fileName, std::vector<std::vector<std::string>> &actionInfo);
	static void loadtorf(const std::string fileName, vector<map<pair<string, string>, string>> &torf);
};
#endif
