/** Biblioteca simples de acesso a banco de dados que usa instru��es SQL.

Atualmente suporta OLEDB e Sqlite.

@author Wanderley Caloni <wanderley.caloni@bitforge.com.br>
@date 2014-05
*/
#pragma once
#include <map>
#include <vector>
#include <string>


class DBAccess
{
public:
	/// Factory para base de dados espec�fica.
	/// @param database Pode ser "sqlite" ou "oledb".
	static DBAccess* CreateDBAccess(const std::string& database);

	typedef std::string ColumnName;
	typedef std::vector<std::string> Rows;
	typedef std::map<ColumnName, Rows> RowSet;

	virtual ~DBAccess() { }

	virtual bool Connect(const std::string& connectionString) = 0;
	virtual bool Disconnect() = 0;

	/// Executa uma instru��o SQL e retorna opcionalmente os resultados (se for uma query).
	virtual bool Execute(const std::string& command, RowSet* response = 0) = 0;
};
