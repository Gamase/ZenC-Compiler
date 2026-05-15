#ifndef ASMGEN_H
#define ASMGEN_H

#include "codegen.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>

class GeneradorASM {
private:
    std::vector<Instruccion> instrucciones;
    std::map<std::string, int> offsetVariables;
    std::map<std::string, std::string> tipoVariables;
    std::map<std::string, int> procStackSizes;
    std::set<std::string> variablesGlobales;
    int stackOffset;

    std::string operandoAASM(const std::string& op);
    bool esFloat(const std::string& nombre);
    void emitirInstruccion(std::ostringstream& out,
                           const Instruccion& ins,
                           std::map<std::string, std::string>& cadenaLabels,
                           std::map<std::string, std::string>& floatLabels);

public:
    GeneradorASM(const std::vector<Instruccion>& instrucciones);
    void generarArchivo(const std::string& nombreArchivo);
};

#endif