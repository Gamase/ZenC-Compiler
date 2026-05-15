#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>

struct Instruccion {
    std::string resultado;
    std::string arg1;
    std::string operador;
    std::string arg2;
    std::string tipo;  // "int", "float", ""
};

class GeneradorCodigo {
private:
    std::vector<Instruccion> instrucciones;
    std::map<std::string, std::string> tipoVariables; // nombre -> tipo
    int tempContador;
    int etiquetaContador;

    std::string nuevoTemp();
    std::string nuevaEtiqueta();
    void emitir(Instruccion instr);

    std::string generarExpr(Expr* expr, std::string& tipoResultado);
    void generarStmt(Stmt* stmt);

public:
    GeneradorCodigo();
    void generar(const std::vector<Stmt*>& programa);
    void imprimir();

    std::vector<Instruccion> getInstrucciones() const {
        return instrucciones;
    }
};

#endif