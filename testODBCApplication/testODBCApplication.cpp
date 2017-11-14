// testODBCApplication.cpp : Defines the entry point for the console application.
#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include "testODBCApplication.h"
#include "stdafx.h"
#include "ODBCConnectorAPI.h"
//#include <stdlib.h>
#include <iostream>
#include <set>
#include <unordered_map>
//#include <sqltypes.h>
#include <sqlext.h>
#include <numeric>



using namespace std;


	bool isUserExiting(string inputAnswer){
		return inputAnswer.compare("e") == 0;
	}

	bool userSaidYes(string inputAnswer){
		return inputAnswer.compare("y") == 0;
	}

	bool isValidIndex(int numTables, int tableIndex) {
		return tableIndex >= 0 && tableIndex < numTables;
	}

	void isUserReadyToChooseDatabase(string databaseType){
		string inputAnswer;
		while (!userSaidYes(inputAnswer)) {
			cout << "Press y to choose your " << databaseType << " database and e to exit the program" << endl;
			getline(cin, inputAnswer);
			if (isUserExiting(inputAnswer))
				exit(EXIT_SUCCESS);
		}
		//cin.clear();
		//cin.ignore(INT_MAX, '\n');
	}

	int userChooseIndex(HANDLE connection, int totalOptions, string type, OdbcObject odbcObject){
		int index = -1;
		string inputAnswer;
		while (!userSaidYes(inputAnswer)) {
			cout << "Enter a number to choose a "<< type << endl;
			if (cin >> index && isValidIndex(totalOptions, index)){
				char name[256];
				GetNameOfOdbcObjectPtr(connection, index, odbcObject, name);
				cout << "You have slected: " << name << endl;
				cout << "Press y to confirm " << type << " selection" << endl;
				cin.ignore();
				getline(cin, inputAnswer);
			}
			else {
				cout << "Please select a valid " << type << "" << endl;
				cin.clear();
				cin.ignore(INT_MAX, '\n');
			}
		}
		return index;
	}

	set<int> parseColumns(int numColumns, string columns){
		set<int> columnIndexes;
		//if (columns.compare("all") == 0) {
		//	for (unsigned int i = 0; i < numColumns; ++i)
		//		columnIndexes.insert(i);
		//}
		int newIndex = 0, currentIndex = 0;
		while (newIndex != -1){
			newIndex = columns.find(",", currentIndex);
			string column = columns.substr(currentIndex, newIndex - currentIndex);
			try {
				int columnInt = stoi(column);
				if (columnInt < 0 || columnInt > numColumns)
					return set<int>();
				columnIndexes.insert(columnInt);
			}
			catch (invalid_argument e){
				return set<int>();
			}
			currentIndex = newIndex + 1;
		}
		return columnIndexes;
	}

	void print2DArray(int n, char names[][256]){
		for (int i = 0; i < n; i++){
			cout << i << ". " << names[i] << endl;
		}
	}

	vector<string> printFiltered2DArray(int n, char names[][256], set<int> indexes){
		vector<string> updatedColumnList;
		for (int i = 0, j = 0; i < n; i++){
			if (indexes.find(i) != indexes.end()){
				updatedColumnList.push_back(names[i]);
				cout << j << ". " << names[i] << endl;
				j++;
			}
		}
		return updatedColumnList;
	}

	void assignFunctionPointers(HINSTANCE dllHandle){
		ConnectPtr = (Connect)GetProcAddress(dllHandle, "RDBC_Connection_Connect");
		CopyTablePtr = (CopyTable) GetProcAddress(dllHandle, "RDBC_Connection_CopyTable");
		DeleteConnectionPtr = (DeleteConnection) GetProcAddress(dllHandle, "RDBC_Connection_DeleteConnection");
		DisconnectPtr = (Disconnect)GetProcAddress(dllHandle, "RDBC_Connection_Disconnect");
		GetCatalogPtr = (GetCatalog) GetProcAddress(dllHandle, "RDBC_Connection_GetCatalog");
		GetNameOfOdbcObjectPtr = (GetNameOfOdbcObject) GetProcAddress(dllHandle, "RDBC_Connection_GetNameOfOdbcObject");
		GetSchemaPtr = (GetSchema) GetProcAddress(dllHandle, "RDBC_Connection_GetSchema");
		GetTablePtr = (GetTable) GetProcAddress(dllHandle, "RDBC_Connection_GetTable");
		InsertTablePtr = (InsertTable)GetProcAddress(dllHandle, "RDBC_Connection_InsertTable");
		NewConnectionPtr = (NewConnection) GetProcAddress(dllHandle, "RDBC_Connection_New");
		ReadAllCatalogsPtr = (ReadAllCatalogs)GetProcAddress(dllHandle, "RDBC_Connection_ReadAllCatalogs");
		ReadAllColumnsPtr = (ReadAllColumns) GetProcAddress(dllHandle, "RDBC_Table_ReadAllColumns");
		ReadAllSchemasPtr = (ReadAllSchemas) GetProcAddress(dllHandle, "RDBC_Connection_ReadAllSchemas");
		ReadTablesForCatalogAndSchemaPtr = (ReadTablesForCatalogAndSchema)GetProcAddress(dllHandle, "RDBC_Connection_ReadTablesForCatalogAndSchema");

	}

	int _tmain(int argc, _TCHAR* argv[])
	{
		string inputAnswer;
		cout << "Hello and welcome to BitQuill ETL :)" << endl;
		isUserReadyToChooseDatabase("source");
		///////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////LOAD DYNAMIC LINK LIBRARY///////////////////////////////////////
		BOOL freeResult, runTimeLinkSuccess = FALSE;
		HINSTANCE dllHandle = NULL;
		int sourceIsConnected, targetIsConnected;

		//Load the dll and keep the handle to it
		dllHandle = LoadLibraryA((LPCSTR) "C:\\Users\\dylanbasdeo\\Documents\\testLibrary\\Debug\\testLibrary.dll");

		// If the handle is valid, try to get the function address. 
		if (NULL != dllHandle)
		{
			//Get pointer to our function using GetProcAddress:
			assignFunctionPointers(dllHandle);
			
			// If the function address is valid, call the function. 
			if (NewConnectionPtr && ConnectPtr)
			{
				sourceConnection = NewConnectionPtr(SQL_OV_ODBC3_80);
				sourceIsConnected = ConnectPtr(sourceConnection);
			}
			if (!sourceIsConnected){
				cout << "Program failed to connect to source -- Exiting" << endl;
				exit(EXIT_SUCCESS);
			}

			int numTables = 0;
			int tableIndex = -1;
			if (ReadAllCatalogsPtr && ReadAllSchemasPtr && ReadTablesForCatalogAndSchemaPtr
				&& CopyTablePtr && ReadAllColumnsPtr && InsertTablePtr && DisconnectPtr){
				char names[32][256];
				while (numTables <= 0) {
					int numCatalogs = ReadAllCatalogsPtr(sourceConnection, names);
					print2DArray(numCatalogs, names);
					int catalogIndex = -1;
					if (numCatalogs > 0) {
						catalogIndex = userChooseIndex(sourceConnection, numCatalogs, "catalog", CATALOG);
					}
					int numSchemas = ReadAllSchemasPtr(sourceConnection, names);
					print2DArray(numSchemas, names);
					int schemaIndex = -1;
					if (numSchemas > 0) {
						schemaIndex = userChooseIndex(sourceConnection, numSchemas, "schema", SCHEMA);
					}
					numTables = ReadTablesForCatalogAndSchemaPtr(sourceConnection, catalogIndex, schemaIndex, names);
					print2DArray(numTables, names);
					if (numTables > 0) {
						tableIndex = userChooseIndex(sourceConnection, numTables, "table", TABLE);
					}
					else {
						cout << "There are no tables in this catalog/schema. Please choose a different combination." << endl;
					}
				}
				char sourceColNames[32][256];
				int numColumns = ReadAllColumnsPtr(sourceConnection, tableIndex, sourceColNames);

				isUserReadyToChooseDatabase("target");
				targetConnection = NewConnectionPtr(SQL_OV_ODBC3_80);
				targetIsConnected = ConnectPtr(targetConnection);
				if (!targetIsConnected){
					cout << "Program failed to connect to target -- Exiting" << endl;
					exit(EXIT_SUCCESS);
				}

				int catalogIndex = -1;
				int numCatalogs = ReadAllCatalogsPtr(targetConnection, names);
				print2DArray(numCatalogs, names);
				if (numCatalogs > 0) {
					catalogIndex = userChooseIndex(targetConnection, numCatalogs, "target catalog", CATALOG);
				}

				int schemaIndex = -1;
				int numSchemas = ReadAllSchemasPtr(targetConnection, names);
				print2DArray(numSchemas, names);
				if (numSchemas > 0) {
					schemaIndex = userChooseIndex(targetConnection, numSchemas, "target schema", SCHEMA);
				}

				int rowsAtATime;
				while (1) {
					cout << "How many rows would you like to copy at a time?" << endl;
					if (cin >> rowsAtATime && rowsAtATime > 0)
						break;
					cout << "Please select a positive integer value" << endl;
					cin.clear();
					cin.ignore(INT_MAX, '\n');
				}

				inputAnswer = "";
				cout << "Would you like to create a new table or append to an existing table?" << endl;
				cout << "Press y for append. Any other string will create a new table." << endl;
				cin.ignore();
				getline(cin, inputAnswer);
				if (userSaidYes(inputAnswer)) {
					numTables = ReadTablesForCatalogAndSchemaPtr(targetConnection, catalogIndex, schemaIndex, names);
					print2DArray(numTables, names);
					int targetTableIndex = -1;
					if (numTables > 0) {
						targetTableIndex = userChooseIndex(targetConnection, numTables, "table to append to", TABLE);
					}
					else {
						cout << "There are no tables in this catalog/schema. Please choose a different combination." << endl;
					}

					cout << "Here are your source columns: " << endl;
					print2DArray(numColumns, sourceColNames);

					char targetColNames[32][256];
					unordered_map<int, int> columnsMap;
					numColumns = ReadAllColumnsPtr(targetConnection, targetTableIndex, targetColNames);
					for (int i = 0; i < numColumns; i++){
						int index = -1;
						string inputAnswer;
						while (!userSaidYes(inputAnswer)) {
							cout << "Choose a source column index for: " << targetColNames[i] << endl;
							if (cin >> index && isValidIndex(numColumns, index)){
								cout << "You have chosen " << sourceColNames[index] << endl;
								cout << "Press y to confirm selection" << endl;
							}
							else {
								cout << "This is not a valid column" << endl;
								cout << "Press y if you would not like to map this column" << endl;
							}
							cin.clear();
							cin.ignore(INT_MAX, '\n');
							getline(cin, inputAnswer);
						}
						if (index != -1 && isValidIndex(numColumns, index))
							columnsMap[index] = i;
					}
					int error = InsertTablePtr(sourceConnection, targetConnection, tableIndex, targetTableIndex, rowsAtATime, columnsMap);
					if (error)
						cout << "There were errors appending the rows" << endl;
					else
						cout << "Rows have been successfully appended" << endl;
				}
				else {
					string inputAnswer;
					cout << "Would you like to copy only some of the columns?" << endl;
					cout << "Press y for yes. Any other string will be taken as a no." << endl;
					set<int> columnIndexes;
					getline(cin, inputAnswer);

					if (userSaidYes(inputAnswer)){
						print2DArray(numColumns, sourceColNames);
						string inputColumns;
						while (columnIndexes.size() == 0) {
							cout << "Which columns would you like to be copied?" << endl;
							cout << "Please enter a comma separated list of values: " << endl;
							getline(cin, inputColumns);
							columnIndexes = parseColumns(numColumns, inputColumns);
							if (columnIndexes.size() == 0)
								cout << "Ensure that you have typed a COMMA separated list of POSITIVE integers" << endl;
						}
					}
					else {
						for (int i = 0; i < numColumns; ++i)
							columnIndexes.insert(i);
					}
					int error = CopyTablePtr(sourceConnection, targetConnection, catalogIndex, schemaIndex, tableIndex, rowsAtATime, columnIndexes);
					if (error)
						cout << "There were errors copying the table" << endl;
					else
						cout << "Table has been successfully copied!" << endl;
				}
				DisconnectPtr(sourceConnection);
				DisconnectPtr(targetConnection);
				
			}
			//Free the library:
			freeResult = FreeLibrary(dllHandle);
		}
	}

