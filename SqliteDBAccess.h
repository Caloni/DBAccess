/** Implementação de sqlite da interface genérica de acesso a banco de dados.

@author Wanderley Caloni <wanderley.caloni@bitforge.com.br>
@date 2014-05

@remark Há a dependência da biblioteca do sqlite.
*/
#pragma once
#include "DBAccess.h"
#include <sqlite3.h>
#include <map>
#include <vector>
#include <string>


class SqliteDBAccess : public DBAccess
{
public:
	typedef std::map<std::string, std::vector<std::string> > RowSet;

	SqliteDBAccess();
	bool Connect(const std::string& connectionString);
	bool Disconnect();
	bool Execute(const std::string& command, RowSet* response = 0);

private:
    struct CallbackContext { SqliteDBAccess* obj; RowSet* response; };

    static int ExecuteCallback(void* ctx, int cols, char** colsTexts, char** colsNames);
    int ExecuteCallback(int cols, char** colsTexts, char** colsNames, RowSet* response);

	sqlite3* sqlite;
};

