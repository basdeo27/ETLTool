// ResultSet.cpp

#include "ResultSet.h"
#include <iostream>

using namespace std;

//namespace ODBCConnectorAPI
//{
	int currentPoint = 0;

	ResultSet::ResultSet(HandleStatement* handleStatement) :
		handleStatement(handleStatement)
	{
		
		int retCode;
		retCode = SQLNumResultCols(handleStatement->getHandle(), &numColumns);
		
		//results = allocateResults(numColumns, handleStatement->getBulkFetch());
		results = allocateResultsArray(numColumns, handleStatement->getBulkFetch());
		for (int i = 0; i < numColumns; i++)
			retCode = SQLBindCol(handleStatement->getHandle(), (SQLUSMALLINT)i + 1, results[i].TargetType, results[i].TargetValuePtr.get(), results[i].BufferLength, results[i].StrLen_or_Ind.get());
			//retCode = SQLBindCol(handleStatement->getHandle(), (SQLUSMALLINT)i + 1, results[i].TargetType, results[i].TargetValuePtr, results[i].BufferLength, results[i].StrLen_or_Ind);
		
	}

	//vector<vector<string>> ResultSet::getResultSetString() {
	//	return resultSetString;
	//}

	int ResultSet::getColumnCount(){
		return numColumns;
	}

	/*
	bool ResultSet::getNextRow(){
		int retCode = SQLFetch(handleStatement->getHandle());
		if (retCode == SQL_NO_DATA)
			return false;
		//handleStatement->printErrorIfExists(retCode);
		currentRow.clear();
		for (int i = 0; i < numColumns; i++){
			if (results[i].StrLen_or_Ind != SQL_NULL_DATA)
				currentRow.push_back(results[i].TargetValuePtr.get());
			else
				currentRow.push_back("");
		}
		return true;
	}
	*/

	bool ResultSet::getNextRow(){
		int retCode = SQLFetch(handleStatement->getHandle());
		if (retCode == SQL_NO_DATA)
			return false;
		//handleStatement->printErrorIfExists(retCode);
		currentRow.clear();
		for (int i = 0; i < numColumns; i++) {
			char * batchRow = results[i].TargetValuePtr.get();
			if (results[i].StrLen_or_Ind.get()[currentPoint] != SQL_NULL_DATA){
				int targetValueStart = currentPoint*results[i].BufferLength;
				currentRow.push_back(string(&batchRow[targetValueStart], &batchRow[targetValueStart + results[i].StrLen_or_Ind.get()[currentPoint]]));
			}
			else
				currentRow.push_back("");
		}
		currentPoint = (currentPoint + 1) % handleStatement->getBulkFetch();
		return true;
	}

	vector<string> ResultSet::getCurrentRow(){
		return currentRow;
	}
	/**
	vector<vector<string>> ResultSet::getNRows(int n){
		int retCode;
		vector<vector<string>> rows;
		for (retCode = SQLFetch(handleStatement->getHandle()); MySQLSuccess(retCode); retCode = SQLFetch(handleStatement->getHandle())){
			handleStatement->printErrorIfExists(retCode);
			currentRow.clear();
			for (int i = 0; i < numColumns; i++){
				currentRow.push_back((results[i].TargetValuePtr.get()));
			}
			rows.push_back(currentRow);
		}
		return rows;
	}*/

	
	vector<vector<string>> ResultSet::getNRows(int n){
		int retCode;
		vector<vector<string>> rows;
		for (retCode = SQLFetch(handleStatement->getHandle()); MySQLSuccess(retCode); retCode = SQLFetch(handleStatement->getHandle())){
			handleStatement->printErrorIfExists(retCode);
			currentRow.clear();
			for (int i = 0; i < handleStatement->getBulkFetch(); i++){
				for (int j = 0; j < numColumns; j++) {
					char * batchRow = results[j].TargetValuePtr.get();
					currentRow.push_back(string(&batchRow[i*results[j].BufferLength], &batchRow[(i*results[j].BufferLength) + results[j].StrLen_or_Ind.get()[i]]));
				}
			}
			rows.push_back(currentRow);
		}
		return rows;
	}

	/*
	vector<vector<string>> ResultSet::getNRows(int n){
		int retCode;
		vector<vector<string>> batchRows(handleStatement->getBulkFetch());
		for (retCode = SQLFetch(handleStatement->getHandle()); MySQLSuccess(retCode); retCode = SQLFetch(handleStatement->getHandle())){
			handleStatement->printErrorIfExists(retCode);
			currentRow.clear();
			for (int j = 0; j < numColumns; j++) {
				char * batchRow = results[j].TargetValuePtr.get();
				for (int i = 0; i < handleStatement->getBulkFetch(); i++){
					batchRows[i].push_back(string(&batchRow[i*results[j].BufferLength], &batchRow[i*results[j].BufferLength]));
				}
			}
		}
		return batchRows;
	} */


	string ResultSet::getColumnValueChar(unsigned int index){
		SQLCHAR targetValuePtr[BUFFER_LENGTH];
		SQLLEN strlenOrIndPtr;
		SQLGetData(handleStatement->getHandle(), index + 1, SQL_C_CHAR, targetValuePtr, sizeof(targetValuePtr), &strlenOrIndPtr);
		return string((char *) targetValuePtr);
	}

	float ResultSet::getColumnValueFloat(unsigned int index){
		SQLREAL targetValuePtr;
		SQLLEN strlenOrIndPtr;
		SQLGetData(handleStatement->getHandle(), index + 1, SQL_C_FLOAT, &targetValuePtr, sizeof(targetValuePtr), &strlenOrIndPtr);
		return targetValuePtr;
	}

	double ResultSet::getColumnValueDouble(unsigned int index){
		SQLDOUBLE targetValuePtr;
		SQLLEN strlenOrIndPtr;
		SQLGetData(handleStatement->getHandle(), index + 1, SQL_C_DOUBLE, &targetValuePtr, sizeof(targetValuePtr), &strlenOrIndPtr);
		return targetValuePtr;
	}

	long ResultSet::getColumnValueLong(unsigned int index){
		SQLINTEGER targetValuePtr;
		SQLLEN strlenOrIndPtr;
		SQLGetData(handleStatement->getHandle(), index + 1, SQL_C_SLONG, &targetValuePtr, sizeof(targetValuePtr), &strlenOrIndPtr);
		return targetValuePtr;
	}

	bool ResultSet::closeCursor(){
		return SQLFreeStmt(handleStatement->getHandle(), SQL_CLOSE) == SQL_SUCCESS;
	}

	//vector<vector<struct DataBinding>> ResultSet::getResultSet() {
	//	return resultSet;
	//}

	ResultSet::~ResultSet(){
		SQLFreeStmt(handleStatement->getHandle(), SQL_CLOSE);
	}
//}


	/////////////// PRIVATE ////////////////////////
	vector<struct DataBinding> ResultSet::allocateResults(int columns, int rows) {
		vector<struct DataBinding> results(columns);
		for (int i = 0; i < columns; i++) {
			results[i].TargetType = SQL_C_CHAR;
			results[i].BufferLength = (BUFFER_LENGTH + 1);
			results[i].TargetValuePtr = shared_ptr<char>{ new char[BUFFER_LENGTH] };
		}
		return results;
	}

	vector<struct DataBindingArray> ResultSet::allocateResultsArray(int columns, int rows) {
		vector<struct DataBindingArray> results(columns);
		for (int i = 0; i < columns; i++) {
			results[i].TargetType = SQL_C_CHAR;
			results[i].BufferLength = BUFFER_LENGTH;
			results[i].TargetValuePtr = shared_ptr<char>{ new char[BUFFER_LENGTH * rows] };
			results[i].StrLen_or_Ind = shared_ptr<SQLLEN>{ new SQLLEN[rows] };
		}
		return results;
	}

	int ResultSet::MySQLSuccess(SQLRETURN rc) {
		return (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO);
	}