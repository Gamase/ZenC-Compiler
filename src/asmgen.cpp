#include "../include/asmgen.h"
#include <iostream>
#include <fstream>
#include <sstream>

GeneradorASM::GeneradorASM(const std::vector<Instruccion>& instr)
    : instrucciones(instr), stackOffset(0) {

    // Pre-paso: identificar variables declaradas en scope global (fuera de procs).
    // Solo nombres de usuario; los temporales _tN se excluyen.
    {
        bool enP = false;
        for (auto& ins : instrucciones) {
            if (ins.operador == "proc_inicio") { enP = true;  continue; }
            if (ins.operador == "proc_fin")    { enP = false; continue; }
            if (!enP && ins.operador == "=" && !ins.resultado.empty()) {
                const std::string& r = ins.resultado;
                if (!(r.size() >= 2 && r[0] == '_' && r[1] == 't'))
                    variablesGlobales.insert(r);
            }
        }
    }

    // Paso 1: asignar offsets de stack a variables NO globales.
    // Las globales van a .bss y se acceden via [rel nombre].
    int offset = 8;
    for (auto& ins : instrucciones) {
        if (ins.resultado.empty() || ins.operador == "label" ||
            ins.operador == "proc_inicio" || ins.operador == "proc_fin")
            continue;
        // Globales: solo registrar tipo, sin offset de stack
        if (variablesGlobales.count(ins.resultado)) {
            if (!tipoVariables.count(ins.resultado))
                tipoVariables[ins.resultado] = ins.tipo;
            continue;
        }
        if (!offsetVariables.count(ins.resultado)) {
            offsetVariables[ins.resultado] = offset;
            if (ins.operador == ">" || ins.operador == "<" ||
                ins.operador == ">=" || ins.operador == "<=" ||
                ins.operador == "==" || ins.operador == "!=" ||
                ins.operador == "&&" || ins.operador == "||" ||
                ins.operador == "not") {
                tipoVariables[ins.resultado] = "int";
            } else {
                tipoVariables[ins.resultado] = ins.tipo;
            }
            offset += 8;
        }
    }
    stackOffset = offset;

    // Paso 2: calcular el offset maximo referenciado dentro de cada procedimiento.
    // Esto determina cuanto stack necesita realmente cada proc, incluyendo variables
    // que comparten nombre con otros scopes y quedaron con offsets altos.
    bool enProc = false;
    std::string procActual = "";
    int maxOff = 0;

    auto actualizarMax = [&](const std::string& nombre) {
        if (offsetVariables.count(nombre))
            maxOff = std::max(maxOff, offsetVariables.at(nombre));
    };

    for (auto& ins : instrucciones) {
        if (ins.operador == "proc_inicio") {
            enProc = true;
            procActual = ins.resultado;
            maxOff = 0;
        } else if (ins.operador == "proc_fin") {
            procStackSizes[procActual] = maxOff + 8;
            enProc = false;
        } else if (enProc) {
            actualizarMax(ins.resultado);
            actualizarMax(ins.arg1);
            actualizarMax(ins.arg2);
        }
    }
}

std::string GeneradorASM::operandoAASM(const std::string& op) {
    if (op.empty()) return "";
    if (isdigit(op[0]) || (op[0] == '-' && op.size() > 1)) return op;
    if (op[0] == '"') return op;
    if (variablesGlobales.count(op))
        return "[rel " + op + "]";
    if (offsetVariables.count(op))
        return "[rbp - " + std::to_string(offsetVariables[op]) + "]";
    return op;
}

bool GeneradorASM::esFloat(const std::string& nombre) {
    if (tipoVariables.count(nombre))
        return tipoVariables[nombre] == "float";
    return false;
}

