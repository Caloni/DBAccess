#include "Log.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <Windows.h>


std::string WinErrorDesc(DWORD errNumber)
{
    std::string ret;
    bool msgOk = false;
    LPVOID lpMsgBuf = NULL;

    if (errNumber == ERROR_SUCCESS)
        errNumber = GetLastError();

    if( FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER // aloque pra mim (não sei o tamanho)
        | FORMAT_MESSAGE_FROM_SYSTEM // descrição do erro está no sistema
        | FORMAT_MESSAGE_IGNORE_INSERTS, // ignora os inserts pra não sofrer com hackerzinhos
        NULL, // sem fonte:
        errNumber, // a fonte é o código de erro
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // idioma padrão
        (LPSTR)&lpMsgBuf,// isso é um ponteiro para um ponteiro para um buffer que será alocado
        0, // nada disso
        NULL // e nem disso
        ) > 0 ) // maior que zero quer dizer "beleza!"
    {
        if( lpMsgBuf ) // só pra...
        {
            ret = (PCSTR) lpMsgBuf; // ok, vamos usar essa string
            msgOk = true;
            LocalFree(lpMsgBuf); // não precisamos mais da memória alocada
        }
    }

    if( ! msgOk ) // alguma coisa não deu certo
    {
        char msgBuf[100]; // o suficiente; sempre
        sprintf_s(msgBuf, "Unknown error (code %d)", errNumber);
        ret = msgBuf;
    }

    return ret;
}


std::string GetModuleName()
{
	CHAR buffer[MAX_PATH] = {};
	PSTR ret = buffer;
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	if (char* path = strrchr(buffer, '\\'))
		ret = path + 1;
	if (char* ext = strrchr(buffer, '.'))
		*ext = 0;
	return ret;
}

void LogHeader(std::ostringstream& os)
{
    DWORD lastError = GetLastError();
	SYSTEMTIME st;
    static size_t st_lineCounter;
	char buffer[100] = "";
	std::string module = GetModuleName();

	GetLocalTime(&st);

	sprintf_s(buffer, "%08X %04d-%02d-%02d %02d:%02d:%02d %04X.%04X %08X %s ",
        InterlockedIncrement(&st_lineCounter),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
		GetCurrentProcessId() & 0xFFFF, GetCurrentThreadId() & 0xFFFF, lastError,
		module.c_str());

	os << buffer;
}

void Log(std::ostringstream& os)
{
	std::cout << os.str() << std::endl;
}

void Loge(std::ostringstream& os)
{
	std::cerr << os.str() << std::endl;
}

void ReconfigureLogOutputs(const std::string& outputLogFilePath)
{
    static std::ofstream ofs(outputLogFilePath.c_str(), std::ofstream::out | std::ofstream::app);
    std::cout.rdbuf(ofs.rdbuf());
    std::cerr.rdbuf(ofs.rdbuf());
}


std::string GetWindowsErrorDescription(DWORD errNumber)
{
    std::string ret;
    bool msgOk = false;
    LPVOID lpMsgBuf = NULL;

    if( FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER // aloque pra mim (não sei o tamanho)
        | FORMAT_MESSAGE_FROM_SYSTEM // descrição do erro está no sistema
        | FORMAT_MESSAGE_IGNORE_INSERTS, // ignora os inserts pra não sofrer com hackerzinhos
        NULL, // sem fonte:
        errNumber, // a fonte é o código de erro
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // idioma padrão
        (LPSTR)&lpMsgBuf,// isso é um ponteiro para um ponteiro para um buffer que será alocado
        0, // nada disso
        NULL // e nem disso
        ) > 0 ) // maior que zero quer dizer "beleza!"
    {
        if( lpMsgBuf ) // só pra...
        {
            // tira nova linha que windows coloca
            if (char* nl = strrchr((char*)lpMsgBuf, '\r'))
                *nl = 0;
            if (char* nl = strrchr((char*)lpMsgBuf, '\n'))
                *nl = 0;

            ret = (PCSTR) lpMsgBuf; // ok, vamos usar essa string
            msgOk = true;
            LocalFree(lpMsgBuf); // não precisamos mais da memória alocada
        }
    }

    if( ! msgOk ) // alguma coisa não deu certo
    {
        char msgBuf[100]; // o suficiente
        _snprintf_s(msgBuf, 100, "Unknown error (code %d)", errNumber);
        ret = msgBuf;
    }

    return ret;
}
