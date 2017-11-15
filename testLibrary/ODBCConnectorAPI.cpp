// ODBCConnectorAPI.cpp
// compile with: cl /c /EHsc ODBCConnectorAPI.cpp
// post-build command: lib ODBCConnectorAPI.obj

#include "ODBCConnectorAPI.h"
//#include "Connection.h"
//#include "Query.h"
#include "Statement.h"

#include <sqlext.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <thread>

using namespace std;

// Establishes connection to the datasource
int RDBC_Connection_Connect(HANDLE connection)
{
	if (isValidHandle(connection)){
		return ((Connection *) connection)->connect();
	}
	return 0;
}

// Add a table to the list of available tables
int RDBC_Connection_CopyTable(HANDLE sourceConnection, HANDLE targetConnection, int targetCatalogIndex, int targetSchemaIndex, int sourceTableIndex, int rowsAtATime, set<int> columnIndexes)
{
	if (isValidHandle(sourceConnection) && isValidHandle(targetConnection)){
		Connection * sourceConnectionPointer = (Connection *)sourceConnection;
		Connection * targetConnectionPointer = (Connection *)targetConnection;
		if (sourceConnectionPointer->isConnected() && targetConnectionPointer->isConnected()) {
			Table table = sourceConnectionPointer->getTable(sourceTableIndex);
			vector<Column> columns = getFilteredColumns(sourceConnectionPointer, table, columnIndexes);
			vector<string> primaryKey = table.readPrimaryKey(sourceConnectionPointer->getHandleStatement());
			string tableName = table.getSimpleName();

			Query* createQuery = createCopyCreateQuery(targetConnectionPointer, tableName, columns, primaryKey, targetCatalogIndex, targetSchemaIndex);
			Statement statement = Statement(targetConnectionPointer->getHandleStatement(), createQuery->getQueryString());
			statement.executeUpdate();

			sourceConnectionPointer->setBulkFetch(rowsAtATime);
			bool hasError = selectStarAndInsertIntoTarget(sourceConnectionPointer, targetConnectionPointer, table.getName(), createQuery->getTableName(), rowsAtATime, columns, columnIndexes);
			/**
			Query selectQuery = Query(SELECT_STAR, table.getName(), "");
			statement = Statement(sourceConnectionPointer->getHandleStatement(), selectQuery.getQueryString());
			ResultSet* resultSet = statement.execute();

			// TODO append columns to after table name and not the table name // IMPORTANT IMPORTANT
			insertResultSetIntoTargetTableNRowsAtATime(targetConnectionPointer, createQuery->getTableName(), resultSet, rowsAtATime, columns, columnIndexes);
			delete resultSet;*/
			delete createQuery;
			return hasError ? 1 : 0;
		}
	}
}

// Deletes a datasource connection object 
void RDBC_Connection_Delete(HANDLE connection)
{
	if (isValidHandle(connection)){
		Connection * connectionPointer = (Connection *)connection;
		delete connectionPointer;
	}
}

// Disconnects from the datasource
int RDBC_Connection_Disconnect(HANDLE connection)
{
	if (isValidHandle(connection)){
		return ((Connection *)connection)->disconnect();
	}
	return 0;
}

void RDBC_Connection_GetNameOfOdbcObject(HANDLE connection, int index, OdbcObject type, char* name) {
	if (isValidHandle(connection)){
		// Add a check for table size and fetch the tables hav not been read
		string nameAsString = ((Connection *)connection)->getNameOfObject(index, type);
		strcpy_s(name, nameAsString.size() + 1, nameAsString.c_str());
	}
}

