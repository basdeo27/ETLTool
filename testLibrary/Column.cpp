// Column.cpp
#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include "Column.h"
#include <string>

using namespace std;

//namespace ODBCConnectorAPI
//{
//	Column::Column(){

//	}

	/**
	Column::Column(string catalog, string schema, string tableName, string name, int dataType, string typeName, int columnSize,
		int bufferLength, int decimalDigits, int numPrecRadix, int nullable, string remarks, int sqlDataType,
		int sqlDatetimeSubtype, int charOctetLength, int ordinalPosition) : 
		catalog(catalog),
		schema(schema),
		tableName(tableName),
		name(name),
		dataType(dataType),
		typeName(typeName),
		columnSize(columnSize),
		bufferLength(bufferLength),
		decimalDigits(decimalDigits),
		numPrecRadix(numPrecRadix),
		nullable(nullable),
		remarks(remarks),
		sqlDataType(sqlDataType),
		sqlDatetimeSubtype(sqlDatetimeSubtype),
		charOctetLength(charOctetLength),
		ordinalPosition(ordinalPosition)
	{
		//nullable = isNullable == 0 ? true : false;
	}
	*/

	Column::Column(string catalog, string schema, string tableName, string name, int dataType, string typeName, int columnSize) :
		catalog(catalog),
		schema(schema),
		tableName(tableName),
		name(name),
		dataType(dataType),
		typeName(typeName),
		columnSize(columnSize)
	{

	}

	string Column::getName(){
		return name;
	}

	string Column::getTypeName(){
		return typeName;
	}

	int Column::getColumnSize(){
		return columnSize;
	}

	Column::~Column()
	{

	}

//}