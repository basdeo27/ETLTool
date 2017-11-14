// Statement.
#include "Statement.h"
#include "Error.h"
#include <string>
#include <iostream>

using namespace std;

/**
	Statement::Statement(HandleStatement* handleStatement, Query query) :
		handleStatement(handleStatement),
		query(query)
	{
	}
	*/

	//Statement::Statement(Connection* connectionHandle, string queryString) :
	Statement::Statement(HandleStatement* handleStatement, string queryString) :
		//connectionHandle(connectionHandle),
		handleStatement(handleStatement),
		queryString(queryString)
	{

	}


	ResultSet* Statement::execute() {
		int returnCode = SQLExecDirectA(handleStatement->getHandle(), (SQLCHAR *) queryString.c_str(), SQL_NTS);
		handleStatement->printErrorIfExists(returnCode);
		return new ResultSet(handleStatement);
	}

	bool Statement::executeUpdate() {
		int returnCode = SQLExecDirectA(handleStatement->getHandle(), (SQLCHAR *) queryString.c_str(), SQL_NTS);
		bool error = handleStatement->printErrorIfExists(returnCode).compare("") != 0;
		SQLFreeStmt(handleStatement->getHandle(), SQL_CLOSE);
		return error;
	}

	Statement::~Statement()
	{

	}
