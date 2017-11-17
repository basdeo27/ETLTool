#ifndef HANDLESTATEMENT_H
#define HANDLESTATEMENT_H
// ODBCConnectorAPIUtils.h
#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include <string>
#include <memory>
#include <vector>
#include <sql.h>
//#include "Error.h"

using namespace std;

class HandleStatement {
	public:
		HandleStatement();

		//Error* getError();

		short allocateEnvironmentHandle();

		short allocateConnectionHandle();

		short allocateStatementHandle();

		short driverConnect();

		SQLHANDLE getHandle();

		SQLHANDLE getConn();

		int getBulkFetch();

		short hiveDriverConnect();

		string printErrorIfExists(int returnCode);

		short releaseStatementHandle();

		short releaseConnectionHandle();

		short releaseEnvironmentHandle();

		void setBulkFetch(int bulkFetch);

		short setEnvironmentVariable(long attribute, unsigned long value);

		~HandleStatement();

		//BOOL DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);

		void makeConnection(HWND hWndDlg);
	
	private:
		SQLHANDLE hEnv, hdlConn, hStmt;

		int bulkFetch;

		//BOOL CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);

};

void clearWindows(HWND hWndDlg);

BOOL enableWindows(HWND hWndDlg, int realm, int fqdn, int serviceName, int httpPath, int userName, int password);

BOOL getAllDialogsText(HWND hWndDlg);

BOOL enableOk(HWND hWndDlg);

BOOL CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);

wstring getConnectionString();

#endif