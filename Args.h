/** Interpreta argumentos da linha de comando.

@author Wanderley Caloni <wanderley.caloni@bitforge.com.br>
@date 2015-06
*/
#pragma once
#include <map>
#include <string>

typedef std::map<std::string, std::string> Args;

/// Processa argumentos de entrada do programa e retorna mapa chave => valor.
void ParseCommandLine(int argc, char* argv[], Args& args);
