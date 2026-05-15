#ifndef TABLA_H
#define TABLA_H

#include <unordered_map>
#include <string>
#include <stdexcept>

struct Valor {
    double numero;
    std::string texto = "";
};

enum TipoDato {
    T_INT,
    T_FLOAT,
    T_BOOL,
    T_STRING
};

struct Simbolo {
    TipoDato tipo;
    double valor;
    std::string texto = "";
};

class Entorno {
public:
    Entorno* padre;
    std::unordered_map<std::string, Simbolo> tabla;

    Entorno(Entorno* padre = nullptr) : padre(padre) {}

    void definir(const std::string& nombre, Simbolo simbolo) {
        if (tabla.count(nombre))
            throw std::runtime_error("Error: variable '" + nombre + "' ya fue declarada");
        tabla[nombre] = simbolo;
    }

    Simbolo obtener(const std::string& nombre) {
        if (tabla.count(nombre))
            return tabla[nombre];
        if (padre)
            return padre->obtener(nombre);
        throw std::runtime_error("Error: variable '" + nombre + "' no declarada");
    }

    void asignar(const std::string& nombre, Simbolo simbolo) {
        if (tabla.count(nombre)) {
            tabla[nombre] = simbolo;
            return;
        }
        if (padre) {
            padre->asignar(nombre, simbolo);
            return;
        }
        throw std::runtime_error("Error: variable '" + nombre + "' no declarada");
    }
};

#endif