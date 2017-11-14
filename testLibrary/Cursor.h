#ifndef CURSOR_H
#define CURSOR_H
// Connection.h
#include <string>

using namespace std;

//namespace ODBCConnectorAPI
//{
	class Cursor
	{
	public:
		// Returns
		Cursor();

		void openCursor();

		void closeCursor();

		void addColumnBuffer();

		string getValues();

		void prev();

		void next();

		void first();

		void last();

		~Cursor();

	private:
		
	};
//}

#endif