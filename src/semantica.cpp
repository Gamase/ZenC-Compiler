#include "semantica.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>

static std::vector<std::string> erroresSem;

static void agregarError(const std::string& msg) {
    erroresSem.push_back(msg);
}

const std::vector<std::string>& obtenerErroresSem() {
    return erroresSem;
}

TipoDato promoverTipos(TipoDato a, TipoDato b) {
    if (a == b) return a;

    if ((a == T_INT && b == T_FLOAT) || (a == T_FLOAT && b == T_INT))
        return T_FLOAT;

    if (a == T_STRING && b == T_STRING)
        return T_STRING;

    agregarError("tipos incompatibles en operacion");
    return T_INT;
}

TipoDato convertirTipo(const std::string& tipoStr) {
    if (tipoStr == "int")    return T_INT;
    if (tipoStr == "float")  return T_FLOAT;
    if (tipoStr == "double") return T_FLOAT;
    if (tipoStr == "bool")   return T_BOOL;
    if (tipoStr == "string") return T_STRING;

    throw std::runtime_error("Error: tipo desconocido: '" + tipoStr + "'");
}

TipoDato analizarExpr(Expr* expr, Entorno& env) {

    if (auto num = dynamic_cast<Numero*>(expr)) {
        return (num->valor.find('.') != std::string::npos) ? T_FLOAT : T_INT;
    }

    if (auto b = dynamic_cast<Booleano*>(expr)) {
        return T_BOOL;
    }

    if (auto str = dynamic_cast<Cadena*>(expr)) {
        return T_STRING;
    }

    if (auto var = dynamic_cast<Variable*>(expr)) {
        return env.obtener(var->nombre).tipo;
    }

    if (auto uni = dynamic_cast<Unaria*>(expr)) {
        TipoDato tipo = analizarExpr(uni->operando.get(), env);

        if (uni->operador == "!") {
            if (tipo != T_BOOL) {
                agregarError("! requiere operando booleano");
                return T_BOOL;
            }
            return T_BOOL;
        }
        if (uni->operador == "-") {
            if (tipo == T_BOOL || tipo == T_STRING) {
                agregarError("negacion unaria no aplica a bool o string");
                return T_INT;
            }
            return tipo;
        }
        agregarError("operador unario no reconocido");
        return T_INT;
    }

    if (auto bin = dynamic_cast<Binaria*>(expr)) {
        TipoDato izq = analizarExpr(bin->izquierda.get(), env);
        TipoDato der = analizarExpr(bin->derecha.get(), env);

        if (bin->operador == "/") {
            if (izq == T_BOOL || der == T_BOOL) {
                agregarError("no se permiten booleanos en division");
                return T_FLOAT;
            }
            return T_FLOAT;
        }

        if (bin->operador == "+" ||
            bin->operador == "-" ||
            bin->operador == "*" ||
            bin->operador == "%") {

            if (izq == T_BOOL || der == T_BOOL) {
                agregarError("no se permiten booleanos en operaciones aritmeticas");
                return T_INT;
            }
            TipoDato resultado = promoverTipos(izq, der);
            if (resultado == T_STRING && bin->operador != "+") {
                agregarError("solo se permite + con strings, no " + bin->operador);
                return T_STRING;
            }
            return resultado;
        }

        if (bin->operador == "==" || bin->operador == "!=") {
            if (izq == T_STRING || der == T_STRING) {
                if (izq != der) {
                    agregarError("comparacion de string con tipo diferente");
                }
                return T_BOOL;
            }
            promoverTipos(izq, der);
            return T_BOOL;
        }

        if (bin->operador == ">" ||
            bin->operador == "<" ||
            bin->operador == ">=" ||
            bin->operador == "<=") {

            if (izq == T_BOOL || der == T_BOOL) {
                agregarError("no se pueden comparar booleanos");
                return T_BOOL;
            }
            promoverTipos(izq, der);
            return T_BOOL;
        }

        if (bin->operador == "&&" || bin->operador == "||") {
            if (izq != T_BOOL || der != T_BOOL) {
                agregarError("los operandos de " + bin->operador + " deben ser booleanos");
                return T_BOOL;
            }
            return T_BOOL;
        }

        agregarError("operador no soportado: " + bin->operador);
        return T_INT;
    }

    agregarError("expresion no soportada en analisis semantico");
    return T_INT;
}

