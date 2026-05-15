#include "../include/codegen.h"
#include <iostream>

GeneradorCodigo::GeneradorCodigo()
    : tempContador(0), etiquetaContador(0) {}

std::string GeneradorCodigo::nuevoTemp() {
    return "t" + std::to_string(++tempContador);
}

std::string GeneradorCodigo::nuevaEtiqueta() {
    return "L" + std::to_string(++etiquetaContador);
}

void GeneradorCodigo::emitir(Instruccion instr) {
    instrucciones.push_back(instr);
}

std::string GeneradorCodigo::generarExpr(Expr* expr, std::string& tipoResultado) {

    if (auto num = dynamic_cast<Numero*>(expr)) {
        if (num->valor.find('.') != std::string::npos)
            tipoResultado = "float";
        else
            tipoResultado = "int";
        return num->valor;
    }

    if (auto var = dynamic_cast<Variable*>(expr)) {
        if (tipoVariables.count(var->nombre))
            tipoResultado = tipoVariables[var->nombre];
        else
            tipoResultado = "int";
        return var->nombre;
    }

    if (auto b = dynamic_cast<Booleano*>(expr)) {
        tipoResultado = "int";
        return b->valor ? "1" : "0";
    }

    if (auto str = dynamic_cast<Cadena*>(expr)) {
        tipoResultado = "string";
        return "\"" + str->valor + "\"";
    }

    if (auto uni = dynamic_cast<Unaria*>(expr)) {
        std::string tipoOp;
        std::string operando = generarExpr(uni->operando.get(), tipoOp);
        std::string temp = nuevoTemp();
        if (uni->operador == "!") {
            tipoResultado = "int";
            emitir({temp, operando, "not", "", "int"});
            tipoVariables[temp] = "int";
        } else {
            tipoResultado = tipoOp;
            emitir({temp, operando, "neg", "", tipoOp});
            tipoVariables[temp] = tipoOp;
        }
        return temp;
    }

    if (auto bin = dynamic_cast<Binaria*>(expr)) {
        std::string tipoIzq, tipoDer;
        std::string izq = generarExpr(bin->izquierda.get(), tipoIzq);
        std::string der = generarExpr(bin->derecha.get(), tipoDer);

       if (bin->operador == ">" || bin->operador == "<" ||
        bin->operador == ">=" || bin->operador == "<=" ||
        bin->operador == "==" || bin->operador == "!=") {
        std::string tipoOp;
        if (tipoIzq == "string" || tipoDer == "string") tipoOp = "string";
        else if (tipoIzq == "float" || tipoDer == "float") tipoOp = "float";
        else tipoOp = "int";
        tipoResultado = "int";
        std::string temp = nuevoTemp();
        emitir({temp, izq, bin->operador, der, tipoOp});
        tipoVariables[temp] = "int";
        return temp;
}

        if (bin->operador == "&&" || bin->operador == "||") {
            tipoResultado = "int";
            std::string temp = nuevoTemp();
            emitir({temp, izq, bin->operador, der, "int"});
            tipoVariables[temp] = "int";
            return temp;
}

        std::string tipoOp = "int";
        if (tipoIzq == "float" || tipoDer == "float" || bin->operador == "/")
            tipoOp = "float";

        tipoResultado = tipoOp;
        std::string temp = nuevoTemp();
        emitir({temp, izq, bin->operador, der, tipoOp});
        return temp;
    }

    tipoResultado = "int";
    return "?";
}