void GeneradorASM::emitirInstruccion(std::ostringstream& out,
                                      const Instruccion& ins,
                                      std::map<std::string, std::string>& cadenaLabels,
                                      std::map<std::string, std::string>& floatLabels) {

    std::string dest = operandoAASM(ins.resultado);
    std::string izq  = operandoAASM(ins.arg1);
    std::string der  = operandoAASM(ins.arg2);
    std::string op   = ins.operador;

    if (op == "label") {
        out << ins.resultado << ":\n";
    }
    else if (op == "goto") {
        out << "    jmp " << ins.arg2 << "\n";
    }
    else if (op == "ifFalse") {
        std::string asmCond = operandoAASM(ins.arg1);
        out << "    mov rax, " << asmCond << "\n";
        out << "    cmp rax, 0\n";
        out << "    je " << ins.arg2 << "\n";
    }
    else if (op == "print") {
        std::string val = ins.arg1;
        if (!val.empty() && val[0] == '"') {
            out << "    sub rsp, 32\n";
            out << "    lea rcx, [rel fmt_str]\n";
            out << "    lea rdx, [rel " << cadenaLabels[val] << "]\n";
            out << "    call printf\n";
            out << "    add rsp, 32\n";
        }
        else if (ins.tipo == "string") {
            out << "    sub rsp, 32\n";
            out << "    lea rcx, [rel fmt_str]\n";
            out << "    mov rdx, " << operandoAASM(val) << "\n";
            out << "    call printf\n";
            out << "    add rsp, 32\n";
        }
        else if (ins.tipo == "float") {
            std::string asmVal = operandoAASM(val);
            out << "    sub rsp, 32\n";
            out << "    movsd xmm1, " << asmVal << "\n";
            out << "    lea rcx, [rel fmt_float]\n";
            out << "    movq rdx, xmm1\n";
            out << "    call printf\n";
            out << "    add rsp, 32\n";
        }
        else {
            std::string asmVal = operandoAASM(val);
            out << "    sub rsp, 32\n";
            out << "    lea rcx, [rel fmt_int]\n";
            out << "    mov rdx, " << asmVal << "\n";
            out << "    call printf\n";
            out << "    add rsp, 32\n";
        }
    }
    else if (op == "input") {
        std::string d = operandoAASM(ins.arg1);
        if (ins.tipo == "float") {
            out << "    sub rsp, 32\n";
            out << "    lea rcx, [rel fmt_scanf_float]\n";
            out << "    lea rdx, [rel input_buffer]\n";
            out << "    call scanf\n";
            out << "    add rsp, 32\n";
            out << "    movsd xmm0, [rel input_buffer]\n";
            out << "    movsd " << d << ", xmm0\n";
        } else {
            out << "    sub rsp, 32\n";
            out << "    lea rcx, [rel fmt_scanf_int]\n";
            out << "    lea rdx, " << d << "\n";
            out << "    call scanf\n";
            out << "    add rsp, 32\n";
        }
    }
    else if (op == "=" && ins.arg2.empty()) {
        std::string d = operandoAASM(ins.resultado);
        std::string src = ins.arg1;
        if (ins.tipo == "float") {
            if (floatLabels.count(src)) {
                out << "    movsd xmm0, [rel " << floatLabels[src] << "]\n";
            } else if (src == "0" || src == "0.0") {
                out << "    xorpd xmm0, xmm0\n";
            } else if (!src.empty() && isdigit(src[0]) && src.find('.') == std::string::npos) {
                // literal entero asignado a float (ej. celsius = 38)
                out << "    mov rax, " << src << "\n";
                out << "    cvtsi2sd xmm0, rax\n";
            } else if (tipoVariables.count(src) && tipoVariables[src] == "int") {
                // variable entera asignada a float (ej. celsius = c)
                out << "    mov rax, " << operandoAASM(src) << "\n";
                out << "    cvtsi2sd xmm0, rax\n";
            } else {
                out << "    movsd xmm0, " << operandoAASM(src) << "\n";
            }
            out << "    movsd " << d << ", xmm0\n";
        } else if (ins.tipo == "string") {
            if (!src.empty() && src[0] == '"')
                out << "    lea rax, [rel " << cadenaLabels[src] << "]\n";
            else
                out << "    mov rax, " << operandoAASM(src) << "\n";
            out << "    mov " << d << ", rax\n";
        } else {
            out << "    mov rax, " << operandoAASM(src) << "\n";
            out << "    mov " << d << ", rax\n";
        }
    }
    else if (op == "call") {
        out << "    sub rsp, 32\n";
        out << "    call proc_" << ins.arg1 << "\n";
        out << "    add rsp, 32\n";
    }
    else if (op == "neg") {
        if (ins.tipo == "float") {
            out << "    movsd xmm1, " << izq << "\n";
            out << "    xorpd xmm0, xmm0\n";
            out << "    subsd xmm0, xmm1\n";
            out << "    movsd " << dest << ", xmm0\n";
        } else {
            out << "    mov rax, " << izq << "\n";
            out << "    neg rax\n";
            out << "    mov " << dest << ", rax\n";
        }
    }
    else if (op == "not") {
        out << "    mov rax, " << izq << "\n";
        out << "    cmp rax, 0\n";
        out << "    sete al\n";
        out << "    movzx rax, al\n";
        out << "    mov " << dest << ", rax\n";
    }
    else if ((op == "==" || op == "!=") && ins.tipo == "string") {
        if (!ins.arg1.empty() && ins.arg1[0] == '"')
            out << "    lea rcx, [rel " << cadenaLabels[ins.arg1] << "]\n";
        else
            out << "    mov rcx, " << operandoAASM(ins.arg1) << "\n";
        if (!ins.arg2.empty() && ins.arg2[0] == '"')
            out << "    lea rdx, [rel " << cadenaLabels[ins.arg2] << "]\n";
        else
            out << "    mov rdx, " << operandoAASM(ins.arg2) << "\n";
        out << "    sub rsp, 32\n";
        out << "    call strcmp\n";
        out << "    add rsp, 32\n";
        out << "    cmp rax, 0\n";
        if (op == "==") out << "    sete al\n";
        else            out << "    setne al\n";
        out << "    movzx rax, al\n";
        out << "    mov " << dest << ", rax\n";
    }
    else {
        if (ins.tipo == "float") {
            if (floatLabels.count(ins.arg1))
                out << "    movsd xmm0, [rel " << floatLabels[ins.arg1] << "]\n";
            else if (tipoVariables.count(ins.arg1) && tipoVariables[ins.arg1] == "int") {
                out << "    mov rax, " << izq << "\n";
                out << "    cvtsi2sd xmm0, rax\n";
            } else
                out << "    movsd xmm0, " << izq << "\n";

            if (floatLabels.count(ins.arg2))
                out << "    movsd xmm1, [rel " << floatLabels[ins.arg2] << "]\n";
            else if (!ins.arg2.empty() && isdigit(ins.arg2[0]) &&
                     ins.arg2.find('.') == std::string::npos) {
                out << "    mov rax, " << ins.arg2 << "\n";
                out << "    cvtsi2sd xmm1, rax\n";
            } else if (tipoVariables.count(ins.arg2) && tipoVariables[ins.arg2] == "int") {
                out << "    mov rax, " << der << "\n";
                out << "    cvtsi2sd xmm1, rax\n";
            } else
                out << "    movsd xmm1, " << der << "\n";

            if      (op == "+") out << "    addsd xmm0, xmm1\n";
            else if (op == "-") out << "    subsd xmm0, xmm1\n";
            else if (op == "*") out << "    mulsd xmm0, xmm1\n";
            else if (op == "/") out << "    divsd xmm0, xmm1\n";
            else if (op == "%") {
                out << "    sub rsp, 32\n";
                out << "    call fmod\n";
                out << "    add rsp, 32\n";
            }
            else if (op == ">" || op == "<" || op == ">=" ||
                     op == "<=" || op == "==" || op == "!=") {
                out << "    ucomisd xmm0, xmm1\n";
                if      (op == ">")  out << "    seta al\n";
                else if (op == "<")  out << "    setb al\n";
                else if (op == ">=") out << "    setae al\n";
                else if (op == "<=") out << "    setbe al\n";
                else if (op == "==") out << "    sete al\n";
                else if (op == "!=") out << "    setne al\n";
                out << "    movzx rax, al\n";
                out << "    mov " << dest << ", rax\n";
                return;
            }
            out << "    movsd " << dest << ", xmm0\n";
        }
        else {
            if (op == "+") {
                out << "    mov rax, " << izq << "\n";
                out << "    add rax, " << der << "\n";
                out << "    mov " << dest << ", rax\n";
            }
            else if (op == "-") {
                out << "    mov rax, " << izq << "\n";
                out << "    sub rax, " << der << "\n";
                out << "    mov " << dest << ", rax\n";
            }
            else if (op == "*") {
                out << "    mov rax, " << izq << "\n";
                out << "    imul rax, " << der << "\n";
                out << "    mov " << dest << ", rax\n";
            }
            else if (op == "/") {
                out << "    mov rax, " << izq << "\n";
                out << "    cqo\n";
                out << "    mov rbx, " << der << "\n";
                out << "    idiv rbx\n";
                out << "    mov " << dest << ", rax\n";
            }
            else if (op == "%") {
                out << "    mov rax, " << izq << "\n";
                out << "    cqo\n";
                out << "    mov rbx, " << der << "\n";
                out << "    idiv rbx\n";
                out << "    mov " << dest << ", rdx\n";
            }
            else if (op == ">" || op == "<" || op == ">=" ||
                     op == "<=" || op == "==" || op == "!=") {
                out << "    mov rax, " << izq << "\n";
                out << "    cmp rax, " << der << "\n";
                if      (op == ">")  out << "    setg al\n";
                else if (op == "<")  out << "    setl al\n";
                else if (op == ">=") out << "    setge al\n";
                else if (op == "<=") out << "    setle al\n";
                else if (op == "==") out << "    sete al\n";
                else if (op == "!=") out << "    setne al\n";
                out << "    movzx rax, al\n";
                out << "    mov " << dest << ", rax\n";
            }
            else if (op == "&&") {
                out << "    mov rax, " << izq << "\n";
                out << "    cmp rax, 0\n";
                out << "    setne al\n";
                out << "    movzx rax, al\n";
                out << "    mov rbx, " << der << "\n";
                out << "    cmp rbx, 0\n";
                out << "    setne bl\n";
                out << "    movzx rbx, bl\n";
                out << "    and rax, rbx\n";
                out << "    mov " << dest << ", rax\n";
            }
            else if (op == "||") {
                out << "    mov rax, " << izq << "\n";
                out << "    cmp rax, 0\n";
                out << "    setne al\n";
                out << "    movzx rax, al\n";
                out << "    mov rbx, " << der << "\n";
                out << "    cmp rbx, 0\n";
                out << "    setne bl\n";
                out << "    movzx rbx, bl\n";
                out << "    or rax, rbx\n";
                out << "    mov " << dest << ", rax\n";
            }
        }
    }
}

