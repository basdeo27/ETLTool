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

	const int maxlen = 256;
	TCHAR driver[maxlen];
	TCHAR host[maxlen];
	TCHAR port[5];
	TCHAR database[maxlen];
	int hiveServerType = 0;
	int mechanism = 0;
	TCHAR realm[maxlen];
	TCHAR hostFqdn[maxlen];
	TCHAR serviceName[maxlen];
	TCHAR httpPath[maxlen];
	TCHAR username[maxlen];
	TCHAR password[maxlen];
	TCHAR delegationUID[maxlen];
	TCHAR strConnectOut[5000];
	SQLSMALLINT	iStrMaxCount = 0;

	TCHAR HiveServer[2][14] =
	{
		TEXT("Hive Server 1"), TEXT("Hive Server 2")
	};

	TCHAR Mechanisms[11][33] =
	{
		TEXT("No Authentication"), TEXT("Kerberos"), TEXT("User Name"), TEXT("User Name and Password"),
		TEXT("User Name and Password (SSL)"), TEXT("Windows Azure HDInsight Emulator"), TEXT("Windows Azure HDInsight Service"), TEXT("HTTP"),
		TEXT("HTTPS"), TEXT("Kerberos over HTTP"), TEXT("Kerberos over HTTPS")
	};

	void clearWindows(HWND hWndDlg) {
		SetDlgItemText(hWndDlg, IDC_EDIT5, L"");
		SetDlgItemText(hWndDlg, IDC_EDIT6, L"");
		SetDlgItemText(hWndDlg, IDC_EDIT7, L"");
		SetDlgItemText(hWndDlg, IDC_EDIT8, L"");
		SetDlgItemText(hWndDlg, IDC_EDIT9, L"");
		SetDlgItemText(hWndDlg, IDC_EDIT10, L"");
	}

	BOOL enableWindows(HWND hWndDlg, int realm, int fqdn, int serviceName, int httpPath, int userName, int password){
		EnableWindow(GetDlgItem(hWndDlg, IDC_EDIT5), realm);
		EnableWindow(GetDlgItem(hWndDlg, IDC_EDIT6), fqdn);
		EnableWindow(GetDlgItem(hWndDlg, IDC_EDIT7), serviceName);
		EnableWindow(GetDlgItem(hWndDlg, IDC_EDIT8), httpPath);
		EnableWindow(GetDlgItem(hWndDlg, IDC_EDIT9), userName);
		EnableWindow(GetDlgItem(hWndDlg, IDC_EDIT10), password);

		return TRUE;
	}

	BOOL getAllDialogsText(HWND hWndDlg) {
		HWND hCombo = GetDlgItem(hWndDlg, IDC_COMBO1);  /* Get combo box window handle. */
		hiveServerType = SendMessage(hCombo, CB_GETCURSEL, (WORD)0, 0L);
		hCombo = GetDlgItem(hWndDlg, IDC_COMBO2);  /* Get combo box window handle. */
		mechanism = SendMessage(hCombo, CB_GETCURSEL, (WORD)0, 0L);

		GetDlgItemText(hWndDlg, IDC_EDIT1, driver, maxlen);
		GetDlgItemText(hWndDlg, IDC_EDIT2, host, maxlen);
		GetDlgItemText(hWndDlg, IDC_EDIT3, port, 5);
		GetDlgItemText(hWndDlg, IDC_EDIT4, database, maxlen);
		GetDlgItemText(hWndDlg, IDC_EDIT5, realm, maxlen);
		GetDlgItemText(hWndDlg, IDC_EDIT6, hostFqdn, maxlen);
		GetDlgItemText(hWndDlg, IDC_EDIT7, serviceName, maxlen);
		GetDlgItemText(hWndDlg, IDC_EDIT8, httpPath, maxlen);
		GetDlgItemText(hWndDlg, IDC_EDIT9, username, maxlen);
		GetDlgItemText(hWndDlg, IDC_EDIT10, password, maxlen);
		GetDlgItemText(hWndDlg, IDC_EDIT11, delegationUID, maxlen);

		return TRUE;
	}

	BOOL enableOk(HWND hWndDlg){
		getAllDialogsText(hWndDlg);
		if (wstring(driver).size() == 0 || wstring(host).size() == 0 || wstring(port).size() == 0){
			return FALSE;
		}
		switch (mechanism){
		case 0:
			return TRUE;
		case 1:
			return wstring(realm).size() != 0 && wstring(hostFqdn).size() != 0 && wstring(serviceName).size() != 0;
		case 2:
			return wstring(username).size() != 0;
		case 3:
		case 4:
			return wstring(username).size() != 0 && wstring(password).size() != 0;
		case 5:
		case 6:
		case 7:
		case 8:
			return wstring(httpPath).size() != 0 && wstring(username).size() != 0 && wstring(password).size() != 0;
		case 9:
		case 10:
			return wstring(realm).size() != 0 && wstring(hostFqdn).size() != 0 && wstring(serviceName).size() != 0 && wstring(httpPath).size() != 0;
		}
	}

	BOOL CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
	{

		switch (Msg)
		{
		case WM_INITDIALOG:
		{
			HWND hiveServerType = GetDlgItem(hWndDlg, IDC_COMBO1);
			for (int i = 0; i < 2; i++) {
				//wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)HiveServer[i]);
				SendMessage(hiveServerType, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)((TCHAR*)HiveServer[i]));
			}
			SendMessage(hiveServerType, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

			HWND mechanism = GetDlgItem(hWndDlg, IDC_COMBO2);
			for (int i = 0; i < 1; i++) {
				//wcscpy_s(A, sizeof(A) / sizeof(TCHAR), (TCHAR*)Mechanisms[i]);
				SendMessage(mechanism, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)((TCHAR*)Mechanisms[i]));
			}
			SendMessage(mechanism, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			EnableWindow(GetDlgItem(hWndDlg, IDOK), FALSE);
			return TRUE;
		}
		case WM_COMMAND:
			int newHiveServerType = 0;
			WORD comboBox = LOWORD(wParam);
			switch (wParam)
			{
			case IDOK:
				getAllDialogsText(hWndDlg);
				EndDialog(hWndDlg, 0);
				return TRUE;
			case IDCANCEL:
				EndDialog(hWndDlg, 0);
				return TRUE;
			case IDC_BUTTON1:
				getAllDialogsText(hWndDlg);
				return TRUE;
			}
			switch (HIWORD(wParam)) 
			{
			case CBN_SELCHANGE:
				HWND hCombo;
				
				switch (comboBox)
				{
				case IDC_COMBO1:
					hCombo = GetDlgItem(hWndDlg, IDC_COMBO1);
					newHiveServerType = SendMessage(hCombo, CB_GETCURSEL, (WORD)0, 0L);
					if (newHiveServerType != hiveServerType) {
						hCombo = GetDlgItem(hWndDlg, IDC_COMBO2);  /* Get combo box window handle. */
						if (newHiveServerType == 0){
							SendMessage(hCombo, (UINT)CB_RESETCONTENT, (WPARAM)0, (LPARAM) 0);
							SendMessage(hCombo, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)((TCHAR*)Mechanisms[0]));
							SendMessage(hCombo, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
							clearWindows(hWndDlg);
							enableWindows(hWndDlg, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
						}
						else {
							for (int i = 1; i < 11; i++) {
								SendMessage(hCombo, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)((TCHAR*)Mechanisms[i]));
							}
						}
						hiveServerType = newHiveServerType;
					}
					EnableWindow(GetDlgItem(hWndDlg, IDOK), enableOk(hWndDlg));
					return TRUE;
				case IDC_COMBO2:
					EnableWindow(GetDlgItem(hWndDlg, IDOK), enableOk(hWndDlg));
					hCombo = GetDlgItem(hWndDlg, IDC_COMBO2);
					int index = SendMessage(hCombo, CB_GETCURSEL, (WORD)0, 0L);
					switch (index){
					case 0:
						enableWindows(hWndDlg, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE);
						break;
					case 1:
						enableWindows(hWndDlg, TRUE, TRUE, TRUE, FALSE, FALSE, FALSE);
						break;
					case 2:
						enableWindows(hWndDlg, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE);
						break;
					case 3:
					case 4:
						enableWindows(hWndDlg, FALSE, FALSE, FALSE, FALSE, TRUE, TRUE);
						break;
					case 5:
					case 6:
					case 7:
					case 8:
						enableWindows(hWndDlg, FALSE, FALSE, FALSE, TRUE, TRUE, TRUE);
						break;
					case 9:
					case 10:
						enableWindows(hWndDlg, TRUE, TRUE, TRUE, TRUE, FALSE, FALSE);
						break;
					}
					return TRUE;
				}
			case EN_CHANGE:
				EnableWindow(GetDlgItem(hWndDlg, IDOK), enableOk(hWndDlg));
				return TRUE;
			}
			return TRUE;
		
		}

		return FALSE;
	}

	wstring getConnectionString() {
		wstring connectionString = L"Driver=" + wstring(driver) + L";Host=" + wstring(host) + L";Port=" + wstring(port)+ L";";
		wstring databaseString = wstring(database);
		if (databaseString.size() > 0)
			connectionString.append(L"Database=" + databaseString + L";");
		if (hiveServerType == 0){
			connectionString.append(L"HiveServerType=1;");
			return connectionString;
		}
		connectionString.append(L"AuthMech=" + to_wstring(mechanism) + L";");
		switch (mechanism){
		case 1:
			connectionString.append(L"KrbRealm=" + wstring(realm) + L";");
			connectionString.append(L"KrbHostFQDN=" + wstring(hostFqdn) + L";");
			connectionString.append(L"KrbServiceName=" + wstring(serviceName) + L";");
			break;
		case 2:
			connectionString.append(L"UID=" + wstring(username) + L";");
			break;
		case 3:
		case 4:
			connectionString.append(L"UID=" + wstring(username) + L";");
			connectionString.append(L"PWD=" + wstring(password) + L";");
			break;
		case 5:
		case 6:
		case 7:
		case 8:
			connectionString.append(L"HttpPath=" + wstring(httpPath) + L";");
			connectionString.append(L"UID=" + wstring(username) + L";");
			connectionString.append(L"PWD=" + wstring(password) + L";");
			break;
		case 9:
		case 10:
			connectionString.append(L"KrbRealm=" + wstring(realm) + L";");
			connectionString.append(L"KrbHostFQDN=" + wstring(hostFqdn) + L";");
			connectionString.append(L"KrbServiceName=" + wstring(serviceName) + L";");
			connectionString.append(L"HttpPath=" + wstring(httpPath) + L";");
			break;
		default:
			break;
		}
		return connectionString;
	}
	
	short HandleStatement::hiveDriverConnect() {
		int nCmdShow = 5;
		HWND hwndOwner = GetDesktopWindow();
		DialogBoxW(GetModuleHandle(L"testLibrary.dll"),                   // application instance
			MAKEINTRESOURCE(IDD_DIALOG1), // dialog box resource
			hwndOwner,                          // owner window
			(DLGPROC) DlgProc                    // dialog box window procedure
		);
		wstring connectionString = getConnectionString();

		int retCode = SQLDriverConnectW(hdlConn, NULL, (SQLWCHAR*)connectionString.c_str(), SQL_NTS, (SQLWCHAR*)strConnectOut, 5000, &iStrMaxCount, SQL_DRIVER_NOPROMPT);
		if (retCode)
			MessageBox(NULL, (LPCWSTR)L"Failed to Connect", (LPCWSTR)L"Connection Error", MB_ICONWARNING | MB_OK);
		return retCode;
	}
	
	