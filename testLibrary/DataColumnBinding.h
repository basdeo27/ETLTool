#pragma once

// DataColumnBinding.h
#include "HandleStatement.h"
#include <string>
#include <vector>

using namespace std;


class DataColumnBinding
{
public:
	// Returns
	DataColumnBinding(string columnName, int dataType, int columnSize, void* valueBuffer);


	~DataColumnBinding();

private:
	string columnName;

	int dataType;

	int columnSize;

	void* valueBuffer;

};

