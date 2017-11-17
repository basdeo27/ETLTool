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
		currentPoint = 0;
		int retCode;
		retCode = SQLNumResultCols(handleStatement->getHandle(), &numColumns);
		
		//results = allocateResults(numColumns, handleStatement->getBulkFetch());
		results = allocateResultsArray(numColumns, handleStatement->getBulkFetch());
		for (int i = 0; i < numColumns; i++)
			//retCode = SQLBindCol(handleStatement->getHandle(), (SQLUSMALLINT)i + 1, results[i].TargetType, results[i].TargetValuePtr.get(), results[i].BufferLength, results[i].StrLen_or_Ind.get());
			retCode = SQLBindCol(handleStatement->getHandle(), (SQLUSMALLINT)i + 1, results[i].TargetType, results[i].TargetValuePtr, results[i].BufferLength, results[i].StrLen_or_Ind);
		
	}

	//vector<vector<string>> ResultSet::getResultSetString() {
	//	return resultSetString;
	//}

	int ResultSet::getColumnCount(){
		return numColumns;
	}

	bool ResultSet::getNextRow(){
		int retCode = SQLFetch(handleStatement->getHandle());
		if (retCode == SQL_NO_DATA)
			return false;
		currentRow.clear();
		for (int i = 0; i < numColumns; i++){
			if (results[i].StrLen_or_Ind[0] != SQL_NULL_DATA)
				currentRow.push_back(results[i].TargetValuePtr);
			else
				currentRow.push_back("");
		}
		return true;
	}
	

	vector<string> ResultSet::getCurrentRow(){
		return currentRow;
	}
	
	vector<vector<string>> ResultSet::getNRows(int n){
		int retCode = SQLFetch(handleStatement->getHandle());
		if (retCode != 0)
			return vector<vector<string>>(0);
		vector<vector<string>> batchRows(n);
		handleStatement->printErrorIfExists(retCode);
		int max = 0;
		for (int j = 0; j < numColumns; j++) {
			for (int i = 0; i < n; i++) {
				if (results[j].StrLen_or_Ind[i] != SQL_NULL_DATA) {
					char * batchRow = results[j].TargetValuePtr + (i * BUFFER_LENGTH);
					batchRows[i].push_back(string(batchRow, batchRow + results[j].StrLen_or_Ind[i]));
					if (i > max) max = i;
					results[j].StrLen_or_Ind[i] = SQL_NULL_DATA;
				}
				else
					batchRows[i].push_back("");
			}
		}
		batchRows.resize(max + 1);
		return batchRows;
	} 

	bool ResultSet::getNRows(vector<vector<string>>* batchRows){
		int size = batchRows->size();
		int retCode = SQLFetch(handleStatement->getHandle());
		if (retCode != 0){
			batchRows->resize(0);
			return batchRows;
		}
		int max = 0;
		for (int j = 0; j < numColumns; j++) {
			for (int i = 0; i < batchRows->size(); i++) {
				if (results[j].StrLen_or_Ind[i] != SQL_NULL_DATA) {
					char * batchRow = results[j].TargetValuePtr + (i * BUFFER_LENGTH);
					(*batchRows)[i].push_back(string(batchRow, batchRow + results[j].StrLen_or_Ind[i]));
					if (i > max) max = i;
					results[j].StrLen_or_Ind[i] = SQL_NULL_DATA;
				}
				else
					(*batchRows)[i].push_back("");
			}
		}
		batchRows->resize(max + 1);
		return max+1 == size;
	}


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

	ResultSet::~ResultSet(){
		for (int i = 0; i < results.size(); i++) {
			free(results[i].TargetValuePtr);
			free(results[i].StrLen_or_Ind);
		}
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
			results[i].TargetValuePtr = (char*) malloc(BUFFER_LENGTH * rows);
			results[i].StrLen_or_Ind = (SQLLEN*) malloc(rows*sizeof(SQLLEN));
			//results[i].TargetValuePtr = shared_ptr<char>{ new char[BUFFER_LENGTH * rows] };
			//results[i].StrLen_or_Ind = shared_ptr<SQLLEN>{ new SQLLEN[rows] };
		}
		return results;
	}

	int ResultSet::MySQLSuccess(SQLRETURN rc) {
		return (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO);
	}