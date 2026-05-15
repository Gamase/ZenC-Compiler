#ifndef INTERPRETE_H
#define INTERPRETE_H

#include "ast.h"
#include "tabla.h"

// evaluar expresiones
Valor evaluarExpr(Expr* expr, Entorno& env);

// ejecutar sentencias
void ejecutarStmt(Stmt* stmt, Entorno& env);

#endif