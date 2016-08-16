#include "SqliteDBAccess.h"
#include "Log.h"

SqliteDBAccess::SqliteDBAccess()
{
    sqlite = NULL;
}


int SqliteDBAccess::ExecuteCallback(void* ctx, int cols, char** colsTexts, char** colsNames)
{
    CallbackContext* callCtx = (CallbackContext*) ctx;
    return callCtx->obj->ExecuteCallback(cols, colsTexts, colsNames, callCtx->response);
}


int SqliteDBAccess::ExecuteCallback(int cols, char** colsTexts, char** colsNames, RowSet* response)
{
	//for( int c = 0;  c < cols; ++c )
	//{
	//	if( char* txt = colsTexts[c] )
	//		cout << txt;
	//}
	//cout << endl;
	//
	//for( int c = 0;  c < cols; ++c )
	//{
	//	if( char* name = colsNames[c] )
	//		cout << name;
	//}
	//cout << endl;
	//
	return 0;
}


bool SqliteDBAccess::Connect(const std::string& connectionString)
{
    Disconnect();
	int err = sqlite3_open("sqlitest.dat", &sqlite);
    return err == 0 && sqlite;
}


bool SqliteDBAccess::Disconnect()
{
    if( sqlite )
    {
        sqlite3_close(sqlite);
        sqlite = NULL;
    }
    return true;
}


bool SqliteDBAccess::Execute(const std::string& command, RowSet* response)
{
    char* errMsg;
    CallbackContext ctx = { this, response };
    int err = sqlite3_exec(sqlite, command.c_str(), 
            ExecuteCallback, &ctx, &errMsg);
    if( errMsg )
    {
        Log("Error: %s", errMsg);
        sqlite3_free(errMsg);
    }
    return err == 0;
}

