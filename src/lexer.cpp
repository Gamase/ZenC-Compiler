#include "../include/lexer.h"
#include <cctype>
#include <iostream>

using namespace std;

//Constructor del Lexer
Lexer::Lexer(const string& nombre_archivo) {
    archivo.open(nombre_archivo);

    if(!archivo.is_open()) {
        cout << "No se pudo abrir el archivo\n";
        exit(1); //Se termina el programa si el archivo no se encuentra
    }

    linea = 1;
    columna = 0;
    avanzar(); //Lee el primer caracter del archivo
}

void Lexer::avanzar() {
    int c = archivo.get();

    if (c == EOF) {
        actual = '\0';
        return;
    }

    actual = static_cast<char>(c);

    columna++;

    if (actual == '\n') {
        linea++;
        columna = 0;
    }
}

char Lexer::revisar() {
    int c = archivo.peek();
    if (c == EOF) return '\0';
    return static_cast<char>(c);
}

bool Lexer::finEOF() {
    return actual == '\0';
}

//Palabras Reservadas
Tipo_de_Token obtenerPalabraClave(const string& palabra)
{
   if (palabra == "int") return Tipo_de_Token::INT;
    if (palabra == "float") return Tipo_de_Token::FLOAT;
    if (palabra == "double") return Tipo_de_Token::DOUBLE;
    if (palabra == "char") return Tipo_de_Token::CHAR;
    if (palabra == "string") return Tipo_de_Token::STRING;
    if (palabra == "bool") return Tipo_de_Token::BOOLEAN;
    if (palabra == "true") return Tipo_de_Token::BOOLEAN;
    if (palabra == "false") return Tipo_de_Token::BOOLEAN;
    if (palabra == "void") return Tipo_de_Token::VOID;

    if (palabra == "if") return Tipo_de_Token::IF;
    if (palabra == "else") return Tipo_de_Token::ELSE;
    if (palabra == "for") return Tipo_de_Token::FOR;
    if (palabra == "while") return Tipo_de_Token::WHILE;
    if (palabra == "return") return Tipo_de_Token::RETURN;

    if (palabra == "input") return Tipo_de_Token::INPUT;
    if (palabra == "print") return Tipo_de_Token::PRINT;

    return Tipo_de_Token::IDENTIFICADOR;

}

//Tokens

