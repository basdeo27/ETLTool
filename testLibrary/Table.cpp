// Table.cpp

#include "Table.h"
#include "Column.h"
#include "ResultSet.h"
#include <string>

using namespace std;

//namespace ODBCConnectorAPI
//{
	Table::Table(string catalog, string schema, string name, string tableType, string identifierQuote) :
		catalog(catalog),
		schema(schema),
		name(name),
		tableType(tableType)
	{
		setFullName(identifierQuote);
	}

//	void Table::addColumn(Column column){
//		columns.push_back(column);
//	}

	int Table::countColumns(){
		return columns.size();
	}

	string Table::getCatalog(){
		return catalog;
	}

	Column Table::getColumn(int index){
		return columns[0];
	}

	vector<Column> Table::getColumns(){
		return columns;
	}

	string Table::getName(){
		return fullName;
	}

	string Table::getOwner(){
		return "";
	}

	vector<string> Table::getPrimaryKeyColumns(){
		return primaryKeyColumns;
	}

	string Table::getSimpleName()
	{
		return name;
	}

	string Table::getSchema(){
		return schema;
	}


	string Table::getType(){
		return tableType.c_str();
	}

	vector<Column> Table::readAllColumns(HandleStatement* handleStatement) {
		columns.clear();
		SQLCHAR* catalogChar = catalog.size() != 0 ? (SQLCHAR *) catalog.c_str() : NULL;
		SQLCHAR* schemaChar = schema.size() != 0 ? (SQLCHAR *) schema.c_str() : NULL;
		int returnCode = SQLColumnsA(handleStatement->getHandle(), catalogChar, SQL_NTS, schemaChar, SQL_NTS, (SQLCHAR*) name.c_str(), SQL_NTS, NULL, 0);
		handleStatement->printErrorIfExists(returnCode);
		ResultSet resultSet = ResultSet(handleStatement);
		while (resultSet.getNextRow()){
			vector<string> aColumn = resultSet.getCurrentRow();
			Column newColumn = Column(aColumn[0], aColumn[1], aColumn[2], aColumn[3], stoi(aColumn[4]), aColumn[5], stoi(aColumn[6]));
			columns.push_back(newColumn);
		}
		return columns;
	}

	vector<string> Table::readPrimaryKey(HandleStatement* handleStatement) {
		SQLCHAR* catalogChar = catalog.size() != 0 ? (SQLCHAR *)catalog.c_str() : NULL;
		SQLCHAR* schemaChar = schema.size() != 0 ? (SQLCHAR *)schema.c_str() : NULL;
		int returnCode = SQLPrimaryKeysA(handleStatement->getHandle(), catalogChar, SQL_NTS, schemaChar, SQL_NTS, (SQLCHAR*) name.c_str(), SQL_NTS);
		handleStatement->printErrorIfExists(returnCode);
		ResultSet resultSet = ResultSet(handleStatement);
		while (resultSet.getNextRow())
			primaryKeyColumns.push_back((resultSet.getCurrentRow())[3]);

		return primaryKeyColumns;
	}

	void Table::setFullName(string identifierQuote){
		fullName = "";
		if (!(catalog.compare("") == 0)){
			fullName.append(identifierQuote);
			fullName.append(catalog);
			fullName.append(identifierQuote);
			fullName.append(".");
		}
		if (!(schema.compare("") == 0)){
			fullName.append(identifierQuote);
			fullName.append(schema);
			fullName.append(identifierQuote);
			fullName.append(".");
		}
		fullName.append(identifierQuote);
		fullName.append(name);
		fullName.append(identifierQuote);
	}

	Table::~Table()
	{

	}

//}