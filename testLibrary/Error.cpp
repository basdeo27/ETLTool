// Error.cpp

#include "Error.h"
#include <string>
#include <iostream>

using namespace std;


	Error::Error(HandleStatement* handleStatement) :
		handleStatement(handleStatement)
	{

	}

	//Error::Error(){

	//}

string Error::extractError()
	{
		SQLINTEGER i = 0;
		SQLSMALLINT type = SQL_HANDLE_STMT;
		SQLINTEGER native;
		SQLCHAR state[7];
		SQLCHAR text[256];
		SQLSMALLINT len;
		SQLRETURN ret;
		do {
			ret = SQLGetDiagRec(SQL_HANDLE_STMT, handleStatement->getHandle(), ++i, state, &native, text, sizeof(text), &len);
			if (SQL_SUCCEEDED(ret))
				cout << state << ":" << i << ":" << native << ":" << text << endl;
		} while (ret == SQL_SUCCESS);
		string returnedState((const char*)state);
		return returnedState;
	}

	Error::~Error()
	{

	}
