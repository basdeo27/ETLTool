#pragma once
// ODBCConnectorAPI.h

#ifdef ODBCCONNECTORAPI_H
#define ODBCCONNECTOR_API __declspec(dllexport)
#else
#define ODBCCONNECTOR_API _declspec(dllimport)
#endif

#include "Connection.h"
#include "Query.h"

#include <string>
#include <set>
#include <unordered_map>

using namespace std;

typedef void * HANDLE;

enum OdbcObject { CATALOG = 0, SCHEMA = 1, TABLE = 2, TABLETTYPE = 3 };

// Establishes connection to the datasource
extern "C" ODBCCONNECTOR_API int RDBC_Connection_Connect(HANDLE connection);

extern "C" ODBCCONNECTOR_API int RDBC_Connection_CopyTable(HANDLE sourceConnection, HANDLE targetConnection, int targetCatalogIndex, int targetSchemaIndex, int sourcetableIndex, int rowsAtATime, set<int> columnIndexes);

// Deletes a datasource connection object 
extern "C" ODBCCONNECTOR_API void RDBC_Connection_Delete(HANDLE connection);

// Disconnects from the datasource
extern "C" ODBCCONNECTOR_API int RDBC_Connection_Disconnect(HANDLE connection);

extern "C" ODBCCONNECTOR_API const char* RDBC_Connection_GetCatalog(HANDLE connection, int index);

extern "C" ODBCCONNECTOR_API void RDBC_Connection_GetNameOfOdbcObject(HANDLE connection, int index, OdbcObject type, char* name);

extern "C" ODBCCONNECTOR_API const char* RDBC_Connection_GetSchema(HANDLE connection, int index);

extern "C" ODBCCONNECTOR_API const char* RDBC_Connection_GetTable(HANDLE connection, int index);

extern "C" ODBCCONNECTOR_API int RDBC_Connection_InsertTable(HANDLE sourceConnection, HANDLE targetConnection, int sourceTableIndex, int targetTableIndex, int rowsAtATime, unordered_map<int, int> columnMap);

// Returns a new object of a datasource connection
extern "C" ODBCCONNECTOR_API HANDLE RDBC_Connection_New(unsigned long odbcVersion);

extern "C" ODBCCONNECTOR_API void RDBC_Connection_PrintAllCatalogs(HANDLE connection);

extern "C" ODBCCONNECTOR_API void RDBC_Connection_PrintAllSchemas(HANDLE connection);

extern "C" ODBCCONNECTOR_API void RDBC_Connection_PrintAllTables(HANDLE connection);

extern "C" ODBCCONNECTOR_API int RDBC_Connection_ReadAllCatalogs(HANDLE connection, char colNames[][256]);

extern "C" ODBCCONNECTOR_API int RDBC_Connection_ReadAllSchemas(HANDLE connection, char colNames[][256]);

// Returns table metadata from the connected database
extern "C" ODBCCONNECTOR_API int RDBC_Connection_ReadAllTables(HANDLE connection, char colNames[][256]);

extern "C" ODBCCONNECTOR_API int RDBC_Connection_ReadTablesForCatalogAndSchema(HANDLE connection, int catalogIndex, int schemaIndex, char colNames[][256]);

//extern "C" ODBCCONNECTOR_API bool RDBC_Connection_SetBulkFetch(HANDLE connection, int bulkFetch);

extern "C" ODBCCONNECTOR_API int RDBC_Table_ReadAllColumns(HANDLE connection, int tableIndex, char colNames[][256]);

//extern "C" ODBCCONNECTOR_API void RDBC_Table_PrintAllColumns(HANDLE connection, int tableIndex);

// Returns current table metadata

// Returns any errors recorded during the datasource connection
//ODBCCONNECTOR_API static Error RDBC_Connection_GetError();

// Add a table to the list of available tables
//ODBCCONNECTOR_API void RDBC_Connection_AddTable(Table table);

/////////////// PRIVATE //////////////////
struct ReadParameters {
	int rowsAtATime;
	ResultSet* resultSet;
};

struct WriteParameters {
	Connection * targetConnectionPointer;
	string targetTable;
	int rowsAtATime;
	vector<Column> columns;
	set<int> columnIndexes;
};

struct MutexStruct {
	bool stillReading, stillWriting, areMoreRows, hasError;
	vector<vector<string>> *writeBuff, *readBuff;
};

Query* createCopyCreateQuery(Connection * targetConnectionPointer, string oldTableName, vector<Column> columns, vector<string> primaryKey, int targetCatalogIndex, int targetSchemaIndex);

vector<Column> getFilteredColumns(Connection* sourceConnectionPointer, Table table, set<int> columnIndexes);

vector<Column> getMappedColumns(Connection * sourceConnectionPointer, Connection * targetConnectionPointer, Table sourceTable, Table targetTable, unordered_map<int, int> columnMap);

bool insertResultSetIntoTargetTableNRowsAtATime(Connection * targetConnectionPointer, string targetTable, ResultSet* resultSet, int rowsAtATime, vector<Column> columns, set<int> columnIndexes);

bool isValidHandle(HANDLE handle);

bool selectStarAndInsertIntoTarget(Connection * sourceConnectionPointer, Connection * targetConnectionPointer, string sourceTable, string targetTable, int rowsAtATime, vector<Column> columns, set<int> columnIndexes);

bool threadedInsertResultSetIntoTargetTableNRowsAtATime(Connection * targetConnectionPointer, string targetTable, ResultSet* resultSet, int rowsAtATime, vector<Column> columns, set<int> columnIndexes);

void readThread(int rowsAtATime, ResultSet* resultSet, MutexStruct* shared);

void writeThread(Connection * targetConnectionPointer, string targetTable, int rowsAtATime, vector<Column> columns, set<int> columnIndexes, MutexStruct* shared);