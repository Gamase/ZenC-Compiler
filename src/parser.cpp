#include "../include/parser.h"
#include <iostream>
#include <cstdlib>
#include "ast.h"
#include <memory>

using namespace std;

string nombreToken(Tipo_de_Token tipo)
{
    
    switch (tipo)
    {
       case Tipo_de_Token:: INT: return "INT";
       case Tipo_de_Token:: FLOAT: return "FLOAT";
       case Tipo_de_Token:: DOUBLE: return "DOUBLE";
       case Tipo_de_Token:: CHAR: return "CHAR";
       case Tipo_de_Token:: STRING: return "STRING";
       case Tipo_de_Token:: BOOLEAN: return "BOOLEAN";
       case Tipo_de_Token:: VOID: return "VOID";
       case Tipo_de_Token:: IF: return "IF";
       case Tipo_de_Token:: ELSE: return "ELSE";
       case Tipo_de_Token:: FOR: return "FOR";
       case Tipo_de_Token:: WHILE: return "WHILE";
       case Tipo_de_Token:: RETURN: return "RETURN";
       case Tipo_de_Token:: INPUT: return "INPUT";
       case Tipo_de_Token:: PRINT: return "PRINT";
       case Tipo_de_Token:: IDENTIFICADOR: return "IDENTIFICADOR";
       case Tipo_de_Token:: NUMERO: return "NUMERO";
       case Tipo_de_Token:: CADENA_LITERAL: return "CADENA_LITERAL";
       case Tipo_de_Token:: OP_SUMA: return "OP_SUMA";
       case Tipo_de_Token:: OP_RESTA: return "OP_RESTA";
       case Tipo_de_Token:: OP_MULT: return "OP_MULT";
       case Tipo_de_Token:: OP_DIVISION: return "OP_DIVISION";
       case Tipo_de_Token:: OP_MODULO:  return "OP_MODULO";
       case Tipo_de_Token:: OP_MAYORQUE: return "OP_MAYORQUE";
       case Tipo_de_Token:: OP_MENORQUE: return "OP_MENORQUE";
       case Tipo_de_Token:: OP_MAYORIGUAL: return "OP_MAYORIGUAL";
       case Tipo_de_Token:: OP_MENORIGUAL: return "OP_MENORIGUAL";
       case Tipo_de_Token:: OP_IGUALIGUAL: return "OP_IGUALIGUAL";
       case Tipo_de_Token:: OP_DIFERENTE: return "OP_DIFERENTE";
       case Tipo_de_Token:: OP_AND: return "OP_AND";
       case Tipo_de_Token:: OP_OR: return "OP_OR";
       case Tipo_de_Token:: OP_NOT: return "OP_NOT";
       case Tipo_de_Token:: OP_ASIGNACION: return "OP_ASIGNACION";
       case Tipo_de_Token:: DEL_PARENTESIS_IZQ: return "DEL_PARENTESIS_IZQ";
       case Tipo_de_Token:: DEL_PARENTESIS_DER: return "DEL_PARENTESIS_DER";
       case Tipo_de_Token:: DEL_LLAVE_IZQ: return "DEL_LLAVE_IZQ";
       case Tipo_de_Token:: DEL_LLAVE_DER: return "DEL_LLAVE_DER";
       case Tipo_de_Token:: DEL_CORCHETE_IZQ: return "DEL_CORCHETE_IZQ";
       case Tipo_de_Token:: DEL_CORCHETE_DER: return "DEL_CORCHETE_DER";
       case Tipo_de_Token:: DEL_COMA: return "DEL_COMA";
       case Tipo_de_Token:: DEL_PUNTOCOMA: return "DEL_PUNTOCOMA";
       case Tipo_de_Token:: DEL_PUNTOPUNTO: return "DEL_PUNTOPUNTO";
       case Tipo_de_Token:: END_OF_FILE: return "EOF";
       case Tipo_de_Token:: DESCONOCIDO: return "DESCONOCIDO";
       default: return "TOKEN_DESCONOCIDO";

    }
}
//Constructor del Parser
Parser::Parser(const vector<Token>& tokens)
        :tokens(tokens),posicion(0){}

