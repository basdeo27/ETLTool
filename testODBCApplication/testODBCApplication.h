#pragma once
// testODBCApplication.cpp : Defines the entry point for the console application.
#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include "ODBCConnectorAPI.h"

extern "C" {

	typedef ODBCCONNECTOR_API int(* Connect)(HANDLE);

	typedef ODBCCONNECTOR_API int(*CopyTable)(HANDLE, HANDLE, int, int, int, int, set<int>);

	typedef ODBCCONNECTOR_API void(* DeleteConnection)(HANDLE);

	typedef ODBCCONNECTOR_API int(* Disconnect)(HANDLE);

	typedef ODBCCONNECTOR_API const char*(* GetCatalog)(HANDLE, int);

	typedef ODBCCONNECTOR_API const char*(* GetNameOfOdbcObject)(HANDLE, int, OdbcObject, char*);

	typedef ODBCCONNECTOR_API const char*(* GetSchema)(HANDLE, int);

	typedef ODBCCONNECTOR_API const char*(* GetTable)(HANDLE, int);

	typedef ODBCCONNECTOR_API int(*InsertTable)(HANDLE, HANDLE, int, int, int, unordered_map<int, int>);

	typedef ODBCCONNECTOR_API HANDLE(*NewConnection)(unsigned long);

	typedef ODBCCONNECTOR_API void(* PrintAllCatalogs)(HANDLE);

	typedef ODBCCONNECTOR_API void(*PrintAllColumns)(HANDLE, int);

	typedef ODBCCONNECTOR_API void(* PrintAllSchemas)(HANDLE);

	typedef ODBCCONNECTOR_API void(* PrintAllTables)(HANDLE);

	typedef ODBCCONNECTOR_API int(*ReadAllCatalogs)(HANDLE, char(*)[256]);

	typedef ODBCCONNECTOR_API int(*ReadAllColumns)(HANDLE, int, char(*)[256]);

	typedef ODBCCONNECTOR_API int(*ReadAllSchemas)(HANDLE, char(*)[256]);

	typedef ODBCCONNECTOR_API int(*ReadAllTables)(HANDLE, char(*)[256]);

	typedef ODBCCONNECTOR_API int(*ReadTablesForCatalogAndSchema)(HANDLE, int, int, char(*)[256]);

	typedef ODBCCONNECTOR_API bool(*SetBulkFetch)(HANDLE, int);

}

HANDLE sourceConnection;

HANDLE targetConnection;

Connect ConnectPtr = NULL;

CopyTable CopyTablePtr = NULL;

DeleteConnection DeleteConnectionPtr = NULL;

Disconnect DisconnectPtr = NULL;

GetCatalog GetCatalogPtr = NULL;

GetNameOfOdbcObject GetNameOfOdbcObjectPtr = NULL;

GetSchema GetSchemaPtr = NULL;

GetTable GetTablePtr = NULL;

InsertTable InsertTablePtr = NULL;

NewConnection NewConnectionPtr = NULL;

PrintAllCatalogs PrintAllCatalogsPtr = NULL;

PrintAllColumns PrintAllColumnsPtr = NULL;

PrintAllSchemas PrintAllSchemasPtr = NULL;

PrintAllTables PrintAllTablesPtr = NULL;

ReadAllCatalogs ReadAllCatalogsPtr = NULL;

ReadAllColumns ReadAllColumnsPtr = NULL;

ReadAllSchemas ReadAllSchemasPtr = NULL;

ReadTablesForCatalogAndSchema ReadTablesForCatalogAndSchemaPtr = NULL;

SetBulkFetch SetBulkFetchPtr = NULL;


// Returns any errors recorded during the datasource connection
//ODBCCONNECTOR_API static Error RDBC_Connection_GetError();

// Add a table to the list of available tables
//ODBCCONNECTOR_API void RDBC_Connection_AddTable(Table table);
