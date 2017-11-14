#ifndef RESULTSET_H
#define RESULTSET_H
// Connection.h
#include <string>
#include "Table.h"
#include "Column.h"
#include "Cursor.h"
#include "HandleStatement.h"
#include <sqlext.h>

using namespace std;


static const int BUFFER_LENGTH = 1024;

	class ResultSet
	{
	public:
		// Returns
		ResultSet(HandleStatement* handleStatement);

		string getOwner();

		string getSimpleName();

		void open();

		void openTable();

		void close();

		int getColumnCount();

		Column getColumnInfo(string columnName);

		string getColumnType(string columnName);

		string getColumnName(int index);

		bool isColumnValueNull(string columnName);

		void getColumnValueFormatted(string columnName);

		void getColumnValueSize(unsigned int index);

		int getColumnValueBinary(unsigned int index);

		string getColumnValueChar(unsigned int index);

		float getColumnValueFloat(unsigned int index);

		double getColumnValueDouble(unsigned int index);

		long getColumnValueLong(unsigned int index);

		string getColumnValueTime(int index);

		string getColumnValueTimestamp(int index);

		string getColumnValueInterval();

		void moveFirst();

		void moveLast();

		void moveNext();

		void movePrev();

		void addNew();

		void update();

		void deleteX();

		void commit();

		void resetColumnValueToDefault(int index);

		void setColumnValueNull(int index);

		void setColumnValueFormatted(int index, string value);

		void setColumnValueBinary(int index, string value);

		void setColumnValueChar(int index, string value);

		void setColumnValueFloat(int index, float value);

		void setColumnValueDouble(int index, double value);

		void setColumnValueLong(int index, long value);

		void setColumnValueTime(int index, string value);

		void setColumnValueTimeStamp(int index, string value);

		void setColumnValueInterval(int index, string value);


		//vector<vector<string>> getResultSetString();

		//vector<vector<struct DataBinding>> getResultSet();

		//bool hasNext();

		bool getNextRow();

		vector<string> getCurrentRow();

		vector<vector<string>> getNRows(int n);

		bool closeCursor();

		~ResultSet();

	private:
		//Table table;

		//Cursor cursor;

		HandleStatement* handleStatement;

		vector<vector<string>> resultSetString;

		vector<vector<struct DataBinding>> resultSet;

		//vector<struct DataBinding> results;

		vector<struct DataBindingArray> results;

		vector<string> currentRow;

		vector<vector<string>> batchRows;

		short numColumns;

		vector<struct DataBinding> allocateResults(int columns, int rows);

		vector<struct DataBindingArray> allocateResultsArray(int columns, int rows);

		int MySQLSuccess(SQLRETURN rc);
	};

	struct DataBinding {
		SQLSMALLINT TargetType;
		shared_ptr<char> TargetValuePtr;
		SQLINTEGER BufferLength;
		SQLLEN StrLen_or_Ind;
	};

	struct DataBindingArray {
		SQLSMALLINT TargetType;
		//shared_ptr<char> TargetValuePtr;
		char* TargetValuePtr;
		SQLINTEGER BufferLength;
		//shared_ptr<SQLLEN> StrLen_or_Ind;
		SQLLEN* StrLen_or_Ind;
	};


#endif