# Biblioteca simples de acesso a banco de dados que usa instru��es SQL.

Quantas vezes voc� pesquisou qual a melhor lib c++ para acessar uma base de dados SQL e descobriu que todas elas s�o irritantemente complexas ou dif�ceis de configurar? Qual delas foi t�o simples quanto copiar um arquivo-fonte para seu projeto e sair usando?

O OLEDB n�o � muito mais conhecido hoje em dia, mas se seu objetivo � acessar uma base SQL em C++ em Windows, essa lib fornece exatamente isso, retornando no lugar um cont�iner SQL de resultado.

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

Para compilar, basta copiar os arquivos OleDBAccess.h e suas implementa��es (OLEDB e sqlite, lembrando que sqlite depende totalmente do projeto sqlite; mas se voc� est� usando sqlite, para qu� voc� precisa de outra lib? =).

# Sobre o autor

Wanderley Caloni � s�cio-desenvolvedor da BitForge, uma empresa especializada em escovar bits e que � al�rgica a fontes dif�ceis de manter e solu��es complicadas demais para o problema que se quer resolver.

