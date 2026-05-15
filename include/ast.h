#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

class ASTNode {
public:
    virtual ~ASTNode() = default;
};


class Expr : public ASTNode {};

class Numero : public Expr {
public:
    std::string valor;

    Numero(const std::string& v) : valor(v) {}
};

class Variable : public Expr {
public:
    std::string nombre;

    Variable(const std::string& n) : nombre(n) {}
};

class Binaria : public Expr {
public:
    std::string operador;
    std::unique_ptr<Expr> izquierda;
    std::unique_ptr<Expr> derecha;

    Binaria(std::string op,
            std::unique_ptr<Expr> izq,
            std::unique_ptr<Expr> der)
        : operador(op),
          izquierda(std::move(izq)),
          derecha(std::move(der)) {}
};

class Stmt : public ASTNode {};

class If : public Stmt {
public:
    std::unique_ptr<Expr> condicion;
    std::unique_ptr<Stmt> entonces;
    std::unique_ptr<Stmt> sino; 

    If(std::unique_ptr<Expr> cond,
       std::unique_ptr<Stmt> thenStmt,
       std::unique_ptr<Stmt> elseStmt)
        : condicion(std::move(cond)),
          entonces(std::move(thenStmt)),
          sino(std::move(elseStmt)) {}
};


class While : public Stmt {
public:
    std::unique_ptr<Expr> condicion;
    std::unique_ptr<Stmt> cuerpo;

    While(std::unique_ptr<Expr> cond,
          std::unique_ptr<Stmt> body)
        : condicion(std::move(cond)),
          cuerpo(std::move(body)) {}
};



class Declaracion : public Stmt {
public:
    std::string tipo;
    std::string nombre;
    std::unique_ptr<Expr> valor;

    Declaracion(std::string t,
                std::string n,
                std::unique_ptr<Expr> v)
        : tipo(t), nombre(n), valor(std::move(v)) {}
};

class Asignacion : public Stmt {
public:
    std::string nombre;
    std::unique_ptr<Expr> valor;

    Asignacion(std::string n,
               std::unique_ptr<Expr> v)
        : nombre(n), valor(std::move(v)) {}
};

class Print : public Stmt {
public:
    std::unique_ptr<Expr> valor;

    Print(std::unique_ptr<Expr> v)
        : valor(std::move(v)) {}
};

class Bloque : public Stmt {
public:
    std::vector<std::unique_ptr<Stmt>> sentencias;
};

class For : public Stmt {
public:
    std::unique_ptr<Stmt> inicializacion;
    std::unique_ptr<Expr> condicion;
    std::unique_ptr<Stmt> actualizacion;
    std::unique_ptr<Stmt> cuerpo;

    For(std::unique_ptr<Stmt> init,
        std::unique_ptr<Expr> cond,
        std::unique_ptr<Stmt> update,
        std::unique_ptr<Stmt> body)
        : inicializacion(std::move(init)),
          condicion(std::move(cond)),
          actualizacion(std::move(update)),
          cuerpo(std::move(body)) {}
};

class Booleano : public Expr {
public:
    bool valor;

    Booleano(bool v) : valor(v) {}
};

class Cadena : public Expr {
public:
    std::string valor;

    Cadena(const std::string& v) : valor(v) {}
};

class Unaria : public Expr {
public:
    std::string operador;
    std::unique_ptr<Expr> operando;

    Unaria(std::string op, std::unique_ptr<Expr> oper)
        : operador(op), operando(std::move(oper)) {}
};


inline void imprimirExpr(const Expr* expr, int nivel = 0) {
    if (!expr) return;

    std::string indent(nivel * 2, ' ');

    // Numero
    if (auto num = dynamic_cast<const Numero*>(expr)) {
        std::cout << indent << "Numero: " << num->valor << "\n";
        return;
    }

    // Variable
    if (auto var = dynamic_cast<const Variable*>(expr)) {
        std::cout << indent << "Variable: " << var->nombre << "\n";
        return;
    }

    // Binaria
    if (auto bin = dynamic_cast<const Binaria*>(expr)) {
        std::cout << indent << "Operador: " << bin->operador << "\n";
        imprimirExpr(bin->izquierda.get(), nivel + 1);
        imprimirExpr(bin->derecha.get(), nivel + 1);
        return;
    }
}


class Procedimiento : public Stmt {
public:
    std::string nombre;
    std::unique_ptr<Stmt> cuerpo;

    Procedimiento(std::string n, std::unique_ptr<Stmt> c)
        : nombre(n), cuerpo(std::move(c)) {}
};


class LlamadaProcedimiento : public Stmt {
public:
    std::string nombre;

    LlamadaProcedimiento(std::string n) : nombre(n) {}
};

class Input : public Stmt {
public:
    std::string nombre;
    Input(std::string n) : nombre(n) {}
};

#endif