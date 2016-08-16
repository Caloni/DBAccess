#include "DBAccess.h"
#include "OleDBAccess.h"
#include "SqliteDBAccess.h"


DBAccess* DBAccess::CreateDBAccess(const std::string& database)
{
    if( database == "oledb" )
        return new OleDBAccess();
    else if( database == "sqlite" )
        return new SqliteDBAccess();
    return NULL;
}

