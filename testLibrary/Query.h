#ifndef QUERY_H
#define QUERY_H
// Connection.h
#include <string>
#include <set>
#include "Column.h"
#include "ResultSet.h"

using namespace std;

//namespace ODBCConnectorAPI
//{
static const string SELECT_STAR = "SELECT * FROM ";

static const string CREATE_TABLE = "CREATE TABLE ";

static const string INSERT = "INSERT INTO ";

	class Query
	{
	public:
		// Returns
		//Query(string queryString);

		//Query(string queryString, vector<string> row);

		Query(string base, string tableName, string afterTableName, string identifierQuote);

		//void addListOfValuesToInsertQuery(vector<vector<string>> rows, set<int> columnIndexes);

		void addListOfValuesToInsertQuery(vector<vector<string>>* rows, set<int> columnIndexes);

		void addColumnsToInsertQuery(vector<Column> columns);

		void addValuesToInsertQuery(vector<string> row, set<int> columnIndexes, bool appendToValues);

		void addFullTableNameToQuery(string catalogName, string schemaName, string copyTableName);

		void addColumnsToCreateQuery(vector<Column> columns, vector<string> primaryKey);

		string getBaseString();

		string getTableName();

		/**

		void open();

		void close();

		void bindColumn(Column column);

		void next();

		string getValues();

		int countColumns();
		*/

		string getQueryString();

		string getInsertQueryString();

		~Query();

	private:
		string afterTableName;

		string base;

		string identifierQuote;

		string queryString;

		string tableName;

		string values;

		string appendColumn(string columnString, Column column);

		string identifierQuoteWord(string word);

		void resetQueryString();

		string singleQuoteWord(string word);

		//ResultSet resultSet;

	};
//}

#endif