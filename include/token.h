#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class Tipo_de_Token {

    // Tipos de dato
    INT,
    FLOAT,
    DOUBLE,
    CHAR,
    STRING,
    BOOLEAN,
    VOID,

    // Control
    IF,
    ELSE,
    FOR,
    WHILE,
    RETURN,

    // I/O
    INPUT,
    PRINT,

    // Identificadores y literales
    IDENTIFICADOR,
    NUMERO,
    CADENA_LITERAL,

    // Operadores aritméticos
    OP_SUMA,
    OP_RESTA,
    OP_MULT,
    OP_DIVISION,
    OP_MODULO,

    // Operadores relacionales
    OP_MAYORQUE,
    OP_MENORQUE,
    OP_MAYORIGUAL,
    OP_MENORIGUAL,
    OP_IGUALIGUAL,
    OP_DIFERENTE,

    // Operadores lógicos
    OP_AND,
    OP_OR,
    OP_NOT,

    // Incremento / decremento
    OP_INCREMENTO,
    OP_DECREMENTO,

    // Asignación
    OP_ASIGNACION,

    // Delimitadores
    DEL_PARENTESIS_IZQ,
    DEL_PARENTESIS_DER,
    DEL_LLAVE_IZQ,
    DEL_LLAVE_DER,
    DEL_CORCHETE_IZQ,
    DEL_CORCHETE_DER,
    DEL_COMA,
    DEL_PUNTOCOMA,
    DEL_PUNTOPUNTO,

    END_OF_FILE,
    DESCONOCIDO
};

struct Token {
    Tipo_de_Token tipo;
    std::string lexema;
    int linea;
    int columna;

    Token(Tipo_de_Token t, std::string l, int li, int co)
        : tipo(t), lexema(l), linea(li), columna(co) {}
};

#endif