int RDBC_Connection_InsertTable(HANDLE sourceConnection, HANDLE targetConnection, int sourceTableIndex, int targetTableIndex, int rowsAtATime, unordered_map<int, int> columnMap)
{
	if (isValidHandle(sourceConnection) && isValidHandle(targetConnection)){
		Connection * sourceConnectionPointer = (Connection *)sourceConnection;
		Connection * targetConnectionPointer = (Connection *)targetConnection;
		if (sourceConnectionPointer->isConnected() && targetConnectionPointer->isConnected()) {
			Table sourceTable = sourceConnectionPointer->getTable(sourceTableIndex);
			Table targetTable = targetConnectionPointer->getTable(targetTableIndex);
			vector<Column> columns = getMappedColumns(sourceConnectionPointer, targetConnectionPointer, sourceTable, targetTable, columnMap);
			set<int> columnIndexes;
			for (unordered_map<int, int>::iterator it = columnMap.begin(); it != columnMap.end(); ++it) {
				columnIndexes.insert(it->first);
			}

			bool hasError = selectStarAndInsertIntoTarget(sourceConnectionPointer, targetConnectionPointer, sourceTable.getName(), targetTable.getName(), rowsAtATime, columns, columnIndexes);
			/**
			Query selectQuery = Query(SELECT_STAR, sourceTable.getName(), "");
			Statement statement = Statement(sourceConnectionPointer->getHandleStatement(), selectQuery.getQueryString());
			ResultSet* resultSet = statement.execute();

			insertResultSetIntoTargetTableNRowsAtATime(targetConnectionPointer, targetTable.getName(), resultSet, rowsAtATime, columns, columnIndexes);
			delete resultSet;*/
			return hasError ? 1 : 0;
		}
	}
}

// Returns a new object of a datasource connection
HANDLE RDBC_Connection_New(unsigned long odbcVersion)
{
	return new Connection(odbcVersion);
}


// Returns table metadata from the connected database
int RDBC_Connection_ReadAllCatalogs(HANDLE connection, char colNames[][256])
{
	int numCatalogs = -1;
	if (isValidHandle(connection)){
		Connection * connectionPointer = (Connection *)connection;
		if (connectionPointer->isConnected()) {
			vector<string> catalogs = connectionPointer->readAllCatalogs();
			numCatalogs = catalogs.size();
			for (unsigned int i = 0; i < catalogs.size(); ++i) {
				string catalogName = catalogs[i];
				strcpy_s(colNames[i], catalogName.size() + 1, catalogName.c_str());
			}
		}
	}
	return numCatalogs;
}

// Returns table metadata from the connected database
int RDBC_Connection_ReadAllSchemas(HANDLE connection, char colNames[][256])
{
	int numSchemas = -1;
	if (isValidHandle(connection)){
		Connection * connectionPointer = (Connection *)connection;
		if (connectionPointer->isConnected()) {
			vector<string> schemas = connectionPointer->readAllSchemas();
			numSchemas = schemas.size();
			for (unsigned int i = 0; i < schemas.size(); ++i){
				string schemasName = schemas[i];
				strcpy_s(colNames[i], schemasName.size() + 1, schemasName.c_str());
			}
		}
	}
	return numSchemas;
}

// Returns table metadata from the connected database
int RDBC_Connection_ReadAllTables(HANDLE connection, char colNames[][256])
{
	int numTables = -1;
	if (isValidHandle(connection)){
		Connection * connectionPointer = (Connection *)connection;
		if (connectionPointer->isConnected()) {
			numTables = connectionPointer->readAllTables();
		}
	}
	return numTables;
}


int RDBC_Connection_ReadTablesForCatalogAndSchema(HANDLE connection, int catalogIndex, int schemaIndex, char colNames[][256])
{
	int numTables = -1;
	if (isValidHandle(connection)){
		Connection * connectionPointer = (Connection *)connection;
		if (connectionPointer->isConnected()) {
			vector<Table> tables = connectionPointer->readTablesForCatalogAndSchema(catalogIndex, schemaIndex);
			numTables = tables.size();
			for (int i = 0; i < numTables; ++i) {
				string tableName = tables[i].getSimpleName();
				strcpy_s(colNames[i], tableName.size() + 1, tableName.c_str());
			}
		}
	}
	return numTables;
}

int RDBC_Table_ReadAllColumns(HANDLE connection, int tableIndex, char colNames[][256]){
	int numColumns = -1;
	if (isValidHandle(connection)){
		Connection * connectionPointer = (Connection *)connection;
		if (connectionPointer->isConnected()) {
			Table table = connectionPointer->getTable(tableIndex);
			vector<Column> columns = table.readAllColumns(connectionPointer->getHandleStatement());
			numColumns = columns.size();
			for (int i = 0; i < numColumns; ++i) {
				string colName = columns[i].getName();
				strcpy_s(colNames[i], colName.size() + 1, colName.c_str());
			}
		}
	}
	return numColumns;
}


bool RDBC_Connection_SetBulkFetch(HANDLE connection, int bulkFetch) {
	if (isValidHandle(connection)){
		Connection * connectionPointer = (Connection *)connection;
		if (connectionPointer->isConnected()) {
			connectionPointer->setBulkFetch(bulkFetch);
		}
	}
	return false;
}


	// Returns any errors recorded during the datasource connection
