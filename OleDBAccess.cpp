#define _CRT_SECURE_NO_WARNINGS
#include "OleDBAccess.h"
#include "Log.h"
#include <iostream>
#include <sstream>

using namespace std;



OleDBAccess::OleDBAccess()
{
	pIDataInitialize = NULL;
	pIDBInitialize = NULL;
	pIDBCreateSession = NULL;
	pIDBCreateCommand = NULL;
}


bool OleDBAccess::Connect(const std::string& connectionString)
{
	HRESULT hr = CoInitialize(NULL);
	if( hr != S_OK )
		Log("Warning: CoInitialize não foi chamado com sucesso");

	Disconnect();

	hr = CoCreateInstance(
		__uuidof(MSDAINITIALIZE), NULL, CLSCTX_ALL,
		__uuidof(IDataInitialize), (void**)&pIDataInitialize);
	if( hr != S_OK )
	{
		Log("Erro %08X criando instância de IDataInitialize.", hr);
		return false;
	}

	hr = pIDataInitialize->GetDataSource(NULL, CLSCTX_ALL,
		ATL::CA2W(connectionString.c_str()),
		__uuidof(IDBInitialize), (IUnknown**)&pIDBInitialize);
	if( hr != S_OK )
	{
		Log("Erro %08X criando instância de IDBInitialize.", hr);
		return false;
	}

	hr = pIDBInitialize->Initialize();
	if( hr != S_OK )
	{
		Log("Erro %08X inicializando banco.", hr);
		return false;
	}

	hr = pIDBInitialize->QueryInterface(
		IID_IDBCreateSession, (void**)&pIDBCreateSession);
	if( hr != S_OK )
	{
		Log("Erro %08X obtendo interface IID_IDBCreateSession.", hr);
		return false;
	}

	hr = pIDBCreateSession->CreateSession(NULL,
		IID_IDBCreateCommand,
		(IUnknown**)&pIDBCreateCommand);
	if( hr != S_OK )
	{
		Log("Erro %08X criando sessão.", hr);
		return false;
	}

	Log("Conexão com o banco estabelecida com sucesso!");
	return true;
}


bool OleDBAccess::Disconnect()
{
	if( pIDBCreateSession )
	{
		pIDBCreateSession->Release();
		pIDBCreateSession = NULL;
	}
	if( pIDBCreateCommand )
	{
		pIDBCreateCommand->Release();
		pIDBCreateCommand = NULL;
	}
	if( pIDBInitialize )
	{
		pIDBInitialize->Release();
		pIDBInitialize = NULL;
	}
	if( pIDataInitialize )
	{
		pIDataInitialize->Release();
		pIDataInitialize = NULL;
	}
	Log("Desconectando do banco");
	return true;
}


IMalloc* OleDBAccess::GetAllocator()
{
	IMalloc* allocator = NULL;
	HRESULT hr = CoGetMalloc(1, &allocator);
	return allocator;
}


bool OleDBAccess::ReadColumnInfo(IRowset* rowSet, RowSet& response, ColumnMap& colMap)
{
	bool ret = false;
	DBORDINAL colsCount = 0;
	IColumnsInfo* colInfo = NULL;
	IMalloc* allocator = GetAllocator();
	HRESULT hr = rowSet->QueryInterface(&colInfo);

	if( SUCCEEDED(hr) && colInfo && allocator )
	{
		DBCOLUMNINFO* cols = NULL;
		OLECHAR* colStrings = NULL;
		hr = colInfo->GetColumnInfo(&colsCount, &cols, &colStrings);

		if( SUCCEEDED(hr) )
		{
			for( DBORDINAL c = 0; c < colsCount; ++c )
			{
				DBCOLUMNINFO col = cols[c];
				string colName(ATL::CW2A(col.pwszName));
				response[colName];
				colMap[c + 1] = colName;
			}

			allocator->Free(cols);
			allocator->Free(colStrings);
			ret = true;
		}
	}

	if( colInfo )
		colInfo->Release();
	if( allocator )
		allocator->Release();

	return ret;
}


