#ifndef CONNECTION_H
#define CONNECTION_H
// Connection.h
#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include <string>
#include <vector>
#include <sqltypes.h>
#include <memory>
#include "Table.h"
#include "Error.h"
#include "Statement.h"
#include "HandleStatement.h"
//#include "ODBCConnectorAPI.h"

using namespace std;

//namespace ODBCConnectorAPI
//{
	class Connection
	{
	public:
		// Returns
		Connection(unsigned long odbcVersion);

		vector<string> readAllCatalogs();

		vector<string> readAllSchemas();

		vector<Table> readTablesForCatalogAndSchema(int catalogIndex, int schemaIndex);

		int readAllTables();

		//Table getTable(string tableName);

		bool hasError(int returnCode);

		//void setError(Error error);

		void addTable(Table table);

		HandleStatement* getHandleStatement();

		string getIdentifierQuote();

		short setIdentifierQuote();

		bool isConnected();

		string getNameOfObject(unsigned int index, int type);

		vector<string> getCatalogs();

		vector<string> getSchemas();

		vector<Table> getTables();

		Table getTable(unsigned int index);

		bool containsTable(string tableName);

		int connect();

		int disconnect();

		bool setBulkFetch(int bulkFetch);

		~Connection();

	private:
		//Error error;

		/// TURN INTO ENUM ////
		//static const int CATALOG = 0;

		//static const int SCHEMA = 1;

		//static const int TABLE = 2;

		// DO THESE NEED TO BE MEMBER VARIABLES ???
 		//ResultSet* catalogs;

		//ResultSet* schemas;

		vector<string> catalogs;
		
		bool connected;

		HandleStatement * handleStatement;

		string identifierQuote;

		vector<string> schemas;

		unsigned long odbcVersion;

		vector<Table> tables;

		//SQLCHAR* getSqlCharName(int index);

	};


//}

#endif