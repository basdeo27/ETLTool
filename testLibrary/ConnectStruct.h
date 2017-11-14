#ifndef CONNECTSTRUCT_H
#define CONNECTSTRUCT_H
// Connection.h
#include <string>
#include <vector>

using namespace std;

//namespace ODBCConnectorAPI
//{
	class ConnectStruct
	{
	public:
		// Returns
		ConnectStruct();

		string getConnectString();

		string getParameter(int index);

		void setParameter(int index, string value);

		string getSqlString();

		~ConnectStruct();

	private:
		string connectString;

		vector<string> parameters;

		vector<string> sqlStrings;
	};
//}

#endif