//	Error RDBC_Connection_GetError()
//	{
//		return connection.getError();
//	}

	// Add a table to the list of available tables
//	void RDBC_Connection_AddTable(Table table)
//	{
		//connection.addTable(table);
//	}

	/**

	// Returns internal representation of the table type
	string RDBC_Table_GetType(){
		return "";
	}

	// Returns number of columns for a given table
	int RDBC_Table_CountColumns(){
		return 0;
	}

	// Returns internal representation of a column retrieved by a column index
	string RDBC_Table_GetColumn(int index){
		return "";
	}

	// Reads table name of a given table
	string RDBC_Table_GetName(){
		return "";
	}

	// Adds a column to a given table by providing the new column's metadata
	void RDBC_Table_AddColumn(Column column){
		return;
	}
	*/

	bool containsPrimaryKey(vector<string> primaryKey, vector<Column> columns) {
		int total = 0;
		for (unsigned int i = 0; i < primaryKey.size(); ++i) {
			string primaryKeyColumnName = primaryKey[i];
			for (unsigned int j = 0; j < columns.size(); ++j) {
				if (columns[i].getName().compare(primaryKeyColumnName) == 0){
					++total;
					break;
				}
			}
		}
		return total == primaryKey.size();
	}

	Query* createCopyCreateQuery(Connection * targetConnectionPointer, string oldTableName, vector<Column> columns, vector<string> primaryKey, int targetCatalogIndex, int targetSchemaIndex) {
		targetConnectionPointer->readTablesForCatalogAndSchema(targetCatalogIndex, targetSchemaIndex);
		string catalogName = targetConnectionPointer->getNameOfObject(targetCatalogIndex, CATALOG);
		string schemaName = targetConnectionPointer->getNameOfObject(targetSchemaIndex, SCHEMA);
		int tableNum = 0;
		string copyTableName = oldTableName + "copy" + to_string(tableNum);
		while (targetConnectionPointer->containsTable(copyTableName)) {
			tableNum++;
			copyTableName = oldTableName + "copy" + to_string(tableNum);
		}
		Query* createQuery = new Query(CREATE_TABLE, "", "", targetConnectionPointer->getIdentifierQuote());
		createQuery->addFullTableNameToQuery(catalogName, schemaName, copyTableName);
		if (!containsPrimaryKey(primaryKey, columns))
			primaryKey.clear();
		createQuery->addColumnsToCreateQuery(columns, primaryKey);
		return createQuery;
	}

	vector<Column> getFilteredColumns(Connection* sourceConnectionPointer, Table table, set<int> columnIndexes) {
		table.readAllColumns(sourceConnectionPointer->getHandleStatement());
		vector<Column> columns = table.getColumns();
		vector<Column> filteredColumns;
		if (columnIndexes.size() > 0){
			for (unsigned int i = 0; i < columns.size(); ++i){
				if (columnIndexes.find(i) != columnIndexes.end())
					filteredColumns.push_back(columns[i]);
			}
			return filteredColumns;
		}
		return columns;
	}

	vector<Column> getMappedColumns(Connection * sourceConnectionPointer, Connection * targetConnectionPointer, Table sourceTable, Table targetTable, unordered_map<int, int> columnMap) {
		vector<Column> sourceColumns = sourceTable.readAllColumns(sourceConnectionPointer->getHandleStatement());
		vector<Column> targetColumns = targetTable.readAllColumns(targetConnectionPointer->getHandleStatement());
		vector<Column> columns;
		for (unsigned int i = 0; i < sourceColumns.size(); ++i) {
			if (columnMap.find(i) != columnMap.end()) {
				columns.push_back(targetColumns[columnMap[i]]);
			}
		}
		return columns;
	}

	bool insertResultSetIntoTargetTableNRowsAtATime(Connection * targetConnectionPointer, string targetTable, ResultSet* resultSet, int rowsAtATime, vector<Column> columns, set<int> columnIndexes){
		Query insertQuery = Query(INSERT, targetTable, " VALUES ", targetConnectionPointer->getIdentifierQuote());
		insertQuery.addColumnsToInsertQuery(columns);
		bool moreRows = true;
		bool hasError = false;
		while (moreRows){
			vector<vector<string>>* currentRows = new vector<vector<string>>(rowsAtATime);
			moreRows = resultSet->getNRows(currentRows);
			if (currentRows->size() > 0) {
				insertQuery.addListOfValuesToInsertQuery2(currentRows, columnIndexes);
				Statement statement = Statement(targetConnectionPointer->getHandleStatement(), insertQuery.getInsertQueryString());
				bool currentError = statement.executeUpdate();
				if (currentError)
					hasError = true;
			}
			delete currentRows;
		}
		return hasError;
	}

	bool threadedInsertResultSetIntoTargetTableNRowsAtATime(Connection * targetConnectionPointer, string targetTable, ResultSet* resultSet, int rowsAtATime, vector<Column> columns, set<int> columnIndexes){
		bool hasError = false;
		MutexStruct* shared = new MutexStruct; 
		shared->areMoreRows = true;
		shared->hasError = false;
		shared->readBuff = new vector<vector<string>>();
		shared->writeBuff = new vector<vector<string>>();

		thread read(readThread, rowsAtATime, resultSet, shared);
		thread write(writeThread, targetConnectionPointer, targetTable, rowsAtATime, columns, columnIndexes, shared);

		read.join();
		write.join();
		hasError = shared->hasError;
		delete shared->readBuff;
		delete shared->writeBuff;
		delete shared;
		return hasError;
	}

	bool isValidHandle(HANDLE handle){
		// TODO
		return true;
	}

	bool selectStarAndInsertIntoTarget(Connection * sourceConnectionPointer, Connection * targetConnectionPointer, string sourceTable, string targetTable, int rowsAtATime, vector<Column> columns, set<int> columnIndexes){
		Query selectQuery = Query(SELECT_STAR, sourceTable, "", sourceConnectionPointer->getIdentifierQuote());
		Statement statement = Statement(sourceConnectionPointer->getHandleStatement(), selectQuery.getQueryString());
		ResultSet* resultSet = statement.execute();

		// TODO append columns to after table name and not the table name // IMPORTANT IMPORTANT
		bool hasError;
		if (rowsAtATime < 500){
			hasError = insertResultSetIntoTargetTableNRowsAtATime(targetConnectionPointer, targetTable, resultSet, rowsAtATime, columns, columnIndexes);
		}
		else {
			hasError = threadedInsertResultSetIntoTargetTableNRowsAtATime(targetConnectionPointer, targetTable, resultSet, rowsAtATime, columns, columnIndexes);
		}
		
		delete resultSet;
		return hasError;
	}

	bool swap(vector<vector<string>>** readRows, vector<vector<string>>** writeRows) {
		// TODO: LOCK
		vector<vector<string>>* tempRows = *readRows;
		*readRows = *writeRows;
		*writeRows = tempRows;
		return true;
	}

	void readThread(int rowsAtATime, ResultSet* resultSet, MutexStruct* shared)
	{
		//// Get rows /////////
		// Set a flag when no more rows
		//bool moreRows = true;
		int i;
		
		while (shared->areMoreRows){
			delete shared->readBuff;
			shared->readBuff = new vector<vector<string>>(rowsAtATime);
			shared->areMoreRows  = resultSet->getNRows(shared->readBuff);

			unique_lock<mutex> l(shared->lock);
			shared->not_full.wait(l, [shared]{return shared->writeBuff->size() == 0; });
			swap(&shared->readBuff, &shared->writeBuff);

			l.unlock();
			shared->not_empty.notify_one();
		}
	}

	void writeThread(Connection * targetConnectionPointer, string targetTable, int rowsAtATime, vector<Column> columns, set<int> columnIndexes, MutexStruct* shared)
	{
		Query insertQuery = Query(INSERT, targetTable, " VALUES ", targetConnectionPointer->getIdentifierQuote());
		insertQuery.addColumnsToInsertQuery(columns);
		while (shared->areMoreRows || shared->writeBuff->size() > 0) {
			unique_lock<mutex> l(shared->lock);
			shared->not_empty.wait(l, [shared]{ return shared->writeBuff->size() != 0; });

			insertQuery.addListOfValuesToInsertQuery2(shared->writeBuff, columnIndexes);
			shared->writeBuff->clear();
			l.unlock();
			shared->not_full.notify_one();

			Statement statement = Statement(targetConnectionPointer->getHandleStatement(), insertQuery.getInsertQueryString());
			bool currentError = statement.executeUpdate();
			if (currentError)
				shared->hasError = true;
		}
	}