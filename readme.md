# Biblioteca simples de acesso a banco de dados que usa instruções SQL.

Quantas vezes você pesquisou qual a melhor lib c++ para acessar uma base de dados SQL e descobriu que todas elas são irritantemente complexas ou difíceis de configurar? Qual delas foi tão simples quanto copiar um arquivo-fonte para seu projeto e sair usando?

O OLEDB não é muito mais conhecido hoje em dia, mas se seu objetivo é acessar uma base SQL em C++ em Windows, essa lib fornece exatamente isso, retornando no lugar um contêiner SQL de resultado.

Para sair usando, basta fazer algo como isso:

```
#include "DBAccess.h"

if (DBAccess* db = DBAccess::CreateDBAccess("oledb"))
{
	if (db->Connect("connection string"))
	{
		DBAccess::RowSet results;
		ret = Execute(db, "select * from table", &results);
		DoSomething(results["columnA"]);
		db->Disconnect();
	}
}
```

Para compilar, basta copiar os arquivos OleDBAccess.h e suas implementações (OLEDB e sqlite, lembrando que sqlite depende totalmente do projeto sqlite; mas se você está usando sqlite, para quê você precisa de outra lib? =).

# Sobre o autor

Wanderley Caloni é sócio-desenvolvedor da BitForge, uma empresa especializada em escovar bits e que é alérgica a fontes difíceis de manter e soluções complicadas demais para o problema que se quer resolver.

