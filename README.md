# Mini C-Like Compiler  
**Course Assignment – Compiler Construction**

This project implements a small compiler for a simplified C-style language.  
It includes all major compilation phases:

1. **Lexer (Scanner)**  
2. **Parser (AST builder)**  
3. **Semantic Analyzer**  
4. **Intermediate Representation (IR) Generator**  
5. **Optimizer**  
6. **Code Generator**  
7. **Interpreter (Execution of IR)**  

The compiler supports both **file input (.txt)** and **interactive terminal mode**.

---

## ✅ Features Implemented

### **1. Lexical Analysis (Lexer)**
- Tokenizes input source code  
- Supports:
  - identifiers  
  - numbers  
  - strings  
  - operators: `+, -, *, /, =`  
  - `cin()` and `cout()`  
  - parentheses  
  - semicolons  
- Supports comments:
  - `// single-line`
  - `/* multi-line */`
- Tracks line numbers for accurate error reporting

---

### **2. Parsing (Parser)**
Builds an **Abstract Syntax Tree (AST)** for:
- arithmetic expressions  
- string concatenation  
- variable assignments  
- cout()  
- cin()  
- unary minus  
- parentheses  
- full statement parsing  
- error recovery (skips to next `;` when an error occurs)

---

### **3. Semantic Analysis**
Checks:
- use of undeclared variables  
- type consistency (number vs string)  
- division by zero (static cases if possible)  
- reports errors with correct line numbers  
- continues after errors  

---

### **4. Intermediate Representation (IR)**
Example IR instructions generated:

MOV 10 -> R1
MOV 5 -> R2
ADD R1, R2 -> R3
STORE R3 -> x
PRINT x


The IR is shown in the console under:

=== Generating IR ===

---

### **5. Optimizer**
Performs simple optimizations:
- constant folding  
  - `2 + 3` → `5`  
- dead code elimination (where possible)  
- redundant MOV removal  

---

### **6. Code Generator**
Produces a pseudo assembly-like output:

LOAD x -> t1
LOAD y -> t2
ADD t1, t2 -> t3
STORE t3 -> z
PRINT z

---

### **7. Interpreter**
Executes the final AST (or simplified IR):
- correct arithmetic  
- string operations  
- cin input  
- cout output  
- graceful runtime errors:
  - division by zero  
  - undefined variable  
  - invalid operations  
- execution continues after errors  

---

## 📂 Project Folder Structure

c++compiler/
│
├──.vscode
  ├──settings.json
├── include/
│ ├── lexer.h
│ ├── parser.h
│ ├── token.h
│ ├── semantic.h
│ ├── ir.h
│ ├── optimizer.h
│ ├── codegen.h
│ └── interpreter.h
│
├── src/
│ ├── lexer.cpp
│ ├── parser.cpp
│ ├── semantic.cpp
│ ├── ir.cpp
│ ├── optimizer.cpp
│ ├── codegen.cpp
│ ├── interpreter.cpp
│ └── main.cpp
│
├── tests/
│ ├── test1.txt
│ ├── test2.txt
│ ├── test3.txt
│ ├── test4.txt
│ └── test5.txt
│
└── compiler.exe (after build)


---

## 🔧 Build Instructions

On Windows (MinGW/G++):

```bash
g++ src/main.cpp src/lexer.cpp src/parser.cpp src/interpreter.cpp src/ir.cpp src/semantic.cpp src/optimizer.cpp src/codegen.cpp -Iinclude -o compiler

This produces:
compiler.exe

Running The Compiler
1. Run a single test file
./compiler tests/test1.txt
2. Run all tests in /tests folder
./compiler

