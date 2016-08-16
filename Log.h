/** Biblioteca mais simples de log. Copiar e colar.

@author Wanderley Caloni <wanderley.caloni@bitforge.com.br>
@date 2015-07
*/
#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <Windows.h>


std::string WinErrorDesc(DWORD errNumber = ERROR_SUCCESS);
std::string GetModuleName();
void LogHeader(std::ostringstream& os);
void Log(std::ostringstream& os);
void Loge(std::ostringstream& os);
void ReconfigureLogOutputs(const std::string& outputLogFilePath);
std::string GetWindowsErrorDescription(DWORD errNumber = GetLastError());


template<typename First, typename...Rest >
void Log(std::ostringstream& os, First parm1, Rest...parm)
{
	os << parm1;
	Log(os, parm...);
}

template<typename...Rest >
void Log(Rest...parm)
{
	std::ostringstream os;
	LogHeader(os);
	Log(os, parm...);
}


template<typename First, typename...Rest >
void Loge(std::ostringstream& os, First parm1, Rest...parm)
{
	os << parm1;
	Loge(os, parm...);
}

template<typename...Rest >
void Loge(Rest...parm)
{
	std::ostringstream os;
	LogHeader(os);
	Loge(os, parm...);
}
