#include "LoadConfig.h"
#include <SQLAPI.h>
#include <string>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/thread.hpp>


#ifndef DBOPERATION_H_
#define DBOPERATION_H_

class DbOperation
{
public:

	DbOperation();
	~DbOperation();

	static vector <vector <string> > allLineOn,allLineOff, upMoveOn, upMoveOff, downMoveOn, downMoveOff;
	static map<int, vector<string>> interLockDetail;
	static map<pair<int, int>, string> tOrf;

	static void connect_db()
	{
		if (con.isConnected())
			return;

		try
		{
			BOOST_LOG_TRIVIAL(debug) << "连接数据库类型：" << LoadConfig::dbType << " 数据库名字：" << LoadConfig::dbName << " 用户名：" << LoadConfig::userName << " 密码:"<<LoadConfig::passWord;

			con.Connect(LoadConfig::dbName.c_str(), 
				LoadConfig::userName.c_str(), 
				LoadConfig::passWord.c_str(), SA_Oracle_Client);
			SACommand cmd1(&con, "alter session set NLS_LANGUAGE= 'SIMPLIFIED CHINESE'");
			cmd1.Execute();
			SACommand cmd2(&con, "alter session set NLS_TERRITORY = 'CHINA'");
			cmd2.Execute();
			BOOST_LOG_TRIVIAL(info) << "数据库连接成功!";
		}
		catch (SAException &x)
		{
			try
			{
				con.Rollback();
			}
			catch(SAException &)
			{
				BOOST_LOG_TRIVIAL(error) << x.ErrText();
			}
			BOOST_LOG_TRIVIAL(error) << x.ErrText();
			return;
		}
	}
	static SAConnection& get_connection() { return con; }
	static void LoadActionInfo();

private:
	static SAConnection con; 
	static void LoadActoinInfo(vector <vector <string> > &content, string cardId);
	static void LoadInterLock();
	static void LoadInterLockResult();
};

#endif
