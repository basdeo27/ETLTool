// ODBCConnectorAPIUtils.cpp

#include "ODBCConnectorAPIUtils.h"
#include <string>
#include <vector>
#include <sqlext.h>

using namespace std;

//namespace ODBCConnectorAPI
//{
/**
	vector<struct DataBinding> allocateResults(int columns) {
		vector<struct DataBinding> results(columns);
		for (int i = 0; i < columns; i++) {
			results[i].TargetType = SQL_C_CHAR;
			results[i].BufferLength = (BUFFER_LENGTH + 1);
			results[i].TargetValuePtr = shared_ptr<char>{ new char[BUFFER_LENGTH] };
		}
		return results;
	}

	int MySQLSuccess(SQLRETURN rc) {
		return (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO);
	}
	
	bool isValidHandle(HANDLE handle){
		// TODO
		return true;
	}

	*/

