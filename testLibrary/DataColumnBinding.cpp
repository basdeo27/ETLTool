// Cursor.cpp

#include "DataColumnBinding.h"
#include <string>

using namespace std;

//namespace ODBCConnectorAPI
//{
DataColumnBinding::DataColumnBinding(string columnName, int dataType, int columnSize, void* valueBuffer) :
	columnName(columnName),
	dataType(dataType),
	columnSize(columnSize),
	valueBuffer(valueBuffer)
{

}

DataColumnBinding::~DataColumnBinding()
{

}