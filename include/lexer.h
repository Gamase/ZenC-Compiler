#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <fstream>
#include "token.h"


//Clase del Lexer
class Lexer {
private:
    std::ifstream archivo;
    char actual;
    int linea;
    int columna;

    void avanzar();
    char revisar();
    bool finEOF();

    //Interfaz
public:
    Lexer(const std::string& nombre_archivo);
    std::vector<Token> tokenize();
};

#endif
