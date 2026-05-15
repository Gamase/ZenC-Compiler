#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "token.h"
#include "ast.h"
#include <memory>

class Parser{
private:
    std::vector<Token> tokens;
    int posicion;

    Token tokenActual();
    Token siguienteToken();
    void avanzar();
    void consumir(Tipo_de_Token tipoEsperado);
    void error(Tipo_de_Token esperado);
    std::vector<std::unique_ptr<Stmt>> programa();
    void lista_sentencias();
    std::unique_ptr<Stmt> declaracion();
    std::unique_ptr<Stmt> sentencia();
    std::unique_ptr<Stmt> asignacion();
    std::unique_ptr<Stmt> bloque();

    std::unique_ptr<Expr> expresion();
    std::unique_ptr<Expr> expresion_logica_or();
    std::unique_ptr<Expr> expresion_logica_and();
    std::unique_ptr<Expr> expresion_relacional();
    std::unique_ptr<Expr> expresion_aritmetica();
    std::unique_ptr<Expr> termino();
    std::unique_ptr<Expr> factor();

    void tipo();

    std::unique_ptr<Stmt> sentencia_if();
    std::unique_ptr<Stmt> sentencia_while();
    std::unique_ptr<Stmt> procedimientos(); 
    std::unique_ptr<Stmt> llamada_procedimientos();
    std::unique_ptr<Stmt> sentencia_for();
    std::unique_ptr<Stmt> asignacion_for();
    std::unique_ptr<Stmt> sentencia_print();
    std::unique_ptr<Stmt> sentencia_input();
    std::unique_ptr<Stmt> incremento_decremento();

public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();
};




#endif