# TinyLang Web Compiler

A beautiful, web-based LL(1) compiler and grammar analysis tool for TinyLang.

## Features

*   **Lexical Analysis:** Breaks down source code into tokens (Keywords, Identifiers, Operators, Literals).
*   **LL(1) Grammar Lab:** Interactively analyze formal grammars, compute FIRST & FOLLOW sets, build parse tables, and detect left recursion and LL(1) conflicts.
*   **Predictive Parsing:** Stack-based LL(1) predictive parser with step-by-step trace visualization.
*   **Abstract Syntax Tree (AST):** Interactive, graphical tree visualization of parsed code.
*   **Symbol Table:** Tracks variables, their types, scopes, and reference counts.
*   **Static Code Analysis:** Estimates Big-O time complexity and detects potential hotspots (e.g., deep nesting).
*   **Code Optimization:** Applies constant folding, dead code elimination, and loop-invariant code motion.

## Setup & Running

### Prerequisites

*   **C Compiler** (`gcc` or `clang`)
*   **Python 3.8+**
*   **Flask** (`pip install flask`)

### 1. Build the C Compiler Core

The web app is a frontend that communicates with a fast C backend. You must compile the C code first.

**On Mac/Linux:**
```bash
make
```

**On Windows:**
```cmd
build.bat
```

### 2. Install Python Dependencies

```bash
pip install flask
```

### 3. Run the Web Server

```bash
python app.py
```

Navigate to [http://localhost:5000](http://localhost:5000) in your web browser.
