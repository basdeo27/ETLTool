// Connection.cpp
#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include "Connection.h"
#include "ODBCConnectorAPI.h"
#include "HandleStatement.h"

//#include <sqltypes.h>
//#include <sqlext.h>
#include <stdexcept>
#include <string>
#include <memory>
#include <vector>
#include <iostream>

#define SQL_NOUNICODEMAP 1

//namespace ODBCConnectorAPI
//{

	Connection::Connection(unsigned long odbcVersion) :
		odbcVersion(odbcVersion)
	{
		handleStatement = new HandleStatement();
		connected = false;
	}

	int Connection::connect(){
		int returnCode;
		long attr_odbc_version = 200;
		returnCode = handleStatement->allocateEnvironmentHandle();
		returnCode = handleStatement->setEnvironmentVariable(attr_odbc_version, odbcVersion);
		returnCode = handleStatement->allocateConnectionHandle();
		//returnCode = handleStatement->driverConnect();
		returnCode = handleStatement->hiveDriverConnect();
		handleStatement->printErrorIfExists(returnCode);
		returnCode = handleStatement->allocateStatementHandle();
		returnCode = setIdentifierQuote();
		connected = returnCode == 0 ? true : false;
//		handleStatement->printErrorIfExists(returnCode);
		return connected;
	}

	short Connection::setIdentifierQuote(){
		SQLSMALLINT strLenPointer = 32;
		SQLCHAR* infoValuePtr = (SQLCHAR*)malloc(sizeof(strLenPointer));
		int returnCode = SQLGetInfoA(handleStatement->getConn(), SQL_IDENTIFIER_QUOTE_CHAR, infoValuePtr, 128, &strLenPointer);
		identifierQuote = string((char *)infoValuePtr);
		free(infoValuePtr);
		return returnCode;
	}

	string Connection::getIdentifierQuote(){
		return identifierQuote;
	}
	
	bool Connection::hasError(int returnCode){
		return handleStatement->printErrorIfExists(returnCode).compare("") != 0;
	}

	/**
	void Connection::setError(Error newError){
		error = newError;
	}
	*/

	void Connection::addTable(Table table){
		tables.push_back(table);
		// Implement adding a table to the server //
	}

	HandleStatement* Connection::getHandleStatement(){
		return handleStatement;
	}

	vector<string> Connection::readAllCatalogs() {
		catalogs.clear();
		int returnCode = SQLTablesA(handleStatement->getHandle(), (SQLCHAR*) "%", SQL_NTS, (SQLCHAR*)"", SQL_NTS, (SQLCHAR*)"", SQL_NTS, (SQLCHAR*)"", SQL_NTS);
		handleStatement->printErrorIfExists(returnCode);
		ResultSet resultSet = ResultSet(handleStatement);
		while (resultSet.getNextRow())
			if ((resultSet.getCurrentRow())[CATALOG].compare("") != 0)
				catalogs.push_back((resultSet.getCurrentRow())[CATALOG]);

		return catalogs;
	}

	vector<string> Connection::readAllSchemas() {
		schemas.clear();
		int returnCode = SQLTablesA(handleStatement->getHandle(), (SQLCHAR*) "", SQL_NTS, (SQLCHAR*)"%", SQL_NTS, (SQLCHAR*)"", SQL_NTS, (SQLCHAR*)"", SQL_NTS);
		ResultSet resultSet = ResultSet(handleStatement);
		while (resultSet.getNextRow())
			if ((resultSet.getCurrentRow())[SCHEMA].compare("") != 0)
				schemas.push_back((resultSet.getCurrentRow())[SCHEMA]);
		
		return schemas;
	}

	vector<Table> Connection::readTablesForCatalogAndSchema(int catalogIndex, int schemaIndex) {
		tables.clear();
		string catalogName = getNameOfObject(catalogIndex, CATALOG);
		SQLCHAR* sqlCatalogName = catalogName.size() != 0 ? (SQLCHAR*)catalogName.c_str() : NULL;

		string schemaName = getNameOfObject(schemaIndex, SCHEMA);
		SQLCHAR* sqlSchemaName = schemaName.size() != 0 ? (SQLCHAR*)schemaName.c_str() : NULL;

		int returnCode = SQLTablesA(handleStatement->getHandle(), sqlCatalogName, SQL_NTS, sqlSchemaName, SQL_NTS, NULL, 0, NULL, 0);
		handleStatement->printErrorIfExists(returnCode);
		ResultSet resultSet = ResultSet(handleStatement);
		while (resultSet.getNextRow()){
			vector<string> currentRow = resultSet.getCurrentRow();
			tables.push_back(Table(currentRow[CATALOG], currentRow[SCHEMA], currentRow[TABLE], currentRow[TABLETTYPE], identifierQuote));
		}
		
		return tables;
	}


	int Connection::readAllTables() {
		tables.clear();
		int returnCode = SQLTablesA(handleStatement->getHandle(), NULL, SQL_NTS, NULL, SQL_NTS, NULL, 0, NULL, 0);
		handleStatement->printErrorIfExists(returnCode);
		ResultSet resultSet = ResultSet(handleStatement);
		while (resultSet.getNextRow()){
			vector<string> currentRow = resultSet.getCurrentRow();
			tables.push_back(Table(currentRow[CATALOG], currentRow[SCHEMA], currentRow[TABLE], currentRow[TABLETTYPE], identifierQuote));
		}

		return tables.size();
	}

	string Connection::getNameOfObject(unsigned int index, int type){
		switch (type){
		case CATALOG:
			//vector<vector<string>> catalogResultSet = catalogs->getResultSetString();
			if (index < catalogs.size() && index >= 0){
				return catalogs[index];
			}
			break;
		case SCHEMA:
			//vector<vector<string>> schemaResultSet = schemas->getResultSetString();
			if (index < schemas.size() && index >= 0){
				return schemas[index];
			}
			break;
		case TABLE:
			if (index < tables.size() && index >= 0){
				return tables[index].getName();
			}
		default:
			break;
		}
		return "";
	}

	vector<string> Connection::getCatalogs(){
		return catalogs;
	}

	vector<string> Connection::getSchemas(){
		return schemas;
	}

	vector<Table> Connection::getTables(){
		return tables;
	}

	Table Connection::getTable(unsigned int index) {
		if (index < tables.size() && index >= 0){
			return tables[index];
		}
		//return "";
	}

	bool Connection::containsTable(string tableName) {
		for (unsigned int i = 0; i < tables.size(); i++){
			if (string(tables[i].getSimpleName()).compare(tableName) == 0){
				return true;
			}
		}
		return false;
	}

	bool Connection::setBulkFetch(int bulkFetch) {
		int retCode = SQLSetStmtAttr(handleStatement->getHandle(), SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER) bulkFetch, 0);
		handleStatement->setBulkFetch(bulkFetch);
		return retCode == 0;
	}

	bool Connection::isConnected(){
		if (connected)
			return true;
		cerr << "Handle is not connected" << endl;
		return false;
	}

	int Connection::disconnect(){
		handleStatement->releaseStatementHandle();
		handleStatement->releaseConnectionHandle();
		handleStatement->releaseEnvironmentHandle();
		connected = false;
		return 1;
	}

	Connection::~Connection()
	{
		delete handleStatement;
/**		if (catalogs)
			delete catalogs;
		if (schemas)
			delete schemas;*/
	}
//}

	//////////////////////////PRIVATE////////////////////

