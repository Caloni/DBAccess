#include "DBAccess.h"
#include "Args.h"
#include "Log.h"
#include <iostream>
#include <string>

using namespace std;

void Usage()
{
	Log("DBAccess v.", __DATE__, " ", __TIME__);
	Log("2014-05 Wanderley Caloni <wanderley.caloni@bitforge.com.br)");
	Log("Usage: DBAccess --db <database> --conn-str <connection-string> --query <sql-query> --cmd <sql-cmd>");
	Log("--db (required) sqlite, oledb");
	Log("--conn-str (required) Connection string (database dependent)");
	Log("--query SQL query with results");
	Log("--cmd SQL query without results");
}

int Execute(DBAccess* db, const string& query, bool getResults)
{
	int ret = -1;
	DBAccess::RowSet results;

	if (db->Execute(query, getResults ? &results : nullptr))
	{
		ret = results.size();

		if (getResults)
			Log("Query executed successfully! ", ret, " results");
		else
			Log("Query executed successfully!");

		if (results.size())
		{
			DBAccess::Rows& rows = (*results.begin()).second;

			for (auto it = results.begin(); it != results.end(); ++it)
			{
				cout << it->first << ' ';
			}
			cout << endl;

			for (size_t i = 0; i < rows.size(); ++i)
			{
				for (auto it = results.begin(); it != results.end(); ++it)
				{
					cout << it->second[i] << ' ';
				}
				cout << endl;
			}
			cout << endl;
		}
	}
	else Loge("Error executing query '", query, "'");

	return ret;
}

int main(int argc, char* argv[])
{
	int ret = 1;
	Args args;
	ParseCommandLine(argc, argv, args);

	if (args.find("--db") != args.end() && args.find("--conn-str") != args.end() )
	{
		string dbType = args["--db"];

		if (DBAccess* db = DBAccess::CreateDBAccess(dbType))
		{
			string connStr = args["--conn-str"];

			if (db->Connect(connStr))
			{
				Log("Database connected successfully!");

				if (args.find("--query") != args.end())
					ret = Execute(db, args["--query"], true);
				else if (args.find("--cmd") != args.end())
					ret = Execute(db, args["--cmd"], false);

				db->Disconnect();
				ret = 0;
			}
			else Loge("Error connecting to database using string '", connStr, "'");
		}
		else Loge("Error creating database");
	}
	else Usage();

	return ret;
}