Token Parser::tokenActual(){
    return tokens[posicion];
}

Token Parser::siguienteToken() {

    if (posicion + 1 < tokens.size()) {
        return tokens[posicion + 1];
    }

    return tokens[posicion]; // devolver EOF o el último token
}

void Parser::avanzar(){
    if (posicion < tokens.size())
    posicion++;
}

void Parser::error(Tipo_de_Token esperado) {
    cout << "\nError sintactico en linea "
         << tokenActual().linea
         << ", columna "
         << tokenActual().columna << endl;

    cout << "Se esperaba: "
         << nombreToken(esperado) << endl;

    cout << "Se encontro: "
         << nombreToken(tokenActual().tipo)
         << " (" << tokenActual().lexema << ")"
         << endl;

    exit(1);
}

void Parser::consumir(Tipo_de_Token tipoEsperado) {
    if (tokenActual().tipo == tipoEsperado) {
        avanzar();
    } else {
        error(tipoEsperado);
    }
}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    return programa();
}

//Gramatica

std::vector<std::unique_ptr<Stmt>> Parser::programa() {
    std::vector<std::unique_ptr<Stmt>> stmts;

    while (tokenActual().tipo != Tipo_de_Token::END_OF_FILE) {
        if (tokenActual().tipo == Tipo_de_Token::VOID) {
            stmts.push_back(procedimientos());  
        } else {
            stmts.push_back(sentencia());
        }
    }
    return stmts;
}

void Parser::lista_sentencias(){
    while(tokenActual().tipo != Tipo_de_Token::END_OF_FILE)
    {
        sentencia();
    }
}

std::unique_ptr<Stmt> Parser::bloque(){

    consumir(Tipo_de_Token::DEL_LLAVE_IZQ);

    auto bloque = std::make_unique<Bloque>();

    while(tokenActual().tipo != Tipo_de_Token::DEL_LLAVE_DER &&
          tokenActual().tipo != Tipo_de_Token::END_OF_FILE)
    {
        bloque->sentencias.push_back(sentencia());
    }

    if (tokenActual().tipo == Tipo_de_Token::END_OF_FILE) {
        error(Tipo_de_Token::DEL_LLAVE_DER);
    }

    consumir(Tipo_de_Token::DEL_LLAVE_DER);

    return bloque;
}

std::unique_ptr<Stmt> Parser::sentencia(){

    if (tokenActual().tipo == Tipo_de_Token::DEL_LLAVE_IZQ){
        return bloque();
    }

    if (tokenActual().tipo == Tipo_de_Token::IF) {
        return sentencia_if();
    }

    if (tokenActual().tipo == Tipo_de_Token::WHILE){
        return sentencia_while();
    }

    if (tokenActual().tipo == Tipo_de_Token::FOR){
        return sentencia_for();
    }

    if (tokenActual().tipo == Tipo_de_Token::INT ||
        tokenActual().tipo == Tipo_de_Token::FLOAT ||
        tokenActual().tipo == Tipo_de_Token::DOUBLE ||
        tokenActual().tipo == Tipo_de_Token::CHAR ||
        tokenActual().tipo == Tipo_de_Token::STRING ||
        tokenActual().tipo == Tipo_de_Token::BOOLEAN) {

        return declaracion();
    }

    if (tokenActual().tipo == Tipo_de_Token::PRINT){
        return sentencia_print();
    }

    if (tokenActual().tipo == Tipo_de_Token::INPUT){
        return sentencia_input();
    }

    if (tokenActual().tipo == Tipo_de_Token::IDENTIFICADOR) {

        if (siguienteToken().tipo == Tipo_de_Token::OP_ASIGNACION) {
            return asignacion();
        }
        else if (siguienteToken().tipo == Tipo_de_Token::DEL_PARENTESIS_IZQ) {
            return llamada_procedimientos();
        }
        else if (siguienteToken().tipo == Tipo_de_Token::OP_INCREMENTO ||
                 siguienteToken().tipo == Tipo_de_Token::OP_DECREMENTO) {
            return incremento_decremento();
        }
        else {
            error(tokenActual().tipo);
        }
    }

    error(tokenActual().tipo);
    return nullptr;
}