vector<Token> Lexer::tokenize(){
    vector<Token> tokens;

    while(!finEOF())
    {
        //Espacios
        if(isspace(actual))
        {
            avanzar();
            continue;
        }
        //Identificadores y palabras reservadas
        if(isalpha(actual) || actual == '_')
        {
            string buffer;
            int columna_inicial = columna;
            while(isalnum(actual) || actual == '_')
            {
                buffer += actual;
                avanzar();
            }
            Tipo_de_Token tipo = obtenerPalabraClave(buffer);
            tokens.push_back({tipo, buffer, linea, columna_inicial});
            continue;
        }
        //Numeros
        if(isdigit(actual))
        {
            string buffer;
            int columna_inicial = columna;
            bool esDecimal = false;

            while(isdigit(actual) || (actual == '.' && !esDecimal))
            {
                if (actual == '.')
                        esDecimal =true;
                buffer += actual;
                avanzar();
            }
            tokens.push_back({Tipo_de_Token::NUMERO,buffer,linea,columna_inicial});

            continue;
        }
        // STRINGS
        if(actual == '"')
        {
            string buffer;
            int columna_inicial = columna;
            avanzar();

            while(actual != '"' && !finEOF())
            {
                buffer += actual;
                avanzar();
            }
            if (finEOF())
            {
                cout << "Error: cadena no cerrada... "
                << linea << endl;
                exit(1);
            }
            avanzar(); 

            tokens.push_back({Tipo_de_Token::CADENA_LITERAL, buffer, linea, columna_inicial});
            continue;
        }

        //Operadores
        int columna_inicial = columna;

        if (actual == '+' && revisar() == '+') {
            avanzar();
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_INCREMENTO, "++", linea, columna_inicial});
            continue;
        }

        if (actual == '-' && revisar() == '-') {
            avanzar();
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_DECREMENTO, "--", linea, columna_inicial});
            continue;
        }

        if (actual == '+') {
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_SUMA, "+", linea, columna_inicial});
            continue;
        }

        if (actual == '-') {
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_RESTA, "-", linea, columna_inicial});
            continue;
        }

        if (actual == '*') {
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_MULT, "*", linea, columna_inicial});
            continue;
        }

        if (actual == '%') {
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_MODULO, "%", linea, columna_inicial});
            continue;
        }

        if (actual == '/') {
            if (revisar() == '/') {
                while (!finEOF() && actual != '\n') avanzar();
                continue;
            }
            if (revisar() == '*') {
                avanzar(); avanzar();
                while (!finEOF()) {
                    if (actual == '*' && revisar() == '/') {
                        avanzar(); avanzar();
                        break;
                    }
                    avanzar();
                }
                continue;
            }
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_DIVISION, "/", linea, columna_inicial});
            continue;
        }

        if (actual == '=' && revisar() == '=') {
            avanzar();
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_IGUALIGUAL, "==", linea, columna_inicial});
            continue;
        }

        if (actual == '=') {
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_ASIGNACION, "=", linea, columna_inicial});
            continue;
        }

        if (actual == '!' && revisar() == '=') {
            avanzar();
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_DIFERENTE, "!=", linea, columna_inicial});
            continue;
        }

        if (actual == '!') {
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_NOT, "!", linea, columna_inicial});
            continue;
        }

        if (actual == '>' && revisar() == '=') {
            avanzar();
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_MAYORIGUAL, ">=", linea, columna_inicial});
            continue;
        }

        if (actual == '>') {
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_MAYORQUE, ">", linea, columna_inicial});
            continue;
        }

        if (actual == '<' && revisar() == '=') {
            avanzar();
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_MENORIGUAL, "<=", linea, columna_inicial});
            continue;
        }

        if (actual == '<') {
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_MENORQUE, "<", linea, columna_inicial});
            continue;
        }

        if (actual == '&' && revisar() == '&') {
            avanzar();
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_AND, "&&", linea, columna_inicial});
            continue;
        }

        if (actual == '|' && revisar() == '|') {
            avanzar();
            avanzar();
            tokens.push_back({Tipo_de_Token::OP_OR, "||", linea, columna_inicial});
            continue;
        }

        if (actual == '(') {
            avanzar();
            tokens.push_back({Tipo_de_Token::DEL_PARENTESIS_IZQ, "(", linea, columna_inicial});
            continue;
        }

        if (actual == ')') {
            avanzar();
            tokens.push_back({Tipo_de_Token::DEL_PARENTESIS_DER, ")", linea, columna_inicial});
            continue;
        }

        if (actual == '{') {
            avanzar();
            tokens.push_back({Tipo_de_Token::DEL_LLAVE_IZQ, "{", linea, columna_inicial});
            continue;
        }

        if (actual == '}') {
            avanzar();
            tokens.push_back({Tipo_de_Token::DEL_LLAVE_DER, "}", linea, columna_inicial});
            continue;
        }

        if (actual == '[') {
            avanzar();
            tokens.push_back({Tipo_de_Token::DEL_CORCHETE_IZQ, "[", linea, columna_inicial});
            continue;
        }

        if (actual == ']') {
            avanzar();
            tokens.push_back({Tipo_de_Token::DEL_CORCHETE_DER, "]", linea, columna_inicial});
            continue;
        }

        if (actual == ',') {
            avanzar();
            tokens.push_back({Tipo_de_Token::DEL_COMA, ",", linea, columna_inicial});
            continue;
        }

        if (actual == ';') {
            avanzar();
            tokens.push_back({Tipo_de_Token::DEL_PUNTOCOMA, ";", linea, columna_inicial});
            continue;
        }
        
        if (actual == ':') {
            avanzar();
            tokens.push_back({Tipo_de_Token::DEL_PUNTOPUNTO, ":", linea, columna_inicial});
            continue;
        }

        // DESCONOCIDO
        string desc(1, actual);
        tokens.push_back({Tipo_de_Token::DESCONOCIDO, desc, linea, columna});
        avanzar();
    }
    tokens.push_back({Tipo_de_Token::END_OF_FILE, "EOF", linea, columna});
    return tokens;
}