// Statement.
#include "PreparedStatement.h"
#include <string>
#include <iostream>

using namespace std;


//Statement::Statement(Connection* connectionHandle, string queryString) :
PreparedStatement::PreparedStatement(HandleStatement* handleStatement, string queryString) :
	handleStatement(handleStatement),
	queryString(queryString)
{

}


ResultSet* PreparedStatement::execute() {
	int returnCode = SQLExecDirectA(handleStatement->getHandle(), (SQLCHAR *)queryString.c_str(), SQL_NTS);
	handleStatement->printErrorIfExists(returnCode);
	return new ResultSet(handleStatement);
}

void PreparedStatement::executeUpdate() {
	int returnCode = SQLExecDirectA(handleStatement->getHandle(), (SQLCHAR *)queryString.c_str(), SQL_NTS);
	handleStatement->printErrorIfExists(returnCode);
	SQLFreeStmt(handleStatement->getHandle(), SQL_CLOSE);
}

PreparedStatement::~PreparedStatement()
{

}