std::unique_ptr<Stmt> Parser::procedimientos() {
    consumir(Tipo_de_Token::VOID);

    std::string nombre = tokenActual().lexema;
    consumir(Tipo_de_Token::IDENTIFICADOR);

    consumir(Tipo_de_Token::DEL_PARENTESIS_IZQ);
    consumir(Tipo_de_Token::DEL_PARENTESIS_DER);

    auto cuerpo = bloque();

    return std::make_unique<Procedimiento>(nombre, std::move(cuerpo));
}

std::unique_ptr<Stmt> Parser::sentencia_print(){

    consumir(Tipo_de_Token::PRINT);
    consumir(Tipo_de_Token::DEL_PARENTESIS_IZQ);

    auto expr = expresion(); 

    consumir(Tipo_de_Token::DEL_PARENTESIS_DER);
    consumir(Tipo_de_Token::DEL_PUNTOCOMA);

    return std::make_unique<Print>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::sentencia_input(){
    consumir(Tipo_de_Token::INPUT);
    consumir(Tipo_de_Token::DEL_PARENTESIS_IZQ);

    std::string nombre = tokenActual().lexema;
    consumir(Tipo_de_Token::IDENTIFICADOR);

    consumir(Tipo_de_Token::DEL_PARENTESIS_DER);
    consumir(Tipo_de_Token::DEL_PUNTOCOMA);

    return std::make_unique<Input>(nombre);  
}

std::unique_ptr<Stmt> Parser::sentencia_for(){

    consumir(Tipo_de_Token::FOR);
    consumir(Tipo_de_Token::DEL_PARENTESIS_IZQ);

   
    auto init = asignacion_for();
    consumir(Tipo_de_Token::DEL_PUNTOCOMA);

    
    auto cond = expresion();
    consumir(Tipo_de_Token::DEL_PUNTOCOMA);

    
    auto update = asignacion_for();
    consumir(Tipo_de_Token::DEL_PARENTESIS_DER);

    
    auto body = sentencia();

    return std::make_unique<For>(
        std::move(init),
        std::move(cond),
        std::move(update),
        std::move(body)
    );
}

std::unique_ptr<Stmt> Parser::asignacion_for(){

    // Si viene con tipo: int j = 0
    if (tokenActual().tipo == Tipo_de_Token::INT ||
        tokenActual().tipo == Tipo_de_Token::FLOAT ||
        tokenActual().tipo == Tipo_de_Token::DOUBLE ||
        tokenActual().tipo == Tipo_de_Token::CHAR ||
        tokenActual().tipo == Tipo_de_Token::STRING ||
        tokenActual().tipo == Tipo_de_Token::BOOLEAN) {

        std::string tipoStr = tokenActual().lexema;
        tipo(); // consume el tipo

        std::string nombre = tokenActual().lexema;
        consumir(Tipo_de_Token::IDENTIFICADOR);
        consumir(Tipo_de_Token::OP_ASIGNACION);

        auto expr = expresion();

        return std::make_unique<Declaracion>(tipoStr, nombre, std::move(expr));
    }

    // Si viene con ++/--: i++, i--
    if (tokenActual().tipo == Tipo_de_Token::IDENTIFICADOR &&
        (siguienteToken().tipo == Tipo_de_Token::OP_INCREMENTO ||
         siguienteToken().tipo == Tipo_de_Token::OP_DECREMENTO)) {
        std::string nombre = tokenActual().lexema;
        consumir(Tipo_de_Token::IDENTIFICADOR);
        std::string op = (tokenActual().tipo == Tipo_de_Token::OP_INCREMENTO) ? "+" : "-";
        avanzar();
        auto var = std::make_unique<Variable>(nombre);
        auto uno = std::make_unique<Numero>("1");
        auto bin = std::make_unique<Binaria>(op, std::move(var), std::move(uno));
        return std::make_unique<Asignacion>(nombre, std::move(bin));
    }

    // Si viene sin tipo: j = 0
    std::string nombre = tokenActual().lexema;
    consumir(Tipo_de_Token::IDENTIFICADOR);
    consumir(Tipo_de_Token::OP_ASIGNACION);
    auto expr = expresion();

    return std::make_unique<Asignacion>(nombre, std::move(expr));
}

std::unique_ptr<Stmt> Parser::sentencia_if(){

    consumir(Tipo_de_Token::IF);
    consumir(Tipo_de_Token::DEL_PARENTESIS_IZQ);

    auto condicion = expresion();

    consumir(Tipo_de_Token::DEL_PARENTESIS_DER);

    auto entonces = sentencia();  

    std::unique_ptr<Stmt> sino = nullptr;

    if (tokenActual().tipo == Tipo_de_Token::ELSE){
        consumir(Tipo_de_Token::ELSE);
        sino = sentencia();
    }

    return std::make_unique<If>(
        std::move(condicion),
        std::move(entonces),
        std::move(sino)
    );
}

std::unique_ptr<Stmt> Parser::sentencia_while(){

    consumir(Tipo_de_Token::WHILE);
    consumir(Tipo_de_Token::DEL_PARENTESIS_IZQ);

    auto condicion = expresion();

    consumir(Tipo_de_Token::DEL_PARENTESIS_DER);

    auto cuerpo = sentencia();  

    return std::make_unique<While>(
        std::move(condicion),
        std::move(cuerpo)
    );
}

std::unique_ptr<Stmt> Parser::declaracion(){

    
    std::string tipoStr = tokenActual().lexema;
    tipo();  // consume el tipo

    
    std::string nombre = tokenActual().lexema;
    consumir(Tipo_de_Token::IDENTIFICADOR);

    std::unique_ptr<Expr> valor = nullptr;

    
    if (tokenActual().tipo == Tipo_de_Token::OP_ASIGNACION){
        consumir(Tipo_de_Token::OP_ASIGNACION);
        valor = expresion();
    }

    consumir(Tipo_de_Token::DEL_PUNTOCOMA);

    return std::make_unique<Declaracion>(
        tipoStr,
        nombre,
        std::move(valor)
    );
}

void Parser::tipo(){
    avanzar();
}

std::unique_ptr<Stmt> Parser::asignacion(){

    std::string nombre = tokenActual().lexema;
    consumir(Tipo_de_Token::IDENTIFICADOR);

    consumir(Tipo_de_Token::OP_ASIGNACION);

    auto expr = expresion();  

    consumir(Tipo_de_Token::DEL_PUNTOCOMA);

    return std::make_unique<Asignacion>(
        nombre,
        std::move(expr)
    );
}

std::unique_ptr<Stmt> Parser::incremento_decremento() {
    std::string nombre = tokenActual().lexema;
    consumir(Tipo_de_Token::IDENTIFICADOR);

    std::string op = (tokenActual().tipo == Tipo_de_Token::OP_INCREMENTO) ? "+" : "-";
    avanzar();
    consumir(Tipo_de_Token::DEL_PUNTOCOMA);

    auto var = std::make_unique<Variable>(nombre);
    auto uno = std::make_unique<Numero>("1");
    auto bin = std::make_unique<Binaria>(op, std::move(var), std::move(uno));
    return std::make_unique<Asignacion>(nombre, std::move(bin));
}

std::unique_ptr<Stmt> Parser::llamada_procedimientos() {
    std::string nombre = tokenActual().lexema;
    consumir(Tipo_de_Token::IDENTIFICADOR);

    consumir(Tipo_de_Token::DEL_PARENTESIS_IZQ);
    consumir(Tipo_de_Token::DEL_PARENTESIS_DER);
    consumir(Tipo_de_Token::DEL_PUNTOCOMA);

    return std::make_unique<LlamadaProcedimiento>(nombre);
}

std::unique_ptr<Expr> Parser::expresion() {
    return expresion_logica_or();
}

std::unique_ptr<Expr> Parser::expresion_logica_or() {
    auto izquierda = expresion_logica_and();

    while (tokenActual().tipo == Tipo_de_Token::OP_OR) {
        std::string op = tokenActual().lexema;
        avanzar();
        auto derecha = expresion_logica_and();
        izquierda = std::make_unique<Binaria>(op, std::move(izquierda), std::move(derecha));
    }

    return izquierda;
}

std::unique_ptr<Expr> Parser::expresion_logica_and() {
    auto izquierda = expresion_relacional();

    while (tokenActual().tipo == Tipo_de_Token::OP_AND) {
        std::string op = tokenActual().lexema;
        avanzar();
        auto derecha = expresion_relacional();
        izquierda = std::make_unique<Binaria>(op, std::move(izquierda), std::move(derecha));
    }

    return izquierda;
}

    std::unique_ptr<Expr> Parser::expresion_relacional() {

    auto izquierda = expresion_aritmetica();

    while (tokenActual().tipo == Tipo_de_Token::OP_MAYORQUE ||
           tokenActual().tipo == Tipo_de_Token::OP_MENORQUE ||
           tokenActual().tipo == Tipo_de_Token::OP_MAYORIGUAL ||
           tokenActual().tipo == Tipo_de_Token::OP_MENORIGUAL ||
           tokenActual().tipo == Tipo_de_Token::OP_IGUALIGUAL ||
           tokenActual().tipo == Tipo_de_Token::OP_DIFERENTE) {

        std::string op = tokenActual().lexema;
        avanzar();

        auto derecha = expresion_aritmetica();

        izquierda = std::make_unique<Binaria>(
            op,
            std::move(izquierda),
            std::move(derecha)
        );
    }

    return izquierda;
}

std::unique_ptr<Expr> Parser::expresion_aritmetica() {

    auto izquierda = termino();

    while (tokenActual().tipo == Tipo_de_Token::OP_SUMA ||
           tokenActual().tipo == Tipo_de_Token::OP_RESTA) {

        std::string op = tokenActual().lexema;
        avanzar();

        auto derecha = termino();

        izquierda = std::make_unique<Binaria>(
            op,
            std::move(izquierda),
            std::move(derecha)
        );
    }

    return izquierda;
}

std::unique_ptr<Expr> Parser::termino() {

    auto izquierda = factor();

    while (tokenActual().tipo == Tipo_de_Token::OP_MULT ||
           tokenActual().tipo == Tipo_de_Token::OP_DIVISION ||
           tokenActual().tipo == Tipo_de_Token::OP_MODULO) {

        std::string op = tokenActual().lexema;
        avanzar();

        auto derecha = factor();

        izquierda = std::make_unique<Binaria>(
            op,
            std::move(izquierda),
            std::move(derecha)
        );
    }

    return izquierda;
}

std::unique_ptr<Expr> Parser::factor() {

    if (tokenActual().tipo == Tipo_de_Token::OP_NOT) {
        avanzar();
        auto operando = factor();
        return std::make_unique<Unaria>("!", std::move(operando));
    }

    if (tokenActual().tipo == Tipo_de_Token::OP_RESTA) {
        avanzar();
        auto operando = factor();
        return std::make_unique<Unaria>("-", std::move(operando));
    }

    if (tokenActual().tipo == Tipo_de_Token::NUMERO) {

        std::string valor = tokenActual().lexema;
        avanzar();

        return std::make_unique<Numero>(valor);
    }

    if (tokenActual().tipo == Tipo_de_Token::BOOLEAN) {

    bool valor = (tokenActual().lexema == "true");
    avanzar();

    return std::make_unique<Booleano>(valor);
    }   

    if (tokenActual().tipo == Tipo_de_Token::CADENA_LITERAL) {

    std::string valor = tokenActual().lexema;
    avanzar();

    return std::make_unique<Cadena>(valor);
    }

    if (tokenActual().tipo == Tipo_de_Token::IDENTIFICADOR) {

        std::string nombre = tokenActual().lexema;
        avanzar();

        return std::make_unique<Variable>(nombre);
    }

    if (tokenActual().tipo == Tipo_de_Token::DEL_PARENTESIS_IZQ) {

        consumir(Tipo_de_Token::DEL_PARENTESIS_IZQ);

        auto expr = expresion();

        consumir(Tipo_de_Token::DEL_PARENTESIS_DER);

        return expr;
    }

    
    error(tokenActual().tipo);
    return nullptr;
}