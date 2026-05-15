#include <iostream>
#include <stdexcept>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/interprete.h"
#include "../include/tabla.h"
#include "../include/semantica.h"
#include "../include/codegen.h"
#include "../include/asmgen.h"

using namespace std;

int main()
{
    try {
        Lexer lexer("input.txt");
        vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        auto programa = parser.parse();

        // Analisis Semantico
        Entorno envSemantico;
        for (auto& stmt : programa) {
            analizarStmt(stmt.get(), envSemantico);
        }

        if (!obtenerErroresSem().empty()) {
            cerr << "\n";
            for (auto& e : obtenerErroresSem())
                cerr << "Error semantico: " << e << "\n";
            cerr << obtenerErroresSem().size()
                 << " error(es) semantico(s). Compilacion abortada.\n";
            return 1;
        }

        // Codigo intermedio
        std::vector<Stmt*> rawStmts;
        for (auto& s : programa) {
            rawStmts.push_back(s.get());
        }
        GeneradorCodigo codegen;
        codegen.generar(rawStmts);
        codegen.imprimir();

        // Generador ASM
        GeneradorASM asmgen(codegen.getInstrucciones());
        asmgen.generarArchivo("output.asm");

        // Interprete
        Entorno envEjecucion;
        for (auto& stmt : programa) {
            ejecutarStmt(stmt.get(), envEjecucion);
        }

        cout << "\nEjecucion finalizada.\n";

    } catch (const std::runtime_error& e) {
        cerr << "\n" << e.what() << "\n";
        return 1;
    }

    return 0;
}
