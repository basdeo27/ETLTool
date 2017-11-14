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
	class Statement
	{
	public:
		// Returns
		//Statement(HandleStatement* handleStatement, Query query);

		Statement(HandleStatement* handleStatement, string queryString);
		//Statement(Connection* connectionHandle, string queryString);

		void beginTransaction();
		
		ResultSet* execute();

		bool executeUpdate();

		bool executeInsert();

		~Statement();

	private:
		//Query query;
		//Connection * aConnectionHandle;
		HandleStatement* handleStatement;

		string queryString;
	};
//}

#endif