#ifndef ERROR_H
#define ERROR_H

// Connection.h
#include "HandleStatement.h"
#include <string>
#include <vector>

using namespace std;


	class Error
	{
	public:
		// Returns
		Error(HandleStatement* handleStatement);

		//Error();

		string extractError();
		//string extractError(HandleStatement* handleStatement);

		~Error();

	private:
		HandleStatement* handleStatement;

	};

#endif