/** Implementação de oledb da interface genérica de acesso a banco de dados.

@author Wanderley Caloni <wanderley.caloni@bitforge.com.br>
@date 2014-05
*/
#pragma once
#include "DBAccess.h"
#include <atlbase.h>
#include <oledb.h>
#include <msdasc.h>
#include <map>
#include <vector>
#include <string>

#pragma pack(push, 1)
	struct OleDbData
	{
		WCHAR* value;
	};
#pragma pack(pop)

class OleDBAccess : public DBAccess
{
public:
    OleDBAccess();
	bool Connect(const std::string& connectionString);
	bool Disconnect();
	bool Execute(const std::string& command, RowSet* response = 0);

private:
	typedef std::map<int, std::string> ColumnMap;

	IMalloc* GetAllocator();
	bool ExecuteRead(ICommandText* iCommand, RowSet& response);
	bool ReadColumnInfo(IRowset* rowSet, RowSet& response, ColumnMap& colMap);
	bool CreateBindingsAndAccessor(IAccessor* accessorFactory, ColumnMap& colMap, 
		DBBINDING** pBindings, HACCESSOR* accessor, OleDbData** data);

	IDataInitialize* pIDataInitialize;
	IDBInitialize* pIDBInitialize;
	IDBCreateSession* pIDBCreateSession;
	IDBCreateCommand* pIDBCreateCommand;
};