bool OleDBAccess::CreateBindingsAndAccessor(IAccessor* accessorFactory, ColumnMap& colMap, 
											DBBINDING** pBindings, HACCESSOR* accessor,
											OleDbData** pData)
{
	bool ret = false;

	if( IMalloc* allocator = GetAllocator() )
	{
		*pBindings = reinterpret_cast<DBBINDING*>( 
			allocator->Alloc(sizeof(DBBINDING) * colMap.size()) );
		*pData = reinterpret_cast<OleDbData*>( 
			allocator->Alloc(sizeof(OleDbData) * colMap.size()) );

		if( *pBindings && *pData )
		{
			DBBINDING* bindings = *pBindings;
			OleDbData* data = *pData;

			memset(data, 0, sizeof(OleDbData) * colMap.size());

			for( ULONG c = 0; c < colMap.size(); ++c )
			{
				bindings[c].iOrdinal = c + 1;
				bindings[c].obValue = (sizeof(OleDbData) * c) + offsetof(OleDbData, value);
				bindings[c].obLength = 0;
				bindings[c].obStatus = 0;
				bindings[c].pTypeInfo = NULL;
				bindings[c].pObject = NULL;
				bindings[c].pBindExt = NULL;
				bindings[c].dwPart = DBPART_VALUE;
				bindings[c].dwMemOwner = DBMEMOWNER_CLIENTOWNED;
				bindings[c].eParamIO = DBPARAMIO_NOTPARAM;
				bindings[c].cbMaxLen = sizeof(void *);
				bindings[c].dwFlags = 0;
				bindings[c].wType = DBTYPE_WSTR | DBTYPE_BYREF;
				bindings[c].bPrecision = 0;
				bindings[c].bScale = 0;
			}

			ret = true;
		}

		if( ret && *pBindings )
		{
			HRESULT hr = accessorFactory->CreateAccessor(DBACCESSOR_ROWDATA, (DBCOUNTITEM) colMap.size(), 
				*pBindings, 0, accessor, NULL);
			ret = ( hr == S_OK );
		}

		if( ! ret )
		{
			if( *pBindings )
			{
				allocator->Free(*pBindings);
				*pBindings = NULL;
			}
			if( *accessor )
			{
				accessorFactory->ReleaseAccessor(*accessor, NULL);
				*accessor = NULL;
			}
			if( *pData )
			{
				allocator->Free(*pData);
				*pData = NULL;
			}
		}
	}

	return ret;
}


bool OleDBAccess::ExecuteRead(ICommandText* iCommand, RowSet& response)
{
	IMultipleResults* results = NULL;
	HRESULT hr = iCommand->Execute(NULL, IID_IMultipleResults, NULL, NULL, (IUnknown**)&results);

	if( SUCCEEDED(hr) && results )
	{
		IRowset* rowSet = NULL;
		hr = results->GetResult(NULL, DBRESULTFLAG_DEFAULT, __uuidof(IRowset), NULL, (IUnknown**)&rowSet);

		if( SUCCEEDED(hr) && rowSet )
		{
			IMalloc* allocator = GetAllocator();
			IAccessor* accessorFactory = NULL;
			hr = rowSet->QueryInterface(&accessorFactory);

			if( allocator && accessorFactory )
			{
				ColumnMap colMap;

				if( ReadColumnInfo(rowSet, response, colMap) )
				{
					DBBINDING* bindings;
					HACCESSOR accessor = NULL;
					OleDbData* data = NULL;

					if( CreateBindingsAndAccessor(accessorFactory, colMap, &bindings, &accessor, &data) )
					{
						HROW row = NULL;
						HROW* pRow = { &row };
						DBCOUNTITEM rows = 0;

						while( (hr = rowSet->GetNextRows(DB_NULL_HCHAPTER, 0, 1, &rows, &pRow)) == S_OK && rows == 1 )
						{
							hr = rowSet->GetData(row, accessor, data);

							if( FAILED(hr) )
								break;

							for( ULONG c = 0; c < colMap.size(); ++c )
							{
								OleDbData d = data[c];
								string colName = colMap[c + 1];
								string colValue = d.value ? CW2A(d.value) : "";
								response[colName].push_back(colValue);
							}

							hr = rowSet->ReleaseRows(1, pRow, NULL, NULL, NULL);
							row = NULL;
							rows = 0;
						}

						accessorFactory->ReleaseAccessor(accessor, NULL);
						allocator->Free(bindings);
						allocator->Free(data);
					}
				}
			}

			if( allocator )
				allocator->Release();
			if( accessorFactory )
				accessorFactory->Release();
			rowSet->Release();
		}
		
		results->Release();
	}

	return SUCCEEDED(hr);
}


bool OleDBAccess::Execute(const std::string& command, RowSet* response)
{
	bool ret = true;
	ICommandText* iCommand = NULL;

	HRESULT hr = pIDBCreateCommand->CreateCommand(NULL, IID_ICommandText, (IUnknown**)&iCommand);

	if( SUCCEEDED(hr) && iCommand )
	{
		PWCHAR cmd = new WCHAR[command.size() + 1];
		mbstowcs(cmd, command.c_str(), command.size() + 1);
		hr = iCommand->SetCommandText(DBGUID_SQL, cmd);
		Log("Execute: %S", cmd);

		if( response )
		{
			ret = ExecuteRead(iCommand, *response);
		}
		else
		{
			hr = iCommand->Execute(NULL, IID_NULL, NULL, NULL, NULL);
			ret = ( hr == S_OK );
		}

		if( ret )
			Log("Sucesso na execução do comando!");

		delete [] cmd;
		iCommand->Release();
	}
	else
	{
		Log("Erro %08X criando comando.", hr);
		ret = false;
	}

	return ret;
}
