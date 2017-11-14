// Query.cpp

#include "Query.h"
#include <string>

using namespace std;

//namespace ODBCConnectorAPI
//{
/**
	Query::Query(string queryString) :
		queryString(queryString)
		//resultSet(resultSet)
	{

	}

	Query::Query(string queryString, vector<string> row)
	{
		Query::queryString = createQueryFromRow(queryString, row);
	}
	*/
	Query::Query(string base, string tableName, string afterTableName, string identifierQuote) : 
		base(base),
		tableName(tableName),
		afterTableName(afterTableName),
		identifierQuote(identifierQuote)
	{
		resetQueryString();
	}

	void Query::addListOfValuesToInsertQuery2(vector<vector<string>>* rows, set<int> columnIndexes) {
		values = "";
		for (unsigned int i = 0; i < rows->size() - 1; i++){
			addValuesToInsertQuery((*rows)[i], columnIndexes, true);
			values.append(", ");
		}
		addValuesToInsertQuery((*rows)[rows->size() - 1], columnIndexes, true);
	}

	void Query::addListOfValuesToInsertQuery(vector<vector<string>> rows, set<int> columnIndexes) {
		values = "";
		for (unsigned int i = 0; i < rows.size() - 1; i++){
			addValuesToInsertQuery(rows[i], columnIndexes, true);
			values.append(", ");
		}
		addValuesToInsertQuery(rows[rows.size() - 1], columnIndexes, true);
	}

	void Query::addColumnsToInsertQuery(vector<Column> columns) {
		string columnString = " (";
		for (unsigned int i = 0; i < columns.size() - 1; i++) {
			columnString.append(columns[i].getName());
			columnString.append(", ");
		}
		columnString.append(columns[columns.size() - 1].getName());
		columnString.append(")");
		tableName.append(columnString);

		resetQueryString();
	}

	vector<string> filterRow(vector<string> row, set<int> columnIndexes){
		if (columnIndexes.size() > 0){
			vector<string> filteredRow;
			for (unsigned int i = 0; i < row.size(); i++){
				if (columnIndexes.find(i) != columnIndexes.end())
					filteredRow.push_back(row[i]);
			}
			return filteredRow;
		}
		return row;
	}

	void Query::addValuesToInsertQuery(vector<string> row, set<int> columnIndexes, bool appendToValues) {
		if (!appendToValues)
			values = "";
		row = filterRow(row, columnIndexes);
		values.append("(");
		for (unsigned int i = 0; i < row.size() - 1; i++){
			values.append(singleQuoteWord(row[i]));
			values.append(",");
		}
		values.append(singleQuoteWord(row[row.size() - 1]));
		values.append(")");
	}

	void Query::addFullTableNameToQuery(string catalog, string schema, string table) {
		////////////////////////////////////////////////////////////
		//INSERT CHECK IF TABLE EXISTS AND SUBSTRING IT IF IT DOES//
		////////////////////////////////////////////////////////////
		tableName =  "";
		if (catalog.size() > 0){
			tableName.append(identifierQuoteWord(catalog));
			tableName.append(".");
		}
		if (schema.size() > 0){
			tableName.append(identifierQuoteWord(schema));
			tableName.append(".");
		}
		tableName.append(identifierQuoteWord(table));
		tableName.append(" ");
		queryString.append(tableName);
	}

	void Query::addColumnsToCreateQuery(vector<Column> columns, vector<string> primaryKey) {
		string columnString = " (";
		for (unsigned int i = 0; i < columns.size() - 1; i++) {
			columnString = appendColumn(columnString, columns[i]);
			columnString.append(", ");
		}
		columnString = appendColumn(columnString, columns[columns.size() - 1]);
		if (primaryKey.size() > 0){
			columnString.append(", PRIMARY KEY(");
			for (unsigned int i = 0; i < primaryKey.size() - 1; i++){
				columnString.append(primaryKey[i]);
			}
			columnString.append(primaryKey[primaryKey.size() - 1]);
			columnString.append(")");
		}
		columnString.append(")");
		queryString.append(columnString);
	}

	string Query::getQueryString(){
		return queryString;
	}

	string Query::getInsertQueryString(){
		return queryString + values;
	}

	string Query::getBaseString(){
		return base;
	}

	string Query::getTableName(){
		return tableName;
	}

	Query::~Query(){

	}

	////////////////// PRIVATE ///////////////////////////
	string Query::appendColumn(string columnString, Column column){
		columnString.append(identifierQuoteWord(column.getName()));
		columnString.append(" ");
		columnString.append(column.getTypeName());
		if (column.getTypeName().find("char") != -1){
			columnString.append("(");
			int columnSize = column.getColumnSize();
			columnSize = columnSize == 0 ? 2048 : columnSize;
			//if (columnSize == 0)
			//	columnString.append("");
			//else 
			columnString.append(to_string(columnSize));
			columnString.append(")");
		}
		return columnString;
	}
	/**
	string Query::escapedQuoteWord(string word){
		string quotedWord = "\"";
		quotedWord.append(word);
		quotedWord.append("\"");
		return quotedWord;
	}*/

	string Query::identifierQuoteWord(string word){
		string quotedWord = identifierQuote;
		quotedWord.append(word);
		quotedWord.append(identifierQuote);
		return quotedWord;
	}


	void Query::resetQueryString(){
		queryString = "";
		queryString.append(base);
		queryString.append(tableName);
		queryString.append(afterTableName);
	}

	string Query::singleQuoteWord(string word){
		string quotedWord = "'";
		quotedWord.append(word);
		quotedWord.append("'");
		return quotedWord;
	}
//}