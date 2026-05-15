#include "../include/interprete.h"
#include <iostream>
#include <map>

std::map<std::string, Procedimiento*> tablaProcedimientos;

void ejecutarStmt(Stmt* stmt, Entorno& env) {

    if (auto decl = dynamic_cast<Declaracion*>(stmt)) {
        Valor val;
        if (decl->valor) {
            val = evaluarExpr(decl->valor.get(), env);
        }

        TipoDato tipo = T_INT;
        if (decl->tipo == "float" || decl->tipo == "double") tipo = T_FLOAT;
        else if (decl->tipo == "bool")   tipo = T_BOOL;
        else if (decl->tipo == "string") tipo = T_STRING;

        env.definir(decl->nombre, {tipo, val.numero, val.texto});
    }

    else if (auto asig = dynamic_cast<Asignacion*>(stmt)) {
        auto val = evaluarExpr(asig->valor.get(), env);
        auto sim = env.obtener(asig->nombre);
        env.asignar(asig->nombre, {sim.tipo, val.numero, val.texto});
    }

    else if (auto print = dynamic_cast<Print*>(stmt)) {
        if (auto str = dynamic_cast<Cadena*>(print->valor.get())) {
            std::cout << str->valor << std::endl;
        } else {
            auto val = evaluarExpr(print->valor.get(), env);
            if (!val.texto.empty())
                std::cout << val.texto << std::endl;
            else
                std::cout << val.numero << std::endl;
        }
    }

    else if (auto bloque = dynamic_cast<Bloque*>(stmt)) {
        Entorno bloqueEnv(&env);
        for (auto& s : bloque->sentencias)
            ejecutarStmt(s.get(), bloqueEnv);
    }

    else if (auto ifstmt = dynamic_cast<If*>(stmt)) {
        auto cond = evaluarExpr(ifstmt->condicion.get(), env);
        if (cond.numero)
            ejecutarStmt(ifstmt->entonces.get(), env);
        else if (ifstmt->sino)
            ejecutarStmt(ifstmt->sino.get(), env);
    }

    else if (auto wh = dynamic_cast<While*>(stmt)) {
        while (evaluarExpr(wh->condicion.get(), env).numero)
            ejecutarStmt(wh->cuerpo.get(), env);
    }

    else if (auto f = dynamic_cast<For*>(stmt)) {
        Entorno forEnv(&env);
        ejecutarStmt(f->inicializacion.get(), forEnv);
        while (evaluarExpr(f->condicion.get(), forEnv).numero) {
            ejecutarStmt(f->cuerpo.get(), forEnv);
            ejecutarStmt(f->actualizacion.get(), forEnv);
        }
    }

    else if (auto proc = dynamic_cast<Procedimiento*>(stmt)) {
        tablaProcedimientos[proc->nombre] = proc;
    }

    else if (auto llamada = dynamic_cast<LlamadaProcedimiento*>(stmt)) {
        if (tablaProcedimientos.count(llamada->nombre)) {
            Entorno procEnv(&env);
            ejecutarStmt(tablaProcedimientos[llamada->nombre]->cuerpo.get(), procEnv);
        } else {
            std::cout << "Error: procedimiento no definido: " << llamada->nombre << "\n";
            exit(1);
        }
    }

    else if (auto inp = dynamic_cast<Input*>(stmt)) {
        auto sim = env.obtener(inp->nombre);
        if (sim.tipo == T_STRING) {
            std::string txt;
            std::cout << "Ingrese valor para " << inp->nombre << ": ";
            std::cin >> txt;
            env.asignar(inp->nombre, {T_STRING, 0, txt});
        } else {
            double valor;
            std::cout << "Ingrese valor para " << inp->nombre << ": ";
            std::cin >> valor;
            env.asignar(inp->nombre, {sim.tipo, valor});
        }
    }
}

Valor evaluarExpr(Expr* expr, Entorno& env) {

    if (auto num = dynamic_cast<Numero*>(expr)) {
        return { std::stod(num->valor) };
    }

    if (auto var = dynamic_cast<Variable*>(expr)) {
        auto sim = env.obtener(var->nombre);
        return { sim.valor, sim.texto };
    }

    if (auto b = dynamic_cast<Booleano*>(expr)) {
        return { b->valor ? 1.0 : 0.0 };
    }

    if (auto str = dynamic_cast<Cadena*>(expr)) {
        return { 0, str->valor };
    }

    if (auto uni = dynamic_cast<Unaria*>(expr)) {
        auto val = evaluarExpr(uni->operando.get(), env);
        if (uni->operador == "!") return { val.numero == 0 ? 1.0 : 0.0 };
        if (uni->operador == "-") return { -val.numero };
    }

    if (auto bin = dynamic_cast<Binaria*>(expr)) {
        auto izq = evaluarExpr(bin->izquierda.get(), env);
        auto der = evaluarExpr(bin->derecha.get(), env);

        if (bin->operador == "+") {
            if (!izq.texto.empty() || !der.texto.empty())
                return { 0, izq.texto + der.texto };
            return { izq.numero + der.numero };
        }
        if (bin->operador == "-") return { izq.numero - der.numero };
        if (bin->operador == "*") return { izq.numero * der.numero };
        if (bin->operador == "/") {
            if (der.numero == 0) { std::cout << "Error: division entre cero\n"; exit(1); }
            return { izq.numero / der.numero };
        }
        if (bin->operador == "%") {
            if (der.numero == 0) { std::cout << "Error: modulo entre cero\n"; exit(1); }
            return { (double)((long long)izq.numero % (long long)der.numero) };
        }
        if (bin->operador == ">")  return { (double)(izq.numero > der.numero) };
        if (bin->operador == "<")  return { (double)(izq.numero < der.numero) };
        if (bin->operador == ">=") return { (double)(izq.numero >= der.numero) };
        if (bin->operador == "<=") return { (double)(izq.numero <= der.numero) };
        if (bin->operador == "==") {
            if (!izq.texto.empty() || !der.texto.empty())
                return { (double)(izq.texto == der.texto) };
            return { (double)(izq.numero == der.numero) };
        }
        if (bin->operador == "!=") {
            if (!izq.texto.empty() || !der.texto.empty())
                return { (double)(izq.texto != der.texto) };
            return { (double)(izq.numero != der.numero) };
        }
        if (bin->operador == "&&") return { (double)((izq.numero != 0) && (der.numero != 0)) };
        if (bin->operador == "||") return { (double)((izq.numero != 0) || (der.numero != 0)) };
    }

    std::cout << "Error: expresion no soportada\n";
    exit(1);
}
