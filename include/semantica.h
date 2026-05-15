#ifndef SEMANTICA_H
#define SEMANTICA_H

#include "ast.h"
#include "tabla.h"
#include <vector>
#include <string>

TipoDato promoverTipos(TipoDato a, TipoDato b);
TipoDato analizarExpr(Expr* expr, Entorno& env);
void analizarStmt(Stmt* stmt, Entorno& env);
TipoDato convertirTipo(const std::string& tipoStr);
const std::vector<std::string>& obtenerErroresSem();

#endif