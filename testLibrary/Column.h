#ifndef COLUMN_H
#define COLUMN_H
// Connection.h
#include <string>

using namespace std;

//namespace ODBCConnectorAPI
//{
	class Column
	{
	public:
		// Returns
		
		//Column(string catalog, string schema, string tableName, string name, int dataType, string typeName, int columnSize,
		//	int bufferLength, int decimalDigits, int numPrecRadix, int nullable, string remarks, int sqlDataType,
		//	int sqlDatetimeSubtype, int charOctetLength, int ordinalPosition);
		Column(string catalog, string schema, string tableName, string name, int dataType, string typeName, int columnSize);

		string getName();

		string getTypeName();

		int getColumnSize();

		~Column();

	private:

		// ODBC Column Info

		string catalog;

		string schema;

		string tableName;

		string name;

		int dataType;

		string typeName;

		int columnSize;
		
		/**
		int bufferLength;

		int decimalDigits;

		int numPrecRadix;

		int nullable;

		string remarks;

		int sqlDataType;

		int sqlDatetimeSubtype;

		int charOctetLength;

		int ordinalPosition;
		*/

	};
//}

#endif