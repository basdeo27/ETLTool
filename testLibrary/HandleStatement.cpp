// Error.cpp
#if defined(WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include "HandleStatement.h"
#include <string>
#include <sql.h>
#include <sqlext.h>
#include "Error.h"
#include "resource.h"

using namespace std;

Error * error;

HandleStatement::HandleStatement() 
	{
		error = new Error(this);
		bulkFetch = 1;
	}

//	Error*  HandleStatement::getError(){
//		return error;
//	}

	short HandleStatement::allocateEnvironmentHandle(){
		return SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	}

	short HandleStatement::allocateConnectionHandle() {
		return SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hdlConn);
	}

	short HandleStatement::allocateStatementHandle(){
		return SQLAllocHandle(SQL_HANDLE_STMT, hdlConn, &hStmt);
	}

	short HandleStatement::driverConnect(){
		SQLWCHAR* pwszConnStr = (SQLWCHAR *) "";
#if defined(WIN32) || defined(_WIN64) 
		return SQLDriverConnect(hdlConn, GetDesktopWindow(), pwszConnStr, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_COMPLETE);
#elif __APPLE__
		short buflen;
		checkODBCResponse(SQLDriverConnect(hdlConn, 0, (SQLCHAR *) "DSN=QuboleODBC", SQL_NTS,
			(SQLCHAR *)outdsn, NUMTCHAR(outdsn), &buflen, SQL_DRIVER_COMPLETE));
#endif
	}

	int HandleStatement::getBulkFetch() {
		return bulkFetch;
	}

	SQLHANDLE HandleStatement::getConn(){
		return hdlConn;
	}

	SQLHANDLE HandleStatement::getHandle(){
		return hStmt;
	}

	string HandleStatement::printErrorIfExists(int returnCode){
		return returnCode != 0 ? error->extractError() : "";
	}

	short HandleStatement::releaseConnectionHandle(){
		SQLDisconnect(hdlConn);
		return SQLFreeHandle(SQL_HANDLE_DBC, hdlConn);
	}

	short HandleStatement::releaseEnvironmentHandle(){
		return SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
	}

	short HandleStatement::releaseStatementHandle(){
		//return SQLFreeHandle(SQL_HANDLE_DBC, hStmt);
		return 0;
	}

	void HandleStatement::setBulkFetch(int bulkFetch){
		HandleStatement::bulkFetch = bulkFetch;
	}

	short HandleStatement::setEnvironmentVariable(long attribute, unsigned long value){
		return SQLSetEnvAttr(hEnv, attribute, (SQLPOINTER)value, 0);
	}

	HandleStatement::~HandleStatement()
	{
		delete error;
	}

	///////////////////////////////////// DIALOGS ///////////////////////////

	void HandleStatement::makeConnection(HWND hWndDlg){
		const int maxlen = 256;
		wchar_t dsn[256];
		wchar_t server[256];
		wchar_t port[5];
		wchar_t username[256];
		wchar_t password[256];
		wchar_t strConnectOut[5000];
		SQLSMALLINT	iStrMaxCount = 0;

		GetDlgItemText(hWndDlg, IDC_EDIT1, dsn, maxlen);
		GetDlgItemText(hWndDlg, IDC_EDIT2, server, maxlen);
		GetDlgItemText(hWndDlg, IDC_EDIT3, port, maxlen);
		GetDlgItemText(hWndDlg, IDC_EDIT4, username, maxlen);
		GetDlgItemText(hWndDlg, IDC_EDIT5, password, maxlen);

		wstring dsnString = L"DSN=" + wstring(dsn) + L";SERVER=" + wstring(server) + L";PORT=" + wstring(port) + L";UID=" + wstring(username) + L";PWD=" + wstring(password) + L";";
		int retCode = SQLDriverConnectW(hdlConn, NULL, (SQLWCHAR*) dsn, SQL_NTS, (SQLTCHAR*)strConnectOut, 5000, &iStrMaxCount, SQL_DRIVER_NOPROMPT);
		int x = 3;
		x = x + 3;
	}

	const int maxlen = 256;
	/*wchar_t dsn[maxlen];
	wchar_t server[maxlen];
	wchar_t port[5];
	wchar_t username[maxlen];
	wchar_t password[maxlen];
	wchar_t strConnectOut[5000];*/
	char dsn[maxlen];
	char server[maxlen];
	char port[5];
	char username[maxlen];
	char password[maxlen];
	char strConnectOut[5000];
	SQLSMALLINT	iStrMaxCount = 0;

	BOOL CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		switch (Msg)
		{
		case WM_INITDIALOG:
			return TRUE;

		case WM_COMMAND:
			switch (wParam)
			{
			case IDOK:
				GetDlgItemTextA(hWndDlg, IDC_EDIT1, dsn, maxlen);
				GetDlgItemTextA(hWndDlg, IDC_EDIT2, server, maxlen);
				GetDlgItemTextA(hWndDlg, IDC_EDIT3, port, maxlen);
				GetDlgItemTextA(hWndDlg, IDC_EDIT4, username, maxlen);
				GetDlgItemTextA(hWndDlg, IDC_EDIT5, password, maxlen);
				EndDialog(hWndDlg, 0);
				return TRUE;
			case IDCANCEL:
				EndDialog(hWndDlg, 0);
				return TRUE;
			}
			break;
		}

		return FALSE;
	}

	
	short HandleStatement::hiveDriverConnect() {
		int nCmdShow = 5;
		HWND hwndOwner = GetDesktopWindow();
		DialogBoxW(GetModuleHandle(L"testLibrary.dll"),                   // application instance
			MAKEINTRESOURCE(IDD_DIALOG1), // dialog box resource
			hwndOwner,                          // owner window
			(DLGPROC) DlgProc                    // dialog box window procedure
			);
		//MessageBox(hwnd, L"Hello", L"Caption", MB_OKCANCEL);
		//wstring dsnString = L"Driver={" + wstring(dsn) + L"};Host=" + wstring(server) + L";Port=" + wstring(port) + L";AuthMech=3;UID=" + wstring(username) + L";PWD=" + wstring(password) + L";";
		//int retCode = SQLDriverConnectW(hdlConn, NULL, (SQLWCHAR*)dsn, SQL_NTS, (SQLTCHAR*)strConnectOut, 5000, &iStrMaxCount, SQL_DRIVER_NOPROMPT);
		string dsnString = "Driver=" + string(dsn) + ";Host=" + string(server) + ";Port=" + string(port) + ";AuthMech=3;UID=" + string(username) + ";PWD=" + string(password) + ";";

		int retCode = SQLDriverConnectA(hdlConn, NULL, (SQLCHAR*)dsnString.c_str(), SQL_NTS, (SQLCHAR*)strConnectOut, 5000, &iStrMaxCount, SQL_DRIVER_NOPROMPT);
		return retCode;
	}
	

	/*
	short HandleStatement::hiveDriverConnect() {
		const int maxlen = 256;
		wchar_t dsn[maxlen];
		wchar_t server[maxlen];
		wchar_t port[5];
		wchar_t username[maxlen];
		wchar_t password[maxlen];
		wchar_t strConnectOut[5000];
		SQLSMALLINT	iStrMaxCount = 0;


		int nCmdShow = 5;
		//HWND hDlg = CreateDialogParam(hinst, MAKEINTRESOURCEW(IDD_DLGFIRST), 0, (DLGPROC)DlgProc, 0);
		//ShowWindow(hDlg, SW_MAXIMIZE);
		HWND hwndOwner = GetDesktopWindow();
		HINSTANCE hinst = GetModuleHandleA("testLibrary.dll");
		//InitCommonControls();
		HWND dialog{};
		LPSTR hi = MAKEINTRESOURCEA(IDD_DIALOG1);
		dialog = CreateDialogA(hinst, hi, hwndOwner, DlgProc);

		DWORD error = GetLastError();
		if (!dialog) {
			MessageBox(NULL, L"Could not create Dialog Window", L"CreateDialog", MB_ICONERROR);
		}
		
		ShowWindow(dialog, nCmdShow);
		UpdateWindow(dialog);

		MSG msg;
		while (GetMessage(&msg, 0, 0, 0)){
			GetDlgItemText(dialog, IDC_EDIT1, dsn, maxlen);
			GetDlgItemText(dialog, IDC_EDIT2, server, maxlen);
			GetDlgItemText(dialog, IDC_EDIT3, port, maxlen);
			GetDlgItemText(dialog, IDC_EDIT4, username, maxlen);
			GetDlgItemText(dialog, IDC_EDIT5, password, maxlen);
		}

		wstring dsnString = L"DSN=" + wstring(dsn) + L";SERVER=" + wstring(server) + L";PORT=" + wstring(port) + L";UID=" + wstring(username) + L";PWD=" + wstring(password) + L";";
		int retCode = SQLDriverConnectW(hdlConn, NULL, (SQLWCHAR*)dsn, SQL_NTS, (SQLTCHAR*)strConnectOut, 5000, &iStrMaxCount, SQL_DRIVER_NOPROMPT);
		//MessageBox(hwnd, L"Hello", L"Caption", MB_OKCANCEL);
		return -1;
	}
	*/
	