void GeneradorCodigo::generarStmt(Stmt* stmt) {

    if (auto decl = dynamic_cast<Declaracion*>(stmt)) {
        tipoVariables[decl->nombre] = decl->tipo;
        if (decl->valor) {
            std::string tipoRes;
            std::string val = generarExpr(decl->valor.get(), tipoRes);
            emitir({decl->nombre, val, "=", "", decl->tipo});
        } else {
            emitir({decl->nombre, "0", "=", "", decl->tipo});
        }
        return;
    }

    if (auto asig = dynamic_cast<Asignacion*>(stmt)) {
        std::string tipoRes;
        std::string val = generarExpr(asig->valor.get(), tipoRes);
        std::string tipo = tipoVariables.count(asig->nombre) ?
                           tipoVariables[asig->nombre] : tipoRes;
        emitir({asig->nombre, val, "=", "", tipo});
        return;
    }

    if (auto print = dynamic_cast<Print*>(stmt)) {
        std::string tipoRes;
        std::string val = generarExpr(print->valor.get(), tipoRes);
        emitir({"", val, "print", "", tipoRes});
        return;
    }

    if (auto inp = dynamic_cast<Input*>(stmt)) {
        std::string tipo = tipoVariables.count(inp->nombre) ?
                           tipoVariables[inp->nombre] : "int";
        emitir({"", inp->nombre, "input", "", tipo});
        return;
    }

    if (auto ifstmt = dynamic_cast<If*>(stmt)) {
        std::string tipoRes;
        std::string cond = generarExpr(ifstmt->condicion.get(), tipoRes);
        std::string lElse = nuevaEtiqueta();
        std::string lFin  = nuevaEtiqueta();

        emitir({"", cond, "ifFalse", lElse, "int"});
        generarStmt(ifstmt->entonces.get());
        emitir({"", "", "goto", lFin, ""});
        emitir({lElse, "", "label", "", ""});
        if (ifstmt->sino)
            generarStmt(ifstmt->sino.get());
        emitir({lFin, "", "label", "", ""});
        return;
    }

    if (auto wh = dynamic_cast<While*>(stmt)) {
        std::string lInicio = nuevaEtiqueta();
        std::string lFin    = nuevaEtiqueta();

        emitir({lInicio, "", "label", "", ""});
        std::string tipoRes;
        std::string cond = generarExpr(wh->condicion.get(), tipoRes);
        emitir({"", cond, "ifFalse", lFin, "int"});
        generarStmt(wh->cuerpo.get());
        emitir({"", "", "goto", lInicio, ""});
        emitir({lFin, "", "label", "", ""});
        return;
    }

    if (auto f = dynamic_cast<For*>(stmt)) {
        std::string lInicio = nuevaEtiqueta();
        std::string lFin    = nuevaEtiqueta();

        generarStmt(f->inicializacion.get());
        emitir({lInicio, "", "label", "", ""});
        std::string tipoRes;
        std::string cond = generarExpr(f->condicion.get(), tipoRes);
        emitir({"", cond, "ifFalse", lFin, "int"});
        generarStmt(f->cuerpo.get());
        generarStmt(f->actualizacion.get());
        emitir({"", "", "goto", lInicio, ""});
        emitir({lFin, "", "label", "", ""});
        return;
    }

    if (auto bloque = dynamic_cast<Bloque*>(stmt)) {
        for (auto& s : bloque->sentencias)
            generarStmt(s.get());
        return;
    }

    if (auto proc = dynamic_cast<Procedimiento*>(stmt)) {
        emitir({"proc_" + proc->nombre, "", "proc_inicio", "", ""});
        generarStmt(proc->cuerpo.get());
        emitir({"", "", "proc_fin", proc->nombre, ""});
        return;
    }

    if (auto llamada = dynamic_cast<LlamadaProcedimiento*>(stmt)) {
        emitir({"", llamada->nombre, "call", "", ""});
        return;
    }
}

void GeneradorCodigo::generar(const std::vector<Stmt*>& programa) {
    for (auto stmt : programa)
        generarStmt(stmt);
}

void GeneradorCodigo::imprimir() {
    std::cout << "\n====== CODIGO INTERMEDIO (TAC) ======\n\n";

    for (auto& ins : instrucciones) {
        if (ins.operador == "label") {
            std::cout << ins.resultado << ":\n";
        }
        else if (ins.operador == "goto") {
            std::cout << "    goto " << ins.arg2 << "\n";
        }
        else if (ins.operador == "ifFalse") {
            std::cout << "    ifFalse " << ins.arg1
                      << " goto " << ins.arg2 << "\n";
        }
        else if (ins.operador == "print") {
            std::cout << "    print " << ins.arg1
                      << " [" << ins.tipo << "]\n";
        }
        else if (ins.operador == "input") {
            std::cout << "    input " << ins.arg1
                      << " [" << ins.tipo << "]\n";
        }
        else if (ins.operador == "proc_inicio") {
            std::cout << "\n" << ins.resultado << ":\n";
        }
        else if (ins.operador == "proc_fin") {
            std::cout << "    ret\n";
        }
        else if (ins.operador == "call") {
            std::cout << "    call " << ins.arg1 << "\n";
        }
        else if (ins.operador == "=" && ins.arg2.empty()) {
            std::cout << "    " << ins.resultado
                      << " = " << ins.arg1
                      << " [" << ins.tipo << "]\n";
        }
        else {
            std::cout << "    " << ins.resultado
                      << " = " << ins.arg1
                      << " " << ins.operador
                      << " " << ins.arg2
                      << " [" << ins.tipo << "]\n";
        }
    }
    std::cout << "\n";
}