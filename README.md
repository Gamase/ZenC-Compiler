# ZenC Compiler

A custom compiled programming language built from scratch in C++. ZenC features a complete compiler pipeline that transforms source code into native x86-64 Windows executables.

##  Compiler Pipeline
Source Code (.txt)
↓
Lexer        → Tokenization
↓
Parser       → Abstract Syntax Tree (AST)
↓
Semantic      → Type checking & validation
Analysis
↓
Interpreter    → Direct execution
↓
TAC          → Three Address Code (Intermediate)
↓
ASM Generator  → x86-64 NASM Assembly
↓
output.asm → output.obj → output.exe

## Features

- **Data types:** `int`, `float`, `double`, `char`, `string`, `bool`
- **Control flow:** `if`, `else`, `while`, `for`
- **Built-in functions:** `print()`, `input()`
- **Procedures:** `void` functions with calls
- **Operators:**
  - Arithmetic: `+`, `-`, `*`, `/`, `%`
  - Relational: `>`, `<`, `>=`, `<=`, `==`, `!=`
  - Logical: `&&`, `||`, `!`
  - Assignment: `=`
- **Comments:** `//`
- **Mixed int/float arithmetic** with automatic type promotion
- **Semantic error detection** with clear messages
- **TAC intermediate code** with type annotations
- **Native x86-64 ASM generation** for Windows

##  Language Syntax

```c
// Variable declaration
int x = 5;
float y = 2.5;
string name = "ZenC";

// If / else
if (x > 3) {
    print("x is greater than 3");
} else {
    print("x is less or equal to 3");
}

// While loop
int i = 0;
while (i < 5) {
    print(i);
    i = i + 1;
}

// For loop
for (int j = 0; j < 3; j = j + 1) {
    print(j);
}

// Procedures
void greet() {
    print("Hello from ZenC!");
}
greet();

// Input
int num = 0;
input(num);
print(num);

// Modulo
int resto = 10 % 3;
print(resto);

// Logical operators
if (x > 3 && y < 5.0) {
    print("both true");
}
```

##  Requirements

- **C++ compiler:** g++ (MinGW recommended)
- **Assembler:** NASM 2.16+
- **Linker:** GCC (MinGW)
- **OS:** Windows x64

##  Build & Run

### 1. Compile the ZenC compiler
```bash
g++ src/*.cpp -Iinclude -o mi_compilador.exe
```

### 2. Write your ZenC code in `input.txt`

### 3. Run the compiler
```bash
.\mi_compilador.exe
```
This generates:
- TAC intermediate code in the console
- `output.asm` — x86-64 assembly file

### 4. Assemble and link
```bash
nasm -f win64 output.asm -o output.obj
gcc output.obj -o output.exe
```

### 5. Run your program
```bash
.\output.exe
```

## 📁 Project Structure
ZenC-Compiler/
├── include/
│   ├── ast.h           # Abstract Syntax Tree nodes
│   ├── lexer.h         # Lexer interface
│   ├── parser.h        # Parser interface
│   ├── interprete.h    # Interpreter interface
│   ├── semantica.h     # Semantic analysis interface
│   ├── tabla.h         # Symbol table
│   ├── token.h         # Token definitions
│   ├── codegen.h       # TAC code generator
│   └── asmgen.h        # ASM code generator
├── src/
│   ├── main.cpp        # Entry point
│   ├── lexer.cpp       # Tokenizer
│   ├── parser.cpp      # Syntax analysis & AST builder
│   ├── interprete.cpp  # Tree-walk interpreter
│   ├── semantica.cpp   # Type checker
│   ├── codegen.cpp     # TAC generator
│   └── asmgen.cpp      # x86-64 ASM generator
└── input.txt           # ZenC source code input

##  TAC Example

Given this ZenC code:
```c
int x = 5;
float y = 2.5;
float result = x + y;
print(result);
```

The compiler generates this TAC:
x = 5 [int]
y = 2.5 [float]
result = 0 [float]
t1 = x + y [float]
result = t1 [float]
print result [float]

And this x86-64 ASM:
```asm
mov rax, 5
mov [rbp - 8], rax
movsd xmm0, [rel flt0]
movsd [rbp - 16], xmm0
xorpd xmm0, xmm0
movsd [rbp - 24], xmm0
mov rax, [rbp - 8]
cvtsi2sd xmm0, rax
movsd xmm1, [rbp - 16]
addsd xmm0, xmm1
movsd [rbp - 32], xmm0
```

##  Error Handling

ZenC provides clear error messages:
Error: variable 'x' already declared
Error: variable 'y' not declared
Error semantico: cannot assign float to int in variable 'z'

##  Author

**Gamase** — Built as a school compiler project
