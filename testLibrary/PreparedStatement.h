#ifndef STATEMENT_H
#define STATEMENT_H
// Statement.h
#include <string>
//#include "Connection.h"
#include "ResultSet.h"
#include "HandleStatement.h"
//#include "Query.h"

using namespace std;

//namespace ODBCConnectorAPI
//{
class PreparedStatement
{
public:
	// Returns
	//Statement(HandleStatement* handleStatement, Query query);

	PreparedStatement(HandleStatement* handleStatement, string queryString);

	ResultSet* execute();

	void executeUpdate();

	void executeInsert();

	~PreparedStatement();

private:
	//Query query;
	//Connection * aConnectionHandle;
	HandleStatement* handleStatement;

	string queryString;
};
//}

#endif