void GeneradorASM::generarArchivo(const std::string& nombreArchivo) {
    std::ostringstream out;

    std::vector<std::string> cadenas;
    int cadenaIdx = 0;
    std::map<std::string, std::string> cadenaLabels;
    std::map<std::string, std::string> floatLabels;
    int floatIdx = 0;

    for (auto& ins : instrucciones) {
        auto recolectarCadena = [&](const std::string& val) {
            if (!val.empty() && val[0] == '"') {
                if (cadenaLabels.find(val) == cadenaLabels.end()) {
                    std::string label = "str" + std::to_string(cadenaIdx++);
                    cadenaLabels[val] = label;
                    cadenas.push_back(val);
                }
            }
        };
        auto recolectarFloat = [&](const std::string& val, const std::string& tipo) {
            if (tipo == "float" && !val.empty() &&
                (isdigit(val[0]) || val[0] == '-') &&
                val.find('.') != std::string::npos) {
                if (floatLabels.find(val) == floatLabels.end()) {
                    floatLabels[val] = "flt" + std::to_string(floatIdx++);
                }
            }
        };
        recolectarCadena(ins.arg1);
        recolectarCadena(ins.arg2);
        recolectarFloat(ins.arg1, ins.tipo);
        recolectarFloat(ins.arg2, ins.tipo);
        if (ins.operador == "=" && ins.tipo == "float")
            recolectarFloat(ins.arg1, ins.tipo);
    }

    // SECCIÓN .data
    out << "section .data\n";
    out << "    fmt_int         db \"%d\", 10, 0\n";
    out << "    fmt_float       db \"%.6g\", 10, 0\n";
    out << "    fmt_scanf_float db \"%lf\", 0\n";
    out << "    fmt_scanf_int   db \"%d\", 0\n";
    out << "    fmt_str         db \"%s\", 10, 0\n";
    out << "    input_buffer    dq 0\n";

    cadenaIdx = 0;
    for (auto& cad : cadenas) {
        std::string contenido = cad.substr(1, cad.size() - 2);
        out << "    " << cadenaLabels[cad] << " db \"" << contenido << "\", 0\n";
    }
    for (auto& fl : floatLabels) {
        out << "    " << fl.second << " dq " << fl.first << "\n";
    }

    // SECCIÓN .bss: variables globales (accesibles desde main y procedimientos)
    if (!variablesGlobales.empty()) {
        out << "\nsection .bss\n";
        for (auto& g : variablesGlobales) {
            out << "    " << g << " resq 1\n";
        }
    }

    // SECCIÓN .text
    out << "\nsection .text\n";
    out << "    extern printf, scanf, strcmp, fmod\n";
    out << "    global main\n\n";
    out << "main:\n";
    out << "    push rbp\n";
    out << "    mov rbp, rsp\n";
    int espacioStack = ((stackOffset + 15) / 16) * 16;
    if (espacioStack < 64) espacioStack = 64;
    out << "    sub rsp, " << espacioStack << "\n";
    out << "    jmp __inicio\n\n";

    // ── PASADA 1: solo procedimientos ──────────────────
    bool enProc = false;
    for (auto& ins : instrucciones) {
        if (ins.operador == "proc_inicio") {
            enProc = true;
            out << "\n" << ins.resultado << ":\n";
            out << "    push rbp\n";
            out << "    mov rbp, rsp\n";
            int procBytes = procStackSizes.count(ins.resultado)
                            ? procStackSizes[ins.resultado] : 8;
            int procAligned = ((procBytes + 15) / 16) * 16;
            if (procAligned < 32) procAligned = 32;
            out << "    sub rsp, " << procAligned << "\n";
        }
        else if (ins.operador == "proc_fin") {
            enProc = false;
            out << "    leave\n";
            out << "    ret\n";
        }
        else if (enProc) {
            emitirInstruccion(out, ins, cadenaLabels, floatLabels);
        }
    }

   // ── PASADA 2: código principal ──────────────────────
out << "\n__inicio:\n";
bool dentroProc2 = false;
for (auto& ins : instrucciones) {
    if (ins.operador == "proc_inicio") { 
        dentroProc2 = true; 
        continue; 
    }
    if (ins.operador == "proc_fin") { 
        dentroProc2 = false; 
        continue; 
    }
    if (dentroProc2) continue;

    emitirInstruccion(out, ins, cadenaLabels, floatLabels);
}

    out << "\n    mov rax, 0\n";
    out << "    leave\n";
    out << "    ret\n";

    std::ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cout << "Error: no se pudo crear " << nombreArchivo << "\n";
        return;
    }
    archivo << out.str();
    archivo.close();
    std::cout << "\nArchivo ASM generado: " << nombreArchivo << "\n";
}