void analizarStmt(Stmt* stmt, Entorno& env) {

    if (auto decl = dynamic_cast<Declaracion*>(stmt)) {
        TipoDato tipo = convertirTipo(decl->tipo);

        if (decl->valor) {
            TipoDato tipoExpr = analizarExpr(decl->valor.get(), env);

            if (tipo == T_INT && tipoExpr == T_FLOAT) {
                agregarError("no se puede asignar float a int en variable '" + decl->nombre + "'");
            } else if (tipo == T_FLOAT && tipoExpr == T_INT) {
                // OK: promocion implicita
            } else if (tipo != tipoExpr) {
                agregarError("tipos incompatibles en declaracion de variable '" + decl->nombre + "'");
            }
        }
        env.definir(decl->nombre, {tipo, 0});
    }

    else if (auto asig = dynamic_cast<Asignacion*>(stmt)) {
        auto simbolo = env.obtener(asig->nombre);
        TipoDato tipoExpr = analizarExpr(asig->valor.get(), env);

        if (simbolo.tipo == T_INT && tipoExpr == T_FLOAT) {
            agregarError("no se puede asignar float a int en variable '" + asig->nombre + "'");
        } else if (simbolo.tipo == T_FLOAT && tipoExpr == T_INT) {
            // OK
        } else if (simbolo.tipo != tipoExpr) {
            agregarError("tipos incompatibles en asignacion de variable '" + asig->nombre + "'");
        }
    }

    else if (auto print = dynamic_cast<Print*>(stmt)) {
        analizarExpr(print->valor.get(), env);
    }

    else if (auto ifstmt = dynamic_cast<If*>(stmt)) {
        TipoDato cond = analizarExpr(ifstmt->condicion.get(), env);
        if (cond != T_BOOL)
            agregarError("condicion de if debe ser booleana");

        Entorno thenEnv(&env);
        analizarStmt(ifstmt->entonces.get(), thenEnv);

        if (ifstmt->sino) {
            Entorno elseEnv(&env);
            analizarStmt(ifstmt->sino.get(), elseEnv);
        }
    }

    else if (auto wh = dynamic_cast<While*>(stmt)) {
        TipoDato cond = analizarExpr(wh->condicion.get(), env);
        if (cond != T_BOOL)
            agregarError("condicion de while debe ser booleana");

        Entorno whileEnv(&env);
        analizarStmt(wh->cuerpo.get(), whileEnv);
    }

    else if (auto f = dynamic_cast<For*>(stmt)) {
        Entorno forEnv(&env);
        analizarStmt(f->inicializacion.get(), forEnv);

        TipoDato cond = analizarExpr(f->condicion.get(), forEnv);
        if (cond != T_BOOL)
            agregarError("condicion de for debe ser booleana");

        analizarStmt(f->cuerpo.get(), forEnv);
        analizarStmt(f->actualizacion.get(), forEnv);
    }

    else if (auto bloque = dynamic_cast<Bloque*>(stmt)) {
        Entorno nuevoEnv(&env);
        for (auto& s : bloque->sentencias)
            analizarStmt(s.get(), nuevoEnv);
    }

    else if (auto proc = dynamic_cast<Procedimiento*>(stmt)) {
        Entorno procEnv(&env);
        analizarStmt(proc->cuerpo.get(), procEnv);
    }

    else if (auto inp = dynamic_cast<Input*>(stmt)) {
        env.obtener(inp->nombre);
    }

    else if (auto llamada = dynamic_cast<LlamadaProcedimiento*>(stmt)) {
        // no requiere validacion de tipo
    }
}
