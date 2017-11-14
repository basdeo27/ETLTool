#ifndef TABLE_H
#define TABLE_H
// Connection.h
#include <string>
#include <vector>
#include "Column.h"
#include "HandleStatement.h"

using namespace std;

	class Table
	{
	public:
		// Returns
		Table(string catalog, string schema, string name, string tableType, string identifierQuote);

		void addColumn(Column column);

		string getType();

		int countColumns();

		string getCatalog();

		Column getColumn(int index);

		void getColumnChars();
		
		vector<Column> getColumns();

		string getName();
		
		string getSchema();

		string getOwner();

		vector<string> getPrimaryKeyColumns();

		string getSimpleName();

		vector<Column> readAllColumns(HandleStatement* handleStatement);

		vector<string> readPrimaryKey(HandleStatement* handleStatement);

		void setFullName(string identifierQuote);

		~Table();

	private:
		string catalog;

		vector<Column> columns;

		string fullName;

		string name;

		vector<string> primaryKeyColumns;

		string schema;

		string tableType;
